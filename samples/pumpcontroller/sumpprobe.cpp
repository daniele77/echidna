/*******************************************************************************
 * echidna - A framework for event based reactive systems
 * Copyright (C) 2012 Daniele Pallastrelli 
 *
 *
 * This file is part of echidna.
 * For more information, see http://echidna.googlecode.com/
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 ******************************************************************************/

#include "echidna/component.h"
#include "levelstatechanged.h"
#include "probeerror.h"
#include "drainevent.h"

using namespace std;


DEFINE_COMPONENT( SumpProbe )
{
public:
    SumpProbe();
    virtual ~SumpProbe();
    virtual void Init( const string& instanceName, CfgPtr cfg, EventBusPtr bus );
    virtual void Start();
    virtual void Stop();
private:
    void LevelChanged( const LevelStateChanged& event );
    void Recalculate();
    void MustDrain( bool md );

    EventBusPtr bus;
    bool mustDrain;
    bool lowLevel;
    bool highLevel;
};

SumpProbe::SumpProbe()
{
}

SumpProbe::~SumpProbe()
{
}

void SumpProbe::Init( const string& instanceName, CfgPtr cfg, EventBusPtr _bus )
{
    bus = _bus;

    bus -> Subscribe< LevelStateChanged >( 
        boost::bind( &SumpProbe::LevelChanged, this, _1 ),
        boost::bind( &LevelStateChanged::id, _1 ) == instanceName
    );
}

void SumpProbe::Start()
{
}

void SumpProbe::Stop()
{
}

void SumpProbe::LevelChanged( const LevelStateChanged& event )
{
    ( event.isLowerSensor ? lowLevel : highLevel ) = event.submerged;

    Recalculate();
}

void SumpProbe::Recalculate()
{
    if ( ! lowLevel && highLevel )
        bus -> Post( ProbeError() );
    else if ( ! lowLevel && ! highLevel )
        MustDrain( false );
    else if ( lowLevel && highLevel )
        MustDrain( true );
}

void SumpProbe::MustDrain( bool md )
{
    if ( mustDrain != md )
    {
        mustDrain = md;
        bus -> Post( DrainEvent( mustDrain ) );
    }
}