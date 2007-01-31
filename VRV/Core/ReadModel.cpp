
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  ReadModel class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "ReadModel.h"
#include "Components.h"

#include "VRV/Interfaces/IModelAdd.h"

#include "SAL/Interfaces/INode.h"

#include "Usul/Interfaces/IRead.h"
#include "Usul/Components/Object.h"

using namespace VRV;
using namespace VRV::Threads;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ReadModel::ReadModel ( const std::string &filename, const Matrix &matrix, Unknown *caller ) : 
  BaseClass(),
  _filename ( filename ),
  _matrix ( matrix ),
  _caller ( caller )
{
  // Make sure the caller supports the interface we need.
  VRV::Interfaces::IModelAdd::ValidQueryPtr add ( _caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ReadModel::~ReadModel()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  The function called from the new thread.
//
///////////////////////////////////////////////////////////////////////////////

void ReadModel::_run()
{
  // Create a component for reading the model.
  Usul::Interfaces::IRead::ValidQueryPtr reader 
    ( Usul::Components::Object::create 
      ( Usul::Interfaces::IRead::IID, VRV::SCENE_ABSTRACTION_LAYER, true, true ) );

  // Read the model.
  SAL::Interfaces::INode::ValidQueryPtr node ( reader->read ( _filename, 0x0 ) );

  // Get the interface we need.
  VRV::Interfaces::IModelAdd::ValidQueryPtr add ( _caller );

  // Add the model.
  add->addModel ( node, _matrix, _filename );
}
