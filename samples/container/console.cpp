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
#include "echidna/exit.h"
#include "token.h"

using namespace std;


DEFINE_COMPONENT( Console )
{
public:
    Console();
    virtual ~Console();
    virtual void Init( const std::string& instanceName, CfgPtr cfg, EventBusPtr msgBroker );
    virtual void Start();
    virtual void Stop();
private:
    void WaitInput( const Token& );
    EventBusPtr broker;
};

Console::Console()
{
    cout << "Console built. Instance " << this << endl;
}

Console::~Console()
{
    cout << "Console destroyed. Instance " << this << endl;
}

void Console::Init( const std::string& instanceName, CfgPtr cfg, EventBusPtr msgBroker )
{
    broker = msgBroker;
    broker -> Subscribe< Token >( boost::bind( &Console::WaitInput, this, _1 ) );
    cout << "Console initialized. Instance " << this << " instance name " << instanceName << endl;
}

void Console::Start()
{
    cout << "Console started. Instance " << this << endl;
    broker -> Post( Token() );
}

void Console::Stop()
{
    cout << "Console stopped. Instance " << this << endl;
}

void Console::WaitInput( const Token& )
{
    // print the prompt:
    cout << "> ";
    // wait for a command:
    string line;
    getline( cin, line );
    // check for shutdown:
    if ( line == "exit" )
        broker -> Post( echidna::event::Exit() );
    else
    {
        // send a std::string event with the command
        broker -> Post( line );
        // send a Token event to schedule again the Console::WaitInput method
        broker -> Post( Token() );
    }
}
