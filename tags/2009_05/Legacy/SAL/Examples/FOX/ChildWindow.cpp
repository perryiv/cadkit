
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  MDI Child window.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "ChildWindow.h"
#include "MainWindow.h"

#include "SAL/FOX/View.h"

#include "SAL/Core/Factory.h"
#include "SAL/Core/Shape.h"
#include "SAL/Core/Material.h"
#include "SAL/Core/PolygonMode.h"

using namespace GSG;

FXIMPLEMENT ( ChildWindow, ChildWindow::BaseClass, 0x0, 0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ChildWindow::ChildWindow() : BaseClass()
{
  // Don't use this constructor.
  ErrorChecker ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ChildWindow::ChildWindow ( const FXString &name, MainWindow *parent ) : 
  BaseClass ( parent->clientArea(), name, NULL, parent->mdiMenu(), 0, 20, 20, 400, 300 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ChildWindow::~ChildWindow()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create.
//
///////////////////////////////////////////////////////////////////////////////

void ChildWindow::create()
{
  // Call the base class's function.
  BaseClass::create();

  // Should be true.
  ErrorChecker ( 0x0 != this->root() );
  ErrorChecker ( 0x0 != this->view() );

  // Make the attributes.
  AttributeSet::ValidPtr attributes ( new AttributeSet );

  // Set a polygon-mode.
  attributes->set ( new PolygonMode ( PolygonMode::LINE ) );

  // Set the material.
  Color color ( 1, 1, 0, 1 );
  Material::ValidPtr material ( new Material );
  material->side ( Material::FRONT );
  material->diffuse ( color );
  material->ambient ( color );
  attributes->set ( material );

  // Make a factory and tell it to use these attributes.
  Factory::ValidPtr factory ( new Factory );
  factory->attributes ( attributes );

  // Make a sphere.
  Vec3 center ( 0, 0, -200 );
  Real radius ( 1 );
  UnsignedInteger numSubDivisions ( 4 );
  Shape::ValidPtr sphere ( factory->sphere ( center, radius, numSubDivisions ) );

  // Add the sphere to the scene.
  this->root()->append ( sphere );

  // View everything.
  this->view()->viewAll();
}
