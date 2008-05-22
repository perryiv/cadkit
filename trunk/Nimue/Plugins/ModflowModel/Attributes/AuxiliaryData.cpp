
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Auxiliary data attribute.
//
///////////////////////////////////////////////////////////////////////////////

#include "ModflowModel/Attributes/AuxiliaryData.h"

#include "OsgTools/Group.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Trace/Trace.h"

#include "osg/Node"

using namespace Modflow::Attributes;

USUL_IMPLEMENT_TYPE_ID ( AuxiliaryData );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

AuxiliaryData::AuxiliaryData ( const std::string &name, IUnknown *parent, osg::Group *scene, const RegistryPath &regPath ) : BaseClass ( name, parent, regPath ),
  _root ( new osg::Group )
{
  USUL_TRACE_SCOPE;

  // Add scene if it's valid.
  if ( 0x0 != scene )
    _root->addChild ( scene );

  // Set the group as the scene.
  this->_setScene ( _root.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

AuxiliaryData::~AuxiliaryData()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &AuxiliaryData::_destroy ), "1646816910" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy
//
///////////////////////////////////////////////////////////////////////////////

void AuxiliaryData::_destroy()
{
  USUL_TRACE_SCOPE;
  OsgTools::Group::removeAllChildren ( _root.get() );
  _root = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state.
//
///////////////////////////////////////////////////////////////////////////////

void AuxiliaryData::setBooleanState ( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  this->visible ( state );
  _root->setNodeMask ( ( true == state ) ? 0xFFFFFFFF : 0 );
  BaseClass::requestActiveDocumentRedraw();
}
