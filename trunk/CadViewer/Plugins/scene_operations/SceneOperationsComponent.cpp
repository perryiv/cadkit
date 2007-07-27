
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "SceneOperationsComponent.h"
#include "UserDataTool.h"
#include "UserDataState.h"
#include "UserDataSelected.h"

#include "CadViewer/ErrorChecker.h"

#include "osg/Group"
#include "osg/Material"

using namespace CV;
using namespace CV::Plugins;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Component , Component::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Component::Component() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Component::~Component()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Component::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Interfaces::IMaterialStack::IID:
    return static_cast<Interfaces::IMaterialStack*>(this);
  case Interfaces::ISelection::IID:
    return static_cast<Interfaces::ISelection*>(this);
  case Interfaces::IVisibility::IID:
    return static_cast<Interfaces::IVisibility*>(this);
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin * > ( this );    
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Push the material.
//
///////////////////////////////////////////////////////////////////////////////

void Component::pushMaterial ( const osg::Material *mat, osg::Node *node )
{
  CV::ErrorChecker ( 1075791422, 0x0 != mat );
  CV::ErrorChecker ( 1075791423, 0x0 != node );

  // Get (perhaps make) a state-set user-data for the node.
  UserDataState::Ptr uds ( Detail::UserDataTool<UserDataState>::get ( node, true ) );
  CV::ErrorChecker ( 1071020889, uds.valid() );

  // Get the state-set (it may be null).
  osg::ref_ptr<osg::StateSet> ss ( node->getStateSet() );

  // Save the node's state (it may be null).
  uds->push ( ss.get() );

  // Now make sure we have a state. Copy the existing one if it's valid.
  ss = ( ss.valid() ) ? new osg::StateSet ( *ss ) : new osg::StateSet;
  CV::ErrorChecker ( 1071020890, ss.valid() );

  // Set the material.
  ss->setAttribute ( new osg::Material ( *mat ), osg::StateAttribute::OVERRIDE );

  // Set the node's state.
  node->setStateSet ( ss.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pop the material.
//
///////////////////////////////////////////////////////////////////////////////

void Component::popMaterial ( osg::Node *node )
{
  CV::ErrorChecker ( 1075791465u, 0x0 != node );

  // See if there is user-data.
  UserDataMap::Ptr map ( dynamic_cast < UserDataMap * > ( node->getUserData() ) );
  if ( map.valid() )
  {
    // See if there is a state-set.
    UserDataMap::iterator i = map->find ( UserDataState::classId() );
    if ( map->end() != i )
    {
      // Get the state-set.
      UserDataState::Ptr stateStack ( dynamic_cast < UserDataState * > ( (*i).second.get() ) );
      CV::ErrorChecker ( 1070874710u, stateStack.valid() );

      // Is there anything on the stack?
      if ( false == stateStack->empty() )
      {
        // Pop the top one off.
        osg::ref_ptr<osg::StateSet> ss ( stateStack->top() );
        stateStack->pop();

        // Set the node's state-set, even if it is null.
        node->setStateSet ( ss.get() );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make sure the object has the user-data that indicates "selected".
//
///////////////////////////////////////////////////////////////////////////////

void Component::select ( osg::Object* object )
{
  CV::ErrorChecker ( 1071020891, 0x0 != object );

  // Get (perhaps make) a selected-user-data for the object.
  UserDataSelected::Ptr uds ( Detail::UserDataTool<UserDataSelected>::get ( object, true ) );
  CV::ErrorChecker ( 1071020888, uds.valid() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make sure the object does not have the user-data that indicates "selected".
//
///////////////////////////////////////////////////////////////////////////////

void Component::unselect ( osg::Object* object )
{
  CV::ErrorChecker ( 1071020891, 0x0 != object );

  // Remove any selected-user-data from the object.
  Detail::UserDataTool<UserDataSelected>::remove ( object );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the object is selected.
//
///////////////////////////////////////////////////////////////////////////////

bool Component::isSelected ( const osg::Object* object ) const
{
  CV::ErrorChecker ( 1071044327, 0x0 != object );
  return Detail::UserDataTool<UserDataSelected>::has ( object );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the node.
//
///////////////////////////////////////////////////////////////////////////////

void Component::show ( osg::Node* node )
{
  CV::ErrorChecker ( 1071571557, 0x0 != node );
  node->setNodeMask ( 0xffffffff );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide the node.
//
///////////////////////////////////////////////////////////////////////////////

void Component::hide ( osg::Node* node )
{
  CV::ErrorChecker ( 1071571558, 0x0 != node );
  node->setNodeMask ( 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the object is visible.
//
///////////////////////////////////////////////////////////////////////////////

bool Component::isVisible ( const osg::Node* node ) const
{
  CV::ErrorChecker ( 1071571559, 0x0 != node );
  return node->getNodeMask() != 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name of the plugin.
//
///////////////////////////////////////////////////////////////////////////////

std::string Component::getPluginName() const
{
  return "Scene Operations";
}
