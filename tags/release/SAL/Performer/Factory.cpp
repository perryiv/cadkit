
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Factory class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "Factory.h"
#include "Transform.h"
#include "Grid.h"
#include "FileIO.h"

#include "SAL/Common/Create.h"

#include "Usul/Components/Create.h"

#include "Performer/pf/pfDCS.h"

using namespace SAL;
using namespace SAL::Performer;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Factory, Factory::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this library was compiled in debug mode.
//
///////////////////////////////////////////////////////////////////////////////

extern "C" bool usul_is_debug_build()
{
  #ifdef _DEBUG
    return true;
  #else
    return false;
  #endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a class factory.
//
///////////////////////////////////////////////////////////////////////////////

extern "C" Usul::Interfaces::IClassFactory *usul_get_class_factory()
{
  Usul::Interfaces::IClassFactory::ValidRefPtr factory ( new Factory() );
  return factory.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Factory::Factory() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Factory::~Factory()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Factory::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IClassFactory::IID:
    return static_cast<Usul::Interfaces::IClassFactory*>(this);
  case Usul::Interfaces::IUnknown::IID:
    return static_cast<Usul::Interfaces::IUnknown*>(this);
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a component of the given interface type (defined by iid).
//  Return an unknown pointer to the new component.
//  Note: we do not allow creation of a Node (INode).
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Factory::createInstance ( unsigned long iid )
{
  switch ( iid )
  {
  case SAL::Interfaces::IGroup::IID:
    return Usul::Components::Create<Group>::create ( iid, new ::pfGroup() );
  case SAL::Interfaces::ITransformFloat::IID:
  case SAL::Interfaces::ITransformDouble::IID:
    return Usul::Components::Create<Transform>::create ( iid, new ::pfDCS() );
  case Usul::Interfaces::IRead::IID:
  case Usul::Interfaces::IWrite::IID:
    return Usul::Components::Create<FileIO>::create ( iid );
  case SAL::Interfaces::IGridFloat::IID:
    return Usul::Components::Create<Grid>::create ( iid );
  default:
    return 0x0; // No component.
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the proper component from the given pointer.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Factory::create ( ::pfNode *n )
{
  typedef SAL::Interfaces::INode INode;
  typedef SAL::Detail::NoRttiCast NoRttiCast;
  typedef SAL::Detail::NodeWrapper < ::pfNode, ::pfDCS,   Transform, INode, NoRttiCast > DCSWrapper;
  typedef SAL::Detail::NodeWrapper < ::pfNode, ::pfGroup, Group,     INode, NoRttiCast > GroupWrapper;
  typedef SAL::Detail::NodeWrapper < ::pfNode, ::pfNode,  Node,      INode, NoRttiCast > NodeWrapper;

  // Try the various types...
  INode *in = 0x0;
  in = DCSWrapper::create ( n );
  if ( in )
    return in;
  in = GroupWrapper::create ( n );
  if ( in )
    return in;
  in = NodeWrapper::create ( n );
  if ( in )
    return in;

  // If we get to here then just return null.
  return 0x0;
}
