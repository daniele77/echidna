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

#ifndef ECHIDNA_CONTAINER_H_
#define ECHIDNA_CONTAINER_H_

#include <stdexcept>
#include <memory>
#include <boost/shared_ptr.hpp>
#include <vector>
#include "eventbus.h"
#include "component.h"
#include "exit.h"


namespace echidna
{

// forward declarations:
class Configuration;


class CfgError : public std::runtime_error
{
public:
    CfgError( const std::string& _symbolNotFound ) :
        std::runtime_error( "Key " + _symbolNotFound + " missing in configuration" ),
        symbolNotFound( _symbolNotFound )
    {}
    virtual ~CfgError() throw() {}
private:
    const std::string symbolNotFound;
};

class MissingComponentError : public std::runtime_error
{
public:
    MissingComponentError( const std::string& _classMissing ) :
        std::runtime_error( "Component " + _classMissing + " not found in the software" ),
        classMissing( _classMissing )
    {}
    virtual ~MissingComponentError() throw() {}
private:
    const std::string classMissing;
};


class Container
{
public:

    Container( std::auto_ptr< Configuration > cfg ) throw ( CfgError, MissingComponentError );
    virtual ~Container();
    void Run();
    void Stop( int code );
    void StopFromInside( event::Exit );

private:

    typedef boost::shared_ptr< const Configuration > CfgConstPtr;

    void LoadComponents( CfgConstPtr cfg ) throw ( CfgError, MissingComponentError );
    void CreateComponent( const std::string& instanceName, const Configuration& componentCfg, CfgConstPtr cfg )
        throw ( CfgError, MissingComponentError );

    volatile bool running;
    boost::shared_ptr< EventBus > broker;
    std::vector< boost::shared_ptr< Component > > components;
};


// ########## implementation ###########


inline Container::Container( std::auto_ptr< Configuration > _cfg )
    throw ( CfgError, MissingComponentError ) :
    running( false ),
    broker( new EventBus() )
{
    // utilizza uno shared_ptr in modo che quando tutti i componenti hanno
    // letto la configurazione, può essere rilasciata dalla memoria
    CfgConstPtr cfg( _cfg );

    // inserisce l'evento di terminazione
    broker -> Subscribe< event::Exit >( boost::bind( &Container::StopFromInside, this, _1 ) );

    // crea i componenti e li inizializza
    LoadComponents( cfg );
}

inline Container::~Container()
{
    Stop( 0 );
}

inline void Container::Run()
{
    if ( running )
        return;

    for_each(
        components.begin(),
        components.end(),
        boost::bind( &Component::Start, _1 )
    );

    running = true;
    while ( running )
        broker -> Run();

}

inline void Container::Stop( int /*code*/ )
{
    if ( !running )
        return;

    running = false;
    broker -> Stop();

    for_each(
        components.begin(),
        components.end(),
        boost::bind( &Component::Stop, _1 )
    );
}

inline void Container::StopFromInside( event::Exit e )
{
    Stop( e.code );
}

inline void Container::LoadComponents( CfgConstPtr cfg )
    throw ( CfgError, MissingComponentError )
{
    Configuration subtree = cfg -> GetChild( "components" );
    subtree.Iterate( boost::bind( &Container::CreateComponent, this, _1, _2, cfg ) );
}

inline void Container::CreateComponent( const std::string& instanceName, const Configuration& componentCfg, CfgConstPtr cfg )
    throw ( CfgError, MissingComponentError )
{
    using namespace std;

    try
    {
        // tipo della classe da caricare:
        const string className = componentCfg.Get< string >( "class" );  // tira una range_error se il parametro "class" non esiste
        // crea il descrittore della classe da utilizzare:
        Class< Component, void > c = Class< Component, void >::ForName( className );
        // crea un'istanza:
        auto_ptr< Component > instance = c.NewInstance();
        // se la classe non e' stata trovata nel registro significa che non e' stata linkata!
        if ( instance.get() == NULL )
            throw MissingComponentError( className );
        else
        {
            boost::shared_ptr< Component > c( instance );
            components.push_back( c );
            c -> Init( instanceName, cfg, broker );
        }
    }
    catch ( const range_error& )
    {
        throw CfgError( instanceName );
    }
}


} // namespace echidna

#endif // ECHIDNA_CONTAINER_H_
