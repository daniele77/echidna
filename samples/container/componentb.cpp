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

#include <iostream>
#include "echidna/component.h"

using namespace std;

DEFINE_COMPONENT( ComponentB )
{
public:
    ComponentB();
    virtual ~ComponentB();
    virtual void Init( const std::string& instanceName, CfgPtr cfg, EventBusPtr msgBroker );
    virtual void Start();
    virtual void Stop();
};

ComponentB::ComponentB()
{
    cout << "ComponentB built. Instance " << this << endl;
}

ComponentB::~ComponentB()
{
    cout << "ComponentB destroyed. Instance " << this << endl;
}

void ComponentB::Init( const std::string& instanceName, CfgPtr cfg, EventBusPtr msgBroker )
{
    cout << "ComponentB initialized. Instance " << this << " instance name " << instanceName << endl;
}

void ComponentB::Start()
{
    cout << "ComponentB started. Instance " << this << endl;
}

void ComponentB::Stop()
{
    cout << "ComponentB stopped. Instance " << this << endl;
}
