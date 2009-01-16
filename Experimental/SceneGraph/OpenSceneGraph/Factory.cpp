
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The factory class.
//
///////////////////////////////////////////////////////////////////////////////

#include "SceneGraph/OpenSceneGraph/Factory.h"
#include "SceneGraph/OpenSceneGraph/Group.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Factory/TypeCreator.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Trace/Trace.h"

#include "osg/Geode"
#include "osg/MatrixTransform"
#include "osg/ref_ptr"

using namespace SceneGraph::OSG;


///////////////////////////////////////////////////////////////////////////////
//
//  Instantiate the singleton.
//
///////////////////////////////////////////////////////////////////////////////

namespace SceneGraph
{
  namespace OSG
  {
    namespace Singleton
    {
      USUL_INSTANTIATE_SINGLETON ( Factory );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper struct for registering node-makers.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class NodeType, class OsgType > struct RegisterMaker
  {
    typedef typename NodeType::RefPtr NodePtr;
    typedef osg::ref_ptr<OsgType> OsgPtr;
    typedef BaseNodeMaker::RefPtr MakerPtr;
    typedef DerivedNodeMaker<NodeType,OsgType> MakerType;

    template < class Makers > static void add ( Makers &makers )
    {
      typedef typename Makers::value_type ValueType;
      OsgPtr node ( new OsgType );
      const std::string type ( node->className() );
      if ( false == type.empty() )
      {
        MakerPtr maker ( new MakerType );
        makers.insert ( ValueType ( type, maker ) );
      }
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Factory::Factory() : BaseClass(),
  _nodeMakers()
{
  USUL_TRACE_SCOPE;
  Helper::RegisterMaker<Group,osg::Group>::add ( _nodeMakers );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Factory::~Factory()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Factory::_destroy ), "1683634390" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Factory::_destroy()
{
  USUL_TRACE_SCOPE;
  _nodeMakers.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return new object.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown::RefPtr Factory::createObject ( unsigned long iid )
{
  USUL_TRACE_SCOPE;
  typedef Usul::Interfaces::SceneGraph::IChild IChild;

  switch ( iid )
  {
    case Usul::Interfaces::SceneGraph::IGroup::IID:
    {
      Group::GroupPtr osg ( new osg::Group );
      Group::RefPtr group ( new Group ( osg ) );
      IChild::QueryPtr child ( group );
      return IUnknown::RefPtr ( child );
    }
    default:
    {
      return IUnknown::RefPtr ( 0x0 );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return new object.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown::RefPtr Factory::createObject ( NodePtr ptr )
{
  USUL_TRACE_SCOPE;
  typedef Usul::Interfaces::SceneGraph::IChild IChild;

  if ( false == ptr.valid() )
    return IUnknown::RefPtr ( 0x0 );

  const std::string type ( ptr->className() );
  if ( true == type.empty() )
    return IUnknown::RefPtr ( 0x0 );

  BaseNodeMaker::RefPtr maker ( 0x0 );
  {
    ReadLock lock ( this );
    NodeMakers::const_iterator i ( _nodeMakers.find ( type ) );
    maker = ( ( _nodeMakers.end() == i ) ? BaseNodeMaker::RefPtr ( 0x0 ) : i->second );
  }

  Node::RefPtr node ( ( true == maker.valid() ) ? ( (*maker) ( ptr ) ) : 0x0 );
  IChild::QueryPtr child ( node );
  return IUnknown::RefPtr ( child );
}
