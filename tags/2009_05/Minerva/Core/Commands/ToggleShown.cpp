
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Commands/ToggleShown.h"
#include "Minerva/Interfaces/IDirtyScene.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Interfaces/ITreeNode.h"

using namespace Minerva::Core::Commands;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ToggleShown, ToggleShown::BaseClass );
USUL_IMPLEMENT_COMMAND ( ToggleShown );
USUL_FACTORY_REGISTER_CREATOR ( ToggleShown );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ToggleShown::ToggleShown ( ) : 
  BaseClass( 0x0 )
{
  this->_addMember ( "layer", _layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ToggleShown::ToggleShown ( Usul::Interfaces::IUnknown* layer, const std::string& text ) : 
  BaseClass( 0x0 ),
  _layer ( layer )
{
  this->_addMember ( "layer", _layer );

  Usul::Interfaces::ITreeNode::QueryPtr tn ( layer );
  this->text ( tn.valid() && text.empty() ? tn->getTreeNodeName() : text );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ToggleShown::~ToggleShown()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute command.
//
///////////////////////////////////////////////////////////////////////////////

void ToggleShown::_execute()
{
  Minerva::Interfaces::IDirtyScene::QueryPtr dirty ( Usul::Documents::Manager::instance().activeDocument() );
  
  // Toggle shown.
  Usul::Interfaces::IBooleanState::QueryPtr state ( _layer );
  if ( state.valid() )
  {
    state->setBooleanState ( !state->getBooleanState() );

    // Dirty the scene.
    if ( dirty.valid() )
      dirty->dirtyScene ( true, Usul::Interfaces::IUnknown::QueryPtr ( _layer ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for an interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* ToggleShown::queryInterface( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::ISerialize::IID:
    return static_cast < Usul::Interfaces::ISerialize* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the item checked?
//
///////////////////////////////////////////////////////////////////////////////

bool ToggleShown::updateCheck() const
{
  Usul::Interfaces::IBooleanState::QueryPtr state ( const_cast<Usul::Interfaces::IUnknown*> ( _layer.get() ) );
  return state.valid() ? state->getBooleanState() : false;
}
