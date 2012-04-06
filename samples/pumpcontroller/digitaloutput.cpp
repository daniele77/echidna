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
#include "out.h"

using namespace std;

DEFINE_COMPONENT( DigitalOutput )
{
public:
    DigitalOutput();
    virtual ~DigitalOutput();
    virtual void Init( const string& instanceName, CfgPtr cfg, EventBusPtr bus );
    virtual void Start();
    virtual void Stop();
};

DigitalOutput::DigitalOutput()
{
}

DigitalOutput::~DigitalOutput()
{
}

void DigitalOutput::Init( const string& instanceName, CfgPtr cfg, EventBusPtr bus )
{
    bus -> Subscribe< Out >( boost::bind( &DigitalOutput::Write, this, _1 ) );
}

void DigitalOutput::Start()
{
}

void DigitalOutput::Stop()
{
}

void DigitalOutput::Write( const Out& out )
{
    cout << out << endl;
}