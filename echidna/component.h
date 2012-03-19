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

#ifndef ECHIDNA_COMPONENT_H_
#define ECHIDNA_COMPONENT_H_

#include <boost/shared_ptr.hpp>
#include "class.h"
#include "configuration.h"
#include "eventbus.h"

namespace echidna
{

class Component
{
public:
    typedef boost::shared_ptr< const Configuration > CfgPtr;
    typedef boost::shared_ptr< EventBus > EventBusPtr;

    virtual ~Component() {}
    virtual void Init( const std::string& instanceName, CfgPtr cfg, EventBusPtr msgBroker ) = 0;
    virtual void Start() = 0;
    virtual void Stop() = 0;
};

} // namespace echidna

#define DEFINE_COMPONENT( c ) \
    REG_CLASS( c, echidna::Component, void )

#endif // ECHIDNA_COMPONENT_H_
