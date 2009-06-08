
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Group of renderers.
//
///////////////////////////////////////////////////////////////////////////////

#include "Display/Render/Group.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include "boost/mem_fn.hpp"

using namespace Display::Render;

USUL_IMPLEMENT_TYPE_ID ( Group );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Group, Group::BaseClass );


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

Group::Group() : BaseClass(),
  _renderers()
{
  USUL_TRACE_SCOPE;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

Group::~Group()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Group::_destroy ), "3781756854" );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destroy this instance.
//
/////////////////////////////////////////////////////////////////////////////

void Group::_destroy()
{
  USUL_TRACE_SCOPE;
  _renderers.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown * Group::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;

  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::SceneGraph::IPostRender::IID:
    return static_cast < Usul::Interfaces::SceneGraph::IPostRender* > ( this );
  case Usul::Interfaces::SceneGraph::IPreRender::IID:
    return static_cast < Usul::Interfaces::SceneGraph::IPreRender* > ( this );
  case Usul::Interfaces::SceneGraph::IRender::IID:
    return static_cast < Usul::Interfaces::SceneGraph::IRender* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to call the interface.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class Interface > struct ForEach
  {
    template < class Function, class Sequence, class Arg1, class Arg2 >
    static void call ( Sequence &s, Function f, Arg1 arg1, Arg2 arg2 )
    {
      USUL_TRACE_SCOPE_STATIC;

      typedef typename Sequence::iterator Itr;
      typedef typename Interface::QueryPtr InterfacePtr;

      for ( Itr i = s.begin(); i != s.end(); ++i )
      {
        InterfacePtr ptr ( *i );
        if ( true == ptr.valid() )
        {
          f ( ptr.get(), arg1, arg2 );
        }
      }
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-render the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Group::preRender ( IUnknown::RefPtr projection, IUnknown::RefPtr scene )
{
  USUL_TRACE_SCOPE;
  Renderers renderers ( Usul::Threads::Safe::get ( this->mutex(), _renderers ) );
  Helper::ForEach<IPreRender>::call ( renderers, boost::mem_fn ( &IPreRender::preRender ), projection, scene );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Post-render the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Group::postRender ( IUnknown::RefPtr projection, IUnknown::RefPtr scene )
{
  USUL_TRACE_SCOPE;
  Renderers renderers ( Usul::Threads::Safe::get ( this->mutex(), _renderers ) );
  Helper::ForEach<IPostRender>::call ( renderers, boost::mem_fn ( &IPostRender::postRender ), projection, scene );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Render the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Group::render ( IUnknown::RefPtr projection, IUnknown::RefPtr scene )
{
  USUL_TRACE_SCOPE;
  Renderers renderers ( Usul::Threads::Safe::get ( this->mutex(), _renderers ) );
  Helper::ForEach<IRender>::call ( renderers, boost::mem_fn ( &IRender::render ), projection, scene );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the new renderers and return the old ones.
//
///////////////////////////////////////////////////////////////////////////////

Group::Renderers Group::renderers ( const Renderers &r )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  Renderers old ( _renderers );
  _renderers = r;
  return old;
}
