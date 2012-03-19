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

#ifndef ECHIDNA_EVENTBUS_H_
#define ECHIDNA_EVENTBUS_H_

#include <string>
#include <map>
#include <queue>
#include <typeinfo>
#include <boost/function.hpp>
#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/utility.hpp>

namespace echidna
{



class HandlerFunctionBase
{
public:
    virtual ~HandlerFunctionBase() {}
    virtual void Exec( const boost::any& msg ) = 0;
};


template < typename E >
class HandlerFunction : public HandlerFunctionBase
{
private:
    static bool True( E /*e*/ ) { return true; }
public:
    typedef boost::function<void (E x)> F;
    typedef boost::function<bool (E x)> P;
    HandlerFunction( F _f ) : f( _f ), p( True ) {}
    HandlerFunction( F _f, P _p ) : f( _f ), p( _p ) {}
    virtual void Exec( const boost::any& msg )
    {
        try
        {
            E e = boost::any_cast< E > ( msg );
            if ( p( e ) )
                f( e );
        }
        catch ( const boost::bad_any_cast& )
        {
            assert( false );
        }
    }
private:
    F f;
    P p;
};


class EventBus : private boost::noncopyable
{
private:

    struct Type_info_cmp
    {
        bool operator()(const std::type_info *t1, const std::type_info *t2) const
        {
            return t1->before(*t2) != 0;
        }
    };

    typedef boost::shared_ptr< HandlerFunctionBase > HandlerPtr;

    typedef std::multimap<
        const std::type_info*,
        HandlerPtr,
        Type_info_cmp
    > Handlers;

    typedef Handlers::iterator It;

    typedef std::pair< boost::any, const std::type_info* > Entry;

    Handlers handlers;
    std::queue< Entry > messages;
    boost::mutex mtx;
    boost::mutex handlersMtx;
    boost::condition_variable cond;
    bool running;

    void Dispatch( Entry e )
    {
        boost::any m = e.first;
        const std::type_info* id = e.second;
        typedef std::pair< It, It > Res;

        std::vector< HandlerPtr > tmp;

        {
            boost::lock_guard< boost::mutex > lock( handlersMtx );
            Res res = handlers.equal_range( id );
            // si copia gli handler in una lista temporanea (per rilasciare il mtx)
            for ( It i = res.first; i != res.second; ++i )
                tmp.push_back( i -> second );
        }

        // ora può invocare gli handlers (perché ha rilasciato il mtx):
        std::for_each( tmp.begin(), tmp.end(), boost::bind( &HandlerFunctionBase::Exec, _1, m ) );
    }

    void Unsubscribe( It i )
    {
        boost::lock_guard< boost::mutex > lock( handlersMtx );
        handlers.erase( i );
    }

    friend class Subscription;

public:

    class Subscription
    {
    public:
        Subscription() : mb( NULL ) {}
        void Disconnect() { if ( mb ) mb -> Unsubscribe( i ); }
    private:
        Subscription( EventBus* _mb, Handlers::iterator _i ) : mb( _mb ), i( _i ) {}
        EventBus* mb;
        Handlers::iterator i;
        friend class EventBus;
    };

    ~EventBus()
    {
        Stop();
    }

    template < typename E >
    Subscription Subscribe( boost::function<void (E x)> handler )
    {
        boost::lock_guard< boost::mutex > lock( handlersMtx );

        It i =
            handlers.insert(
                std::make_pair(
                    &typeid( E ),
                    HandlerPtr( new HandlerFunction< E >( handler ) )
                )
            );
        return Subscription( this, i );
    }

    template < typename E >
    Subscription Subscribe( boost::function<void (E x)> handler, boost::function<bool (E x)> predicate )
    {
        boost::lock_guard< boost::mutex > lock( handlersMtx );

        It i =
            handlers.insert(
                std::make_pair(
                    &typeid( E ),
                    HandlerPtr( new HandlerFunction< E >( handler, predicate ) )
                )
            );
        return Subscription( this, i );
    }

    template < typename E >
    void Post( E e )
    {
        const std::type_info* id = &typeid( E );
        {
            boost::lock_guard< boost::mutex > lock( mtx );
            messages.push( std::make_pair( e, id ) );
        }
        cond.notify_one();
    }

    bool PollOne()
    {
        Entry e;

        {
            boost::lock_guard< boost::mutex > lock( mtx );
            if ( ! messages.empty() )
            {
                e = messages.front();
                messages.pop();
            }
        }

        if ( e.first.empty() )
            return false;
        else
        {
            Dispatch( e );
            return true;
        }
    }

    void Poll()
    {
        bool result = true;
        while ( result )
            result = PollOne();
    }

    void RunOne()
    {
        Entry e;

        {
            boost::unique_lock< boost::mutex > lock( mtx );
            running = true; // TODO: qui oppure un metodo Reset() apposito?

            while ( messages.empty() && running )
                cond.wait( lock );

            if ( !running ) return; // significa che è stato invocato Stop()

            e = messages.front();
            messages.pop();
        }

        assert ( !e.first.empty() );
        Dispatch( e );
    }

    void Run()
    {
        Entry e;

        {
            boost::unique_lock< boost::mutex > lock( mtx );
            running = true; // TODO: qui oppure un metodo Reset() apposito?

            while ( messages.empty() && running )
                cond.wait( lock );

            if ( !running ) return; // significa che è stato invocato Stop()

            e = messages.front();
            messages.pop();
        }

        assert ( !e.first.empty() );
        Dispatch( e );

        // controllo se ho messo qualcosa:
        Poll();
    }

    void Stop()
    {
        {
            boost::lock_guard< boost::mutex > lock( mtx );
            running = false;
        }
        cond.notify_one();
    }
};


} // namespace echidna

#endif // ECHIDNA_EVENTBUS_H_
