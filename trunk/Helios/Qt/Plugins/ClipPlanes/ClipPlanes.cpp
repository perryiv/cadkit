
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Plugins/ClipPlanes/ClipPlanes.h"

#include "Usul/Interfaces/IClippingPlanes.h"
#include "Usul/Interfaces/IGetBoundingBox.h"
#include "Usul/Interfaces/IRedraw.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Strings/Format.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Threads/Named.h"

#include "osg/BoundingBox"
#include "osg/Plane"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ClipPlanes::ClipPlanes( QWidget *parent ) : BaseClass ( parent ),
_caller ()
{
  this->setupUi ( this );

  // Set the inital state
  _advancedOptions->setVisible ( Qt::Checked == _advancedButton->checkState() );

  connect ( _addButton, SIGNAL ( clicked() ), this, SLOT ( _addClipPlaneClicked() ) );
  connect ( _removeButton, SIGNAL ( clicked() ), this, SLOT ( _removeClipPlaneClicked() ) );

  // Toggle the advanced options.
  connect ( _advancedButton, SIGNAL ( toggled ( bool ) ), _advancedOptions, SLOT ( setVisible ( bool ) ) );

  // Selected item changed.
  connect ( _clipPlaneList, SIGNAL ( currentRowChanged( int ) ), this, SLOT ( _selectedItemChanged( int ) ) );

  // Connect the slider.
  connect ( _distanceSlider, SIGNAL ( sliderMoved( int ) ), this, SLOT ( _distanceSliderChanged( int ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ClipPlanes::~ClipPlanes ()
{
  _caller = static_cast < Usul::Interfaces::IUnknown* > ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a clipping plane.
//
///////////////////////////////////////////////////////////////////////////////

void ClipPlanes::_addClipPlaneClicked()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  Usul::Interfaces::IClippingPlanes::QueryPtr cp ( _caller );

  if ( cp.valid () )
  {
    cp->addClippingPlane();
    unsigned int num ( cp->numClippingPlanes() );
    _clipPlaneList->addItem ( Usul::Strings::format ( "Clip Plane ", num - 1 ).c_str() );

    this->_render();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a clipping plane.
//
///////////////////////////////////////////////////////////////////////////////

void ClipPlanes::_removeClipPlaneClicked()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  Usul::Interfaces::IClippingPlanes::QueryPtr cp ( _caller );

  if ( cp.valid () )
  {
    unsigned int row ( _clipPlaneList->currentRow() );
   
    cp->removeClippingPlane ( row );

    QListWidgetItem *item ( _clipPlaneList->takeItem( row ) );
    delete item;

    this->_render();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear.
//
///////////////////////////////////////////////////////////////////////////////

void ClipPlanes::clear()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  _clipPlaneList->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build.
//
///////////////////////////////////////////////////////////////////////////////

void ClipPlanes::build( Usul::Interfaces::IUnknown* caller )
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  this->clear( );

  _caller = caller;

  Usul::Interfaces::IClippingPlanes::QueryPtr cp ( caller );

  if ( cp.valid () )
  {
    this->setEnabled ( true );
    unsigned int num ( cp->numClippingPlanes() );
    for ( unsigned int i = 0; i < num; ++i )
      _clipPlaneList->addItem ( Usul::Strings::format ( "Clip Plane ", i ).c_str() );
  }
  else
    this->setEnabled ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Positive X axis clicked.
//
///////////////////////////////////////////////////////////////////////////////

void ClipPlanes::on_posXAxis_clicked()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  Usul::Interfaces::IGetBoundingBox::QueryPtr getBB ( _caller );

  if ( false == getBB.valid() )
    return;

  osg::BoundingBox bb ( getBB->getBoundingBox() );

  osg::Vec3 p0 ( bb.xMax(), bb.yMin(), bb.zMax() );
  osg::Vec3 p1 ( bb.xMax(), bb.yMax(), bb.zMax() );
  osg::Vec3 p2 ( bb.xMax(), bb.yMin(), bb.zMin() );
  
  osg::Plane plane ( p0, p1, p2 );
  this->_addClippingPlane ( plane );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Positive Y axis clicked.
//
///////////////////////////////////////////////////////////////////////////////

void ClipPlanes::on_posYAxis_clicked()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  Usul::Interfaces::IGetBoundingBox::QueryPtr getBB ( _caller );

  if ( false == getBB.valid() )
    return;

  osg::BoundingBox bb ( getBB->getBoundingBox() );

  osg::Vec3 p0 ( bb.xMin(), bb.yMax(), bb.zMax() );
  osg::Vec3 p1 ( bb.xMin(), bb.yMax(), bb.zMin() );
  osg::Vec3 p2 ( bb.xMax(), bb.yMax(), bb.zMin() );
  
  osg::Plane plane ( p0, p1, p2 );
  this->_addClippingPlane ( plane );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Positive Z axis clicked.
//
///////////////////////////////////////////////////////////////////////////////

void ClipPlanes::on_posZAxis_clicked()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  Usul::Interfaces::IGetBoundingBox::QueryPtr getBB ( _caller );

  if ( false == getBB.valid() )
    return;

  osg::BoundingBox bb ( getBB->getBoundingBox() );

  osg::Vec3 p0 ( bb.xMin(), bb.yMin(), bb.zMax() );
  osg::Vec3 p1 ( bb.xMin(), bb.yMax(), bb.zMax() );
  osg::Vec3 p2 ( bb.xMax(), bb.yMax(), bb.zMax() );
  
  osg::Plane plane ( p0, p1, p2 );
  this->_addClippingPlane ( plane );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Negative X axis clicked.
//
///////////////////////////////////////////////////////////////////////////////

void ClipPlanes::on_negXAxis_clicked()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  Usul::Interfaces::IGetBoundingBox::QueryPtr getBB ( _caller );

  if ( false == getBB.valid() )
    return;

  osg::BoundingBox bb ( getBB->getBoundingBox() );

  osg::Vec3 p0 ( bb.xMin(), bb.yMin(), bb.zMax() );
  osg::Vec3 p1 ( bb.xMin(), bb.yMin(), bb.zMin() );
  osg::Vec3 p2 ( bb.xMin(), bb.yMax(), bb.zMax() );
  
  osg::Plane plane ( p0, p1, p2 );
  this->_addClippingPlane ( plane );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Negative Y axis clicked.
//
///////////////////////////////////////////////////////////////////////////////

void ClipPlanes::on_negYAxis_clicked()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  Usul::Interfaces::IGetBoundingBox::QueryPtr getBB ( _caller );

  if ( false == getBB.valid() )
    return;

  osg::BoundingBox bb ( getBB->getBoundingBox() );

  osg::Vec3 p0 ( bb.xMin(), bb.yMin(), bb.zMax() );
  osg::Vec3 p1 ( bb.xMax(), bb.yMin(), bb.zMin() );
  osg::Vec3 p2 ( bb.xMin(), bb.yMin(), bb.zMin() );
  
  osg::Plane plane ( p0, p1, p2 );
  this->_addClippingPlane ( plane );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Negative Z axis clicked.
//
///////////////////////////////////////////////////////////////////////////////

void ClipPlanes::on_negZAxis_clicked()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  Usul::Interfaces::IGetBoundingBox::QueryPtr getBB ( _caller );

  if ( false == getBB.valid() )
    return;

  osg::BoundingBox bb ( getBB->getBoundingBox() );

  osg::Vec3 p0 ( bb.xMin(), bb.yMin(), bb.zMin() );
  osg::Vec3 p1 ( bb.xMax(), bb.yMax(), bb.zMin() );
  osg::Vec3 p2 ( bb.xMin(), bb.yMax(), bb.zMin() );
  
  osg::Plane plane ( p0, p1, p2 );
  this->_addClippingPlane ( plane );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we in advanced mode?
//
///////////////////////////////////////////////////////////////////////////////

bool ClipPlanes::_isAdvanced() const
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return false );

  return Qt::Checked == _advancedButton->checkState();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Render.
//
///////////////////////////////////////////////////////////////////////////////

void ClipPlanes::_render()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  Usul::Interfaces::IRedraw::QueryPtr draw ( _caller );

  // Render.
  if ( draw.valid() )
    draw->redraw();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a clipping plane.
//
///////////////////////////////////////////////////////////////////////////////

void ClipPlanes::_addClippingPlane( const osg::Plane& plane )
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  Usul::Interfaces::IClippingPlanes::QueryPtr cp ( _caller );

  if ( false == cp.valid () )
    return;

  if ( false == this->_isAdvanced() )
  {
    cp->removeClippingPlanes();
    _clipPlaneList->clear();
  }

  cp->addClippingPlane( plane );

  unsigned int num ( cp->numClippingPlanes() );
  _clipPlaneList->addItem ( Usul::Strings::format ( "Clip Plane ", num - 1 ).c_str() );
  _clipPlaneList->setCurrentRow ( _clipPlaneList->count() - 1 );

  this->_render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Distance has changed.
//
///////////////////////////////////////////////////////////////////////////////

void ClipPlanes::_distanceSliderChanged ( int value )
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  Usul::Interfaces::IClippingPlanes::QueryPtr cp ( _caller );

  if ( false == cp.valid () )
    return;

  int index ( _clipPlaneList->currentRow() );

  if ( index < 0 )
    return;

  osg::Plane plane ( cp->getClippingPlane( index ) );
  plane[3] = value;
  cp->setClippingPlane ( index, plane );

  this->_render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Selected item has changed.
//
///////////////////////////////////////////////////////////////////////////////

void ClipPlanes::_selectedItemChanged( int index )
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  Usul::Interfaces::IClippingPlanes::QueryPtr cp ( _caller );
  Usul::Interfaces::IGetBoundingBox::QueryPtr getBB ( _caller );

  if ( false == cp.valid () || false == getBB.valid() || index < 0 )
    return;

  // Get the bounding box.
  osg::BoundingBox bb ( getBB->getBoundingBox() );

  // Get the plane.
  osg::Plane plane ( cp->getClippingPlane( index ) );
  const float d ( plane[3] );

  // Move the plane to origin.
  plane[3] = 0.0;

  const float t0 ( plane.distance ( bb._min ) );
  const float t1 ( plane.distance ( bb._max ) );

  const float min ( Usul::Math::minimum ( t0, t1 ) );
  const float max ( Usul::Math::maximum ( t0, t1 ) );
  const float distance ( max - min );
  
  _distanceSlider->setMinimum ( static_cast < int > ( ::floor ( -distance ) ) );
  _distanceSlider->setMaximum ( static_cast < int > ( ::ceil  ( distance ) ) );
  _distanceSlider->setValue ( static_cast < int > ( d ) );

  osg::Vec3 normal ( plane.getNormal() );
  _normalX->setValue ( normal[0] );
  _normalY->setValue ( normal[1] );
  _normalZ->setValue ( normal[2] );
}
