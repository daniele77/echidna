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

using namespace std;


DEFINE_COMPONENT( LevelSensor )
{
public:
    LevelSensor();
    virtual ~LevelSensor();
    virtual void Init( const string& instanceName, CfgPtr cfg, EventBusPtr msgBroker );
    virtual void Start();
    virtual void Stop();
};

LevelSensor::LevelSensor()
{
}

LevelSensor::~LevelSensor()
{
}

void LevelSensor::Init( const string& instanceName, CfgPtr cfg, EventBusPtr msgBroker )
{
}

void LevelSensor::Start()
{
}

void LevelSensor::Stop()
{
}

