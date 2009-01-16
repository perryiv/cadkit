
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The group class.
//
///////////////////////////////////////////////////////////////////////////////

#include "SceneGraph/OpenSceneGraph/Group.h"
#include "SceneGraph/OpenSceneGraph/Convert.h"
#include "SceneGraph/OpenSceneGraph/Factory.h"

#include "OsgTools/Group.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Strings/Format.h"
#include "Usul/Trace/Trace.h"

#include <algorithm>

using namespace SceneGraph::OSG;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Group, Group::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Group::Group ( GroupPtr g ) : BaseClass ( g ),
  _group ( g )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Group::~Group()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Group::_destroy ), "2352876050" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Group::_destroy()
{
  USUL_TRACE_SCOPE;
  _group = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Group::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::SceneGraph::IGroup::IID:
    return static_cast < Usul::Interfaces::SceneGraph::IGroup* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Insert the child.
//
///////////////////////////////////////////////////////////////////////////////

void Group::insertChild ( unsigned int i, IUnknown::RefPtr unknown ) 
{
  USUL_TRACE_SCOPE;

  // Get the real node.
  Node::NodePtr node ( Usul::Convert::Type<IUnknown,osg::Node>::convert ( unknown ) );
  if ( false == node.valid() )
    return;

  // Local scope.
  {
    WriteLock lock ( this );

    // Check state.
    if ( false == _group.valid() )
      return;

    // Adjust the index if needed.
    const unsigned int numChildren ( _group->getNumChildren() );
    if ( i > numChildren )
    {
      i = numChildren;
    }

    // Insert the child.
    _group->insertChild ( i, node.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the child.
//
///////////////////////////////////////////////////////////////////////////////

void Group::appendChild ( IUnknown::RefPtr child ) 
{
  USUL_TRACE_SCOPE;
  this->insertChild ( this->getNumChildren(), child );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Prepend the child.
//
///////////////////////////////////////////////////////////////////////////////

void Group::prependChild ( IUnknown::RefPtr child ) 
{
  USUL_TRACE_SCOPE;
  this->insertChild ( 0, child );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the child.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Group::findChild ( IUnknown::RefPtr child ) const
{
  USUL_TRACE_SCOPE;
  ReadLock lock ( this );

  // If there's no group then returning zero is like returning 
  // the index that is one past the end.
  if ( false == _group.valid() )
    return 0;

  // If the given child is not a node then it is not a child.
  NodePtr node ( Usul::Convert::Type<IUnknown,osg::Node>::convert ( child ) );
  if ( false == node.valid() )
    return _group->getNumChildren();

  // Return the index.
  const unsigned int index ( _group->getChildIndex ( node.get() ) );
  return index;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the child.
//
///////////////////////////////////////////////////////////////////////////////

void Group::removeChild ( unsigned int i ) 
{
  USUL_TRACE_SCOPE;
  WriteLock lock ( this );

  // Handle no group.
  if ( false == _group.valid() )
    return;

  // Check range.
  if ( i >= _group->getNumChildren() )
    return;

  // Erase the child.
  _group->removeChild ( i );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all the children.
//
///////////////////////////////////////////////////////////////////////////////

void Group::removeAllChildren()
{
  USUL_TRACE_SCOPE;
  WriteLock lock ( this );

  // Handle no group.
  if ( false == _group.valid() )
    return;

  // Remove all the children.
  OsgTools::Group::removeAllChildren ( _group.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of children.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Group::getNumChildren() const
{
  USUL_TRACE_SCOPE;
  ReadLock lock ( this );
  return ( ( true == _group.valid() ) ? _group->getNumChildren() : 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the child.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown::RefPtr Group::getChild ( unsigned int i ) const
{
  USUL_TRACE_SCOPE;
  ReadLock lock ( this );

  // Handle no group.
  if ( false == _group.valid() )
    return IUnknown::RefPtr ( 0x0 );

  // Check range.
  if ( i >= _group->getNumChildren() )
    return IUnknown::RefPtr ( 0x0 );

  // Return the child.
  return Singleton::Factory::instance().createObject ( NodePtr ( _group->getChild ( i ) ) );
}
