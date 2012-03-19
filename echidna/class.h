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

#ifndef ECHIDNA_CLASS_H_
#define ECHIDNA_CLASS_H_

#include <string>
#include <map>
#include <memory>
#include <list>

namespace echidna
{

template < class I, class P > class Registry;
template < class I, class P > class RegEntry;

// è il descrittore di una classe con un costruttore
// che prende un parametro di tipo P
template < class I, class P >
class Class
{
    public :

        typedef std::auto_ptr< I > (*FactoryMethod)( P ) ;

        typedef std::list< Class< I, P > > ClassSet;
        std::auto_ptr< I > NewInstance( P p )
        {
            if( fm != NULL )
                return( fm( p ) ) ;
            else
                return( std::auto_ptr< I >() ) ;
        }
        const std::string& Name() const { return name; }
        static ClassSet All()
        {
            return Reg().All();
        }
        static Class ForName( const std::string& name )
        {
            return Reg().ForName( name );
        }
    private :
        static Registry< I, P >& Reg()
        {
            static Registry< I, P > reg;
            return reg;
        }
        const std::string name ;
        FactoryMethod fm;
        friend class Registry< I, P >;
        friend class RegEntry< I, P >;
        static void Register( const std::string& s, FactoryMethod m )
        {
            Reg().Add( Class( s, m ) );
        }
        Class() : name( "" ), fm( NULL )
        {
        }
        Class( const std::string& s, FactoryMethod m ) :
            name( s ), fm( m )
        {
        }
};

// specializzazione della classe Class nel caso di P=void
// (per le classi con il costruttore di default)
template < class I >
class Class< I, void >
{
    public :

        typedef std::auto_ptr< I > (*FactoryMethod)() ;

        typedef std::list< Class< I, void > > ClassSet;
        std::auto_ptr< I > NewInstance()
        {
            if( fm != NULL )
                return( fm() ) ;
            else
                return( std::auto_ptr< I >() ) ;
        }
        const std::string& Name() const { return name; }
        static ClassSet All()
        {
            return Reg().All();
        }
        static Class ForName( const std::string& name )
        {
            return Reg().ForName( name );
        }
    private :
        static Registry< I, void >& Reg()
        {
            static Registry< I, void > reg;
            return reg;
        }
        const std::string name ;
        FactoryMethod fm;
        friend class Registry< I, void >;
        friend class RegEntry< I, void >;
        static void Register( const std::string& s, FactoryMethod m )
        {
            Reg().Add( Class( s, m ) );
        }
        Class() : name( "" ), fm( NULL )
        {
        }
        Class( const std::string& s, FactoryMethod m ) :
            name( s ), fm( m )
        {
        }
};

template< class T, class I, const char S[], class P >
class Registered
{
    public :
        static std::auto_ptr< I > NewInstance( P p )
        {
            return( std::auto_ptr< I >( new T( p ) ) ) ;
        }
    private :
        static const RegEntry< I, P > r ;
};

template< class T, class I, const char S[] >
class Registered< T, I, S, void >
{
    public :
        static std::auto_ptr< I > NewInstance()
        {
            return( std::auto_ptr< I >( new T() ) ) ;
        }
    private :
        static const RegEntry< I, void > r ;
};

template < class I, class P >
class Registry
{
public:

    typedef typename Class< I, P >::ClassSet ClassSet;

    void Add( Class< I, P > c )
    {
        classes.insert( std::make_pair( c.Name(), c ) );
    }
    Class< I, P > ForName( const std::string& name ) const
    {
        typename ClassMap::const_iterator i = classes.find( name );
        if ( i == classes.end() )
            return Class< I, P >();  // valore di default
        else
            return i -> second ;
    }
    ClassSet All() const
    {
        ClassSet result;
        for ( typename ClassMap::const_iterator i = classes.begin(); i != classes.end(); ++i )
            result.push_back( i -> second );
        return result;
    }

private:
    typedef std::map< std::string, Class< I, P > > ClassMap;
    friend class Class< I, P >;
    Registry() {}
    ClassMap classes;
};




template < class I, class P >
class RegEntry
{
    public :
        typedef typename Class< I, P >::FactoryMethod FactoryMethod;
        RegEntry( const char s[], FactoryMethod f )
        {
            Class< I, P >::Register( s, f );
        }
};

template< class T, class I, const char S[], class P > const RegEntry< I, P >
Registered< T, I, S, P > :: r = RegEntry< I, P >( S, Registered< T, I, S, P >::NewInstance ) ;

template< class T, class I, const char S[] > const RegEntry< I, void >
Registered< T, I, S, void > :: r = RegEntry< I, void >( S, Registered< T, I, S, void >::NewInstance ) ;

#define REG_CLASS( C, I, P ) \
char C##Name__[] = #C ; \
class C ; \
template class echidna::Registered< C, I, C##Name__, P > ; \
class C : public echidna::Registered< C, I, C##Name__, P >, public I

} // namespace echidna


#endif // ECHIDNA_CLASS_H_
