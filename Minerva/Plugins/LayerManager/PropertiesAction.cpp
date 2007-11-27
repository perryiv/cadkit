
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for the action.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/LayerManager/PropertiesAction.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Components/Manager.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/ILayerModifyGUIQt.h"
#include "Usul/Trace/Trace.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PropertiesAction::PropertiesAction ( ILayer* layer, IUnknown *caller ) : BaseClass ( 0x0 ),
  _layer ( layer ),
  _caller ( caller )
{
  USUL_TRACE_SCOPE;

  this->setText ( tr ( "Properties" ) );
  this->setToolTip ( tr ( "Show the property dialog for this layer" ) );


  this->connect ( this, SIGNAL ( triggered() ), this, SLOT ( _slot() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

PropertiesAction::~PropertiesAction()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &PropertiesAction::_destroy ), "3812105717" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this instance.
//
///////////////////////////////////////////////////////////////////////////////

void PropertiesAction::_destroy()
{
  USUL_TRACE_SCOPE;
  _caller = static_cast < IUnknown * > ( 0x0 );
  _layer = static_cast < ILayer * > ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called by the GUI system.
//
///////////////////////////////////////////////////////////////////////////////

void PropertiesAction::_slot()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &PropertiesAction::_execute ), "2988288717" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void PropertiesAction::_execute()
{
  USUL_TRACE_SCOPE;

  typedef Usul::Components::Manager PluginManager;
  typedef PluginManager::UnknownSet Unknowns;

  Unknowns unknowns ( PluginManager::instance().getInterfaces ( Usul::Interfaces::ILayerModifyGUIQt::IID ) );

  for ( Unknowns::iterator iter = unknowns.begin (); iter != unknowns.end(); ++iter )
  {
    Usul::Interfaces::ILayerModifyGUIQt::QueryPtr gui ( *iter );
    if ( gui->handle ( _layer.get() ) )
    {
      gui->showModifyGUI( _layer.get(), _caller.get() );
      break;
    }
  }
}
