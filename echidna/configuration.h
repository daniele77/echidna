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
 
#ifndef ECHIDNA_CONFIGURATION_H_
#define ECHIDNA_CONFIGURATION_H_

#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace echidna
{

// E' in grado acquisire un insieme di file di configurazione,
// effettuare una verifica e restituire le proprietà contenute
class Configuration
{
public:

    typedef boost::function<void (const std::string& key, const Configuration& subTree)> Visitor;

    // costruisce una configurazione vuota
    Configuration();

    virtual ~Configuration();

    // Aggiunge il file specificato alla configurazione.
    // Tira l'eccezione range_error se il nuovo file contiene delle chiavi di
    // primo livello già presenti nei file caricati finora.
    void LoadFile( const std::string& fileName ) throw ( std::range_error );

    // Restituisce il valore della configurazione corrispondente alla chiave key
    // tira l'eccezione range_error se l'elemento non esiste
    template < typename T >
    T Get( const std::string& key ) const throw ( std::range_error );

    Configuration GetChild( const std::string& path ) const throw ( std::range_error );

    // Itera su tutti gli elementi di primo livello dell'albero corrente e per ognuno invoca
    // il funtore passato come argomento.
    void Iterate( Visitor visitorFunctor ) const;


private:

    explicit Configuration( const boost::property_tree::ptree& t );

    // Ritorna true se toCheck contiene chiavi di primo livello già contenuti in tree
    bool HasIntersections( const boost::property_tree::ptree& toCheck ) const;

    boost::property_tree::ptree tree;
};


template < typename T >
inline T Configuration::Get( const std::string& key ) const throw ( std::range_error )
{
    try
    {
        return tree.get< T >( key );
    }
    catch ( const boost::property_tree::ptree_error& e )
    {
        throw std::range_error( e.what( ) );
    }
}

// ########## implementation ###########

inline Configuration::Configuration()
{
    // non fa niente
}

inline Configuration::~Configuration()
{
    // non fa niente
}

inline  Configuration::Configuration( const boost::property_tree::ptree& t ) :
    tree( t )
{
}

inline void Configuration::LoadFile( const std::string& fileName ) throw ( std::range_error )
{
    using namespace boost::algorithm;
    using namespace boost::property_tree;

    try
    {
        ptree tmp;

        // cerco di capire il formato del file in base all'estensione:
        if ( iends_with( fileName, ".xml" ) )
            read_xml( fileName, tmp, xml_parser::no_comments | xml_parser::trim_whitespace );
        else if ( iends_with( fileName, ".ini" ) )
            read_ini( fileName, tmp );
        else if ( iends_with( fileName, ".info" ) )
            read_info( fileName, tmp );
        else if ( iends_with( fileName, ".json" ) )
            read_json( fileName, tmp );
        else // per defaul provo con l'xml
            read_xml( fileName, tmp, xml_parser::no_comments | xml_parser::trim_whitespace );

        // Controllo se il nuovo file ha delle chiavi di primo livello già contenute
        // nei file già elaborati. In questo caso tiro un'eccezione, perché boost
        // non sarebbe in grado di fare correttamente il merge
        if ( HasIntersections( tmp ) )
            throw std::range_error( fileName + " file contains root symbols already present" );

        // Infine, faccio il merge del nuovo file con quelli già elaborati
        tree.insert( tree.begin(), tmp.begin(), tmp.end() );
    }
    catch ( const xml_parser_error& e )
    {
        throw std::range_error( e.what() );
    }
}

inline bool Configuration::HasIntersections( const boost::property_tree::ptree& toCheck ) const
{
    using namespace boost::property_tree;

    // itera sulle chiavi di primo livello di toCheck e controlla che non
    // siano già presenti nei file già processati.
    for ( ptree::const_iterator i = toCheck.begin(); i != toCheck.end(); ++i )
    {
        ptree::const_assoc_iterator j = tree.find( i -> first );
        if ( j != tree.not_found() )  // se è stato trovato, decreto che esistono intersezioni
            return true;
    }
    return false;
}

inline Configuration Configuration::GetChild( const std::string& path ) const throw ( std::range_error )
{
    try
    {
        return Configuration( tree.get_child( path ) );
    }
    catch ( const boost::property_tree::ptree_error& e )
    {
        throw std::range_error( e.what( ) );
    }
}


inline void Configuration::Iterate( Visitor visitorFunctor ) const
{
    using namespace boost::property_tree;

    for ( ptree::const_iterator i = tree.begin(); i != tree.end(); ++i )
    {
        const std::string key = i -> first;
        const Configuration c( i -> second );
        visitorFunctor( key, c );
    }
}


} // namespace echidna

#endif // ECHIDNA_CONFIGURATION_H_
