
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

#include "QtTools/ScopedSignals.h"

#include "osg/BoundingBox"
#include "osg/Plane"

#include <algorithm>

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ClipPlanes::ClipPlanes( QWidget *parent ) : BaseClass ( parent ),
_caller ()
{
  this->setupUi ( this );

  // Hide advanced button in release.  Still has kinks to work out.
//#ifdef NDEBUG
//  _advancedButton->setVisible ( false );
//#endif

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
  connect ( _distanceSpinBox, SIGNAL ( valueChanged( double ) ), this, SLOT ( _distanceSpinBoxChanged( double ) ) );
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
    osg::Plane::Vec3_type normal ( _normalX->value(), _normalY->value(), _normalZ->value() );
    normal.normalize();
    osg::Plane plane ( normal, _distanceSpinBox->value() );
    cp->addClippingPlane ( plane );
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
    const int row ( _clipPlaneList->currentRow() );
    if ( ( row >= 0 ) && ( row < _clipPlaneList->count() ) )
    {
      cp->removeClippingPlane ( row );

      QListWidgetItem *item ( _clipPlaneList->takeItem( row ) );
      delete item;

      this->_render();
    }
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

  this->_setDistance( value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Selected item has changed.
//
///////////////////////////////////////////////////////////////////////////////

void ClipPlanes::_selectedItemChanged ( int index )
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  Usul::Interfaces::IClippingPlanes::QueryPtr cp ( _caller );
  Usul::Interfaces::IGetBoundingBox::QueryPtr getBB ( _caller );

  if ( false == cp.valid () || false == getBB.valid() || index < 0 )
    return;

  // Handle out of range value.
  const unsigned int numPlanes ( cp->numClippingPlanes() );
  if ( ( index < 0 ) || ( static_cast < unsigned int > ( index ) >= numPlanes ) )
    return;

  // Get the bounding box.
  osg::BoundingBox bb ( getBB->getBoundingBox() );

  // Get the plane.
  osg::Plane plane ( cp->getClippingPlane ( index ) );
  
  // Get the distance.
  const float d ( plane[3] );
  
  // Get the normal of the plane.
  const osg::Vec3 normal ( plane.getNormal() );

  // Distances from the orgin, plus a small buffer.
  const float t0 ( ( ( normal[0] * bb._min[0] ) + ( normal[1] * bb._min[1] ) + ( normal[2] * bb._min[2] ) ) * -1.01 );
  const float t1 ( ( ( normal[0] * bb._max[0] ) + ( normal[1] * bb._max[1] ) + ( normal[2] * bb._max[2] ) ) * -1.01 );
  
  // Find the min and max distance.
  const float min ( Usul::Math::minimum ( t0, t1 ) );
  const float max ( Usul::Math::maximum ( t0, t1 ) );
  
  // Set the min max values.
  this->_setMinMax ( min, max );
  
  // Set the value.
  _distanceSlider->setValue ( static_cast < int > ( d ) );
  _distanceSpinBox->setValue ( d );

  // Set normal values.
  _normalX->setValue ( normal[0] );
  _normalY->setValue ( normal[1] );
  _normalZ->setValue ( normal[2] );
}



///////////////////////////////////////////////////////////////////////////////
//
//  Set the min and max values.
//
///////////////////////////////////////////////////////////////////////////////

void ClipPlanes::_setMinMax ( double min, double max )
{
  USUL_TRACE_SCOPE;

  // Set the slider min max.
  _distanceSlider->setMinimum ( static_cast < int > ( ::floor ( min ) ) );
  _distanceSlider->setMaximum ( static_cast < int > ( ::ceil  ( max ) ) );
  
  _distanceSpinBox->setMinimum ( static_cast < int > ( ::floor ( min ) ) );
  _distanceSpinBox->setMaximum ( static_cast < int > ( ::ceil  ( max ) ) );
  //_distanceSpinBox->setSingleStep ( ( max - min ) *0.01 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The spin box has changed.
//
///////////////////////////////////////////////////////////////////////////////

void ClipPlanes::_distanceSpinBoxChanged ( double value )
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  this->_setDistance( value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the distance.
//
///////////////////////////////////////////////////////////////////////////////

void ClipPlanes::_setDistance ( double value )
{
  USUL_TRACE_SCOPE;
  
  // Disable signals during this function.
  QtTools::ScopedSignals s0 ( *_distanceSlider );
  QtTools::ScopedSignals s1 ( *_distanceSpinBox );
  
  Usul::Interfaces::IClippingPlanes::QueryPtr cp ( _caller );

  if ( false == cp.valid () )
    return;

  int index ( _clipPlaneList->currentRow() );

  if ( index < 0 )
    return;

  osg::Plane plane ( cp->getClippingPlane( index ) );
  plane[3] = value;
  cp->setClippingPlane ( index, plane );
  
  // Set the value.
  _distanceSlider->setValue ( static_cast < int > ( value ) );
  _distanceSpinBox->setValue ( value );

  // Draw.
  this->_render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear all clipping planes.
//
///////////////////////////////////////////////////////////////////////////////

void ClipPlanes::on_clearButton_clicked()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  Usul::Interfaces::IClippingPlanes::QueryPtr cp ( _caller );

  if ( false == cp.valid () )
    return;

  // Make sure checks are all unchecked.
  QRadioButton *buttons[] = { posXAxis, posYAxis, posZAxis, negXAxis, negYAxis, negZAxis };
  const unsigned int numButtons ( sizeof ( buttons ) / sizeof ( *buttons ) );
  std::for_each ( buttons, buttons + numButtons, std::bind2nd ( std::mem_fun ( &QRadioButton::setAutoExclusive ), false ) );
  std::for_each ( buttons, buttons + numButtons, std::bind2nd ( std::mem_fun ( &QRadioButton::setChecked ), false ) );
  std::for_each ( buttons, buttons + numButtons, std::bind2nd ( std::mem_fun ( &QRadioButton::setAutoExclusive ), true ) );

  cp->removeClippingPlanes();
  _clipPlaneList->clear();
  this->_render();
}
