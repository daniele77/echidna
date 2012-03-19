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

#ifndef ECHIDNA_APPLICATION_H_
#define ECHIDNA_APPLICATION_H_

#include "configuration.h"
#include "container.h"

namespace echidna
{


class Application
{
public:
    Application( int argc, char* argv[] );
    void Loop();
private:
    static std::auto_ptr< Configuration > GetCfg( int argc, char* argv[] );
    Container container;
};


// ########## implementation ###########

inline Application::Application( int argc, char* argv[] ) :
    container( GetCfg( argc, argv ) )
{
}

inline void Application::Loop()
{
    container.Run();
}

inline std::auto_ptr< Configuration > Application::GetCfg( int argc, char* argv[] )
{
    using namespace std;

    auto_ptr< Configuration > cfg( new Configuration() );

    if ( argc > 1 )
    {
        for ( int i = 1; i < argc; ++i )
            cfg -> LoadFile( argv[ i ] );
    }

    return cfg;
}

} // namespace echidna

#endif // ECHIDNA_APPLICATION_H_
