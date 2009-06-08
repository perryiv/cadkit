
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all data objects.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Data/DataObject.h"
#include "Minerva/Core/Data/UserData.h"
#include "Minerva/Core/Visitor.h"

#include "OsgTools/Convert.h"
#include "OsgTools/Font.h"
#include "OsgTools/Widgets/Legend.h"
#include "OsgTools/Widgets/LegendObject.h"
#include "OsgTools/Widgets/Text.h"

#include "Usul/Interfaces/IPlanetCoordinates.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Threads/Safe.h"

#include "osg/BlendFunc"
#include "osg/Geode"
#include "osg/Group"
#include "osgText/Text"

#include <limits>

using namespace Minerva::Core::Data;

USUL_IMPLEMENT_TYPE_ID ( DataObject );
USUL_IMPLEMENT_TYPE_ID ( ClickedCallback );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( DataObject, DataObject::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DataObject::DataObject() :
  BaseClass(),
  _dirty ( true ),
  _label(),
  _labelPosition ( 0.0, 0.0, 1000.0 ),
  _labelColor ( 1.0, 1.0, 1.0, 1.0 ),
  _labelSize ( 25.0f ),
  _showLabel ( false ),
  _dataSource ( static_cast < Usul::Interfaces::IUnknown* > ( 0x0 ) ),
  _geometries(),
  _clickedCallback ( 0x0 ),
  _propagateIntersections ( true )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DataObject::~DataObject()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* DataObject::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast<Usul::Interfaces::IBuildScene*> ( this );
  case Minerva::Interfaces::IElevationChangedListener::IID:
    return static_cast<Minerva::Interfaces::IElevationChangedListener*> ( this );
  case Usul::Interfaces::IBooleanState::IID:
    return static_cast<Usul::Interfaces::IBooleanState*> ( this );
  case Usul::Interfaces::IUpdateListener::IID:
    return static_cast<Usul::Interfaces::IUpdateListener*> ( this );
  case Minerva::Interfaces::IDataObject::IID:
    return static_cast<Minerva::Interfaces::IDataObject*> ( this );
  case Minerva::Interfaces::IWithinExtents::IID:
    return static_cast<Minerva::Interfaces::IWithinExtents*> ( this );
  case Minerva::Interfaces::IIntersectNotify::IID:
    return static_cast<Minerva::Interfaces::IIntersectNotify*> ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accept the visitor.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::accept ( Minerva::Core::Visitor& visitor )
{
  visitor.visit ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a geometry.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::addGeometry ( Geometry::RefPtr geometry )
{
  if ( true == geometry.valid() )
  {
    Guard guard ( this );
    _geometries.push_back ( geometry );
    
    this->_updateExtents ( Usul::Interfaces::IUnknown::QueryPtr ( geometry ) );
    
    this->dirty ( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

bool DataObject::dirty() const
{
  Guard guard ( this );
  return _dirty;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::dirty( bool b )
{
  Guard guard ( this );
  _dirty = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the label.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::label ( const std::string& label )
{
  Guard guard ( this );
  _label = label;
  this->dirty( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the label.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& DataObject::label() const
{
  Guard guard ( this );
  return _label;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the label position.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::labelPosition ( const PositionType& position )
{
  Guard guard ( this );
  _labelPosition = position;
  this->dirty( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the label position.
//
///////////////////////////////////////////////////////////////////////////////

DataObject::PositionType DataObject::labelPosition () const
{
  Guard guard ( this );
  return _labelPosition;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the geometry.
//
///////////////////////////////////////////////////////////////////////////////

DataObject::Geometries DataObject::geometries() const
{
  Guard guard ( this );
  return _geometries;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the color.
//
///////////////////////////////////////////////////////////////////////////////

DataObject::ColorType DataObject::labelColor() const
{
  Guard guard ( this );
  return _labelColor;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the color.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::labelColor ( const ColorType& color )
{
  Guard guard ( this );
  // Set the lable color.
  _labelColor = color;
  this->dirty( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the label size.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::labelSize( float size )
{
  Guard guard ( this );
  _labelSize = size;
  this->dirty( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the label size.
//
///////////////////////////////////////////////////////////////////////////////

float DataObject::labelSize() const
{
  Guard guard ( this );
  return _labelSize;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the connection.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::dataSource ( Unknown* c )
{
  Guard guard ( this );
  _dataSource = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the connection.
//
///////////////////////////////////////////////////////////////////////////////

DataObject::Unknown* DataObject::dataSource()
{
  Guard guard ( this );
  return _dataSource.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the data source.
//
///////////////////////////////////////////////////////////////////////////////

const DataObject::Unknown* DataObject::dataSource() const
{
  Guard guard ( this );
  return _dataSource.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the node for the label.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* DataObject::_buildLabel ( const PositionType& position )
{
  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  
  const std::string label ( this->label() );

  if ( this->showLabel () && false == label.empty() )
  {
    osg::ref_ptr < osgText::Text > text ( new osgText::Text );
    text->setFont( OsgTools::Font::defaultFont() );
    text->setColor( Usul::Convert::Type<ColorType,osg::Vec4f>::convert ( this->labelColor() ) );
    text->setPosition ( Usul::Convert::Type<PositionType,osg::Vec3d>::convert ( position ) );
    text->setAutoRotateToScreen( true );
    text->setCharacterSizeMode( osgText::Text::SCREEN_COORDS );
    text->setCharacterSize( this->labelSize() );

    text->setText ( this->label() );

    geode->addDrawable( text.get() );
  }

  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flag to show the label.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::showLabel ( bool value )
{
  Guard guard ( this );
  _showLabel = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flag to show the label.
//
///////////////////////////////////////////////////////////////////////////////

bool DataObject::showLabel () const
{
  Guard guard ( this );
  return _showLabel;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::preBuildScene ( Usul::Interfaces::IUnknown* caller )
{
  Geometries geometries ( this->geometries() );
  
  osg::ref_ptr<osg::Group> group ( new osg::Group );
  
  Extents extents;
  
  // Does the scene need to be sorted?
  bool isTransparent ( false );
  
  for ( Geometries::iterator iter = geometries.begin(); iter != geometries.end(); ++iter )
  {
    Geometry::RefPtr geometry ( *iter );
    if ( true == geometry.valid() )
    {    
      // Build the scene for the geometry.
      osg::ref_ptr<osg::Node> node ( geometry->buildScene ( caller ) );

      // Expand the extents by the geometry's extents.
      // This needs to be called after the scene is built because the extents may be updated in Geometry::buildScene.
      extents.expand ( geometry->extents() );
      
      if ( node.valid() )
      {
        // Add user data.
        node->setUserData ( new Minerva::Core::Data::UserData ( this->objectId() ) );

        group->addChild ( node.get() );
        
        // See if the geometry is transparent.
        if ( true == geometry->isSemiTransparent() )
        {
          isTransparent = true;
        }
      }
    }
  }
  
  // If one of the geometries is transparent, set the proper state and add needed callbacks.
  if ( isTransparent )
  {
    // Get the state set.
    osg::ref_ptr < osg::StateSet > ss ( group->getOrCreateStateSet () );
    
    // Add a blend function.
    osg::ref_ptr<osg::BlendFunc> blend ( new osg::BlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) );
    ss->setAttributeAndModes ( blend.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );
    
    ss->setRenderingHint ( osg::StateSet::TRANSPARENT_BIN );
    ss->setRenderBinDetails ( 1, "DepthSortedBin" );
  }

  // Set the new extents.
  this->extents ( extents );

  // Do we have a label?
  if( this->showLabel() && !this->label().empty() )
  {
    PositionType position ( this->labelPosition() );
    Usul::Math::Vec3d p ( extents.center()[0], extents.center()[1], position[2] );

    Usul::Interfaces::IPlanetCoordinates::QueryPtr planet ( caller );

    if( planet.valid() )
    {
      planet->convertToPlanet( Usul::Math::Vec3d ( p ), p );
    }

    group->addChild ( this->_buildLabel ( PositionType ( p[0], p[1], p[2] ) ) );
  }

  Guard guard ( this );
  _preBuiltScene = group.get();

  this->dirty ( false );  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* DataObject::buildScene ( const Options& options, Usul::Interfaces::IUnknown* caller )
{
  // Build the scene if we need to.
  if ( this->dirty() )
    this->preBuildScene( caller );

  Guard guard ( this );

  // Switch the pre built scene for what we hand back to osg.
  // This is safer in a multi-threaded environment.
  if( _preBuiltScene.valid() )
  {
    // Get the visibilty state.
    const bool visible ( BaseClass::visibility() );
    
    _root = _preBuiltScene;
    _preBuiltScene = 0x0;

    // Set the visibility state.
    this->visibility ( visible );
  }

  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the visibilty flag.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::visibility ( bool b )
{
  BaseClass::visibility ( b );
 
  Guard guard ( this );

  if ( _root.valid () )
  {
    const unsigned int nodeMask ( b ? 0xffffffff : 0x0 );
    _root->setNodeMask ( nodeMask );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name (ITreeNode).
//
///////////////////////////////////////////////////////////////////////////////

std::string DataObject::getTreeNodeName() const
{
  USUL_TRACE_SCOPE;
  return ( false == this->name().empty() ? this->name() : this->label() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  DataObject has been clicked.
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::Widgets::Item* DataObject::clicked ( Usul::Interfaces::IUnknown* caller ) const
{
  ClickedCallback::RefPtr cb ( this->clickedCallback() );
  
  // Use the callback if we have one.
  if ( cb.valid() )
    return (*cb)( *this, caller );
  
  OsgTools::Widgets::Legend::RefPtr legend ( new OsgTools::Widgets::Legend );
  legend->maximiumSize ( 300, 300 );
  legend->position ( 10, 10 );
  legend->growDirection ( OsgTools::Widgets::Legend::GROW_DIRECTION_UP );
  
  OsgTools::Widgets::LegendObject::RefPtr row0 ( new OsgTools::Widgets::LegendObject );
  
  // Make some text.
  OsgTools::Widgets::Text::RefPtr text0 ( new OsgTools::Widgets::Text );
  text0->text ( this->name() );
  text0->wrapLine ( false );
  text0->autoSize ( false );
  text0->alignmentVertical ( OsgTools::Widgets::Text::TOP );
  text0->fontSize ( 15 );
  
  // Add the items.
  row0->addItem ( text0.get() );
  
  // Set the percentage of the row.
  row0->percentage ( 0 ) = 1.00;
  
  const std::string description ( this->description() );
  if ( false == description.empty() )
  {
    OsgTools::Widgets::LegendObject::RefPtr row1 ( new OsgTools::Widgets::LegendObject );
    
    // Make some text.
    OsgTools::Widgets::Text::RefPtr text ( new OsgTools::Widgets::Text );
    text->text ( description );
    text->wrapLine ( true );
    text->alignmentVertical ( OsgTools::Widgets::Text::TOP );
    text->fontSize ( 15 );
    
    // Add the items.
    row1->addItem ( text.get() );
    
    // Set the percentage of the row.
    row1->percentage ( 0 ) = 1.00;
    
    legend->addRow ( row1.get() );
  }
  
  legend->addRow ( row0.get() );
  
  return legend.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Elevation has changed within given extents (IElevationChangedListnerer).
//
///////////////////////////////////////////////////////////////////////////////

bool DataObject::elevationChangedNotify ( const Extents& extents, unsigned int level, ElevationDataPtr elevationData, Unknown * caller )
{
  Extents e ( this->extents() );

  if ( e.intersects ( extents ) )
  {
#if 0
    Geometries geometries ( this->geometries() );
    for ( Geometries::iterator iter = geometries.begin(); iter != geometries.end(); ++iter )
    {
      Geometry::RefPtr geometry ( *iter );
      geometry->elevationChangedNotify ( extents, level, elevationData, caller );
    }
#else
    this->dirty ( true );
    this->preBuildScene ( caller );
#endif
    return true;
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state (IBooleanState).
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::setBooleanState ( bool b )
{
  this->visibility ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the state (IBooleanState).
//
///////////////////////////////////////////////////////////////////////////////

bool DataObject::getBooleanState() const
{
  return BaseClass::visibility();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the clicked callback.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::clickedCallback ( ClickedCallback::RefPtr cb )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _clickedCallback = cb;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the clicked callback.
//
///////////////////////////////////////////////////////////////////////////////

ClickedCallback::RefPtr DataObject::clickedCallback() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _clickedCallback;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ClickedCallback::ClickedCallback()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ClickedCallback::~ClickedCallback()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::updateNotify ( Usul::Interfaces::IUnknown* caller )
{
  USUL_TRACE_SCOPE;
  Geometries geometries ( this->geometries() );
  for ( Geometries::iterator iter = geometries.begin(); iter != geometries.end(); ++iter )
  {
    Geometry::RefPtr geometry ( *iter );
    geometry->updateNotify ( caller );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the pointer to this (IDataObject).
//
///////////////////////////////////////////////////////////////////////////////

DataObject* DataObject::dataObject()
{
  USUL_TRACE_SCOPE;
  return this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the data object empty?
//
///////////////////////////////////////////////////////////////////////////////

bool DataObject::empty() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _geometries.empty();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the items within the extents.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown::RefPtr DataObject::getItemsWithinExtents ( double minLon, double minLat, double maxLon, double maxLat, IUnknown::RefPtr caller ) const
{
  USUL_TRACE_SCOPE;

  // Initialize.
  DataObject::RefPtr answer ( new DataObject );
  Extents givenExtents ( minLon, minLat, maxLon, maxLat );

  // Get a copy of the geometries.
  const Geometries geometries ( Usul::Threads::Safe::get ( this->mutex(), _geometries ) );

  // Loop through the geometries.
  for ( Geometries::const_iterator i = geometries.begin(); i != geometries.end(); ++i )
  {
    // Does this layer have extents?
    const Usul::Interfaces::ILayerExtents::QueryPtr e ( i->get() );
    if ( true == e.valid() )
    {
      // Calculate the center.
      const Extents layerExtents ( e->minLon(), e->minLat(), e->maxLon(), e->maxLat() );
      const Extents::Vertex center ( layerExtents.center() );

      // Is the center in the extents?
      if ( true == givenExtents.contains ( center ) )
      {
        answer->addGeometry ( *i );
      }
    }
  }

  // Return the answer.
  return IUnknown::QueryPtr ( ( false == answer->empty() ) ? answer.get() : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Call to notify of an intersection.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::intersectNotify ( double x, double y, double z, double lon, double lat, double elev, 
                                   IUnknown::RefPtr tile, IUnknown::RefPtr body, IUnknown::RefPtr caller, Closest &answer )
{
  USUL_TRACE_SCOPE;
  typedef IIntersectNotify::Path Path;
  typedef IIntersectNotify::Point Point;
  typedef IIntersectNotify::PointAndDistance PointAndDistance;

  // Should we pass this along?
  if ( false == this->propagateIntersections() )
    return;

  // Get a copy of the geometries.
  Geometries geometries ( Usul::Threads::Safe::get ( this->mutex(), _geometries ) );

  // Loop through the geometries.
  for ( Geometries::iterator i = geometries.begin(); i != geometries.end(); ++i )
  {
    IIntersectNotify::QueryPtr notify ( i->get() );
    if ( true == notify.valid() )
    {
      Closest closest ( Path(), PointAndDistance ( Point(), std::numeric_limits<double>::max() ) );
      notify->intersectNotify ( x, y, z, lon, lat, elev, tile, body, caller, closest );

      if ( ( closest.second.second < answer.second.second ) && ( false == closest.first.empty() ) )
      {
        answer.first.insert ( answer.first.end(), Usul::Interfaces::IUnknown::QueryPtr ( this ) );
        answer.first.insert ( answer.first.end(), closest.first.begin(), closest.first.end() );
        answer.second = closest.second;
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the flag that says to propagate intersection notifications.
//
///////////////////////////////////////////////////////////////////////////////

bool DataObject::propagateIntersections() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _propagateIntersections;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flag that says to propagate intersection notifications.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::propagateIntersections ( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _propagateIntersections = state;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reserve geometry size.
//
///////////////////////////////////////////////////////////////////////////////

void DataObject::reserveGeometry ( unsigned int size )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _geometries.reserve ( size );
}
