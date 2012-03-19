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
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include "echidna/component.h"

using namespace std;

DEFINE_COMPONENT( Interpreter )
{
public:
    Interpreter();
    virtual ~Interpreter();
    virtual void Init( const std::string& instanceName, CfgPtr cfg, EventBusPtr msgBroker );
    virtual void Start();
    virtual void Stop();
private:
    void PrintCmdList();
};

Interpreter::Interpreter()
{
    cout << "Interpreter built. Instance " << this << endl;
}

Interpreter::~Interpreter()
{
    cout << "Interpreter destroyed. Instance " << this << endl;
}

void Interpreter::Init( const string& instanceName, CfgPtr cfg, EventBusPtr msgBroker )
{
    boost::lambda::placeholder1_type arg;
    msgBroker -> Subscribe< string >( 
        boost::bind( &Interpreter::PrintCmdList, this ),
        //cout << boost::lambda::constant( "received cmd: " ) << arg << '\n',
        arg == "help"
    );
    cout << "Interpreter initialized. Instance " << this << " instance name " << instanceName << endl;
}

void Interpreter::Start()
{
    cout << "Interpreter started. Instance " << this << endl;
}

void Interpreter::Stop()
{
    cout << "Interpreter stopped. Instance " << this << endl;
}

void Interpreter::PrintCmdList()
{
    cout << "Commands:" << endl;
    cout << "\thelp" << endl;
    cout << "\texit" << endl;
    cout << endl;
}
