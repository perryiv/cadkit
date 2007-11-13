
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Layers/Layer.h"
#include "Minerva/Core/Visitor.h"

#include "Usul/Functions/GUID.h"
#include "Usul/Functions/ToString.h"
#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IGeometryCenter.h"
#include "Usul/Interfaces/IProjectCoordinates.h"
#include "Usul/Interfaces/IPlanetCoordinates.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Math/NaN.h"
#include "Usul/Trace/Trace.h"

#include "OsgTools/Legend/Text.h"
#include "OsgTools/Legend/LegendObject.h"

#include "osg/Group"
#include "osg/MatrixTransform"

#include "osgText/Text"

#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>

using namespace Minerva::Core::Layers;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Layer, Layer::BaseClass );

SERIALIZE_XML_DECLARE_VECTOR_4_WRAPPER ( osg::Vec4 );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Layer::Layer() : BaseClass(),
_guid ( Usul::Functions::GUID::generate() ),
_name( "Layer" ),
_primaryKeyColumn( "id" ),
_tablename(),
_labelColumn(),
_query(),
_renderBin ( 0 ),
_xOffset ( 0.0 ),
_yOffset ( 0.0 ),
_zOffset ( 0.0 ),
_dataObjects(),
_connection(),
_colorFunctor( 0x0 ),
_legendText( "" ),
_showInLegend ( true ),
_showLabel ( false ),
_shown ( true ),
_labelColor( 1.0, 1.0, 1.0, 1.0 ),
_labelZOffset( 1000.0 ),
_labelSize ( 25.0f ),
_colorColumn(),
_customQuery ( false ),
_legendFlags ( 0x0 ),
_minMax ( std::numeric_limits< double >::max(), std::numeric_limits< double >::min() ),
_alpha ( 1.0f ),
SERIALIZE_XML_INITIALIZER_LIST
{
  USUL_TRACE_SCOPE;

  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Layer::Layer( const Layer& layer )  : BaseClass(),
_guid ( layer._guid ),
_name( layer._name ),
_primaryKeyColumn( layer._primaryKeyColumn ),
_tablename( layer._tablename ),
_labelColumn( layer._labelColumn ),
_query( layer._query ),
_renderBin ( layer._renderBin ),
_xOffset ( layer._xOffset ),
_yOffset ( layer._yOffset ),
_zOffset ( layer._zOffset ),
_dataObjects( layer._dataObjects ),
_connection( layer._connection ),
_colorFunctor( 0x0 ),
_legendText( layer._legendText ),
_showInLegend ( layer._showInLegend ),
_showLabel ( layer._showLabel ),
_shown ( layer._shown ),
_labelColor( layer._labelColor ),
_labelZOffset( layer._labelZOffset ),
_labelSize ( layer._labelSize ),
_colorColumn( layer._colorColumn ),
_customQuery( layer._customQuery ),
_legendFlags ( layer._legendFlags ),
_minMax( layer._minMax ),
_alpha ( layer._alpha )
{
  USUL_TRACE_SCOPE;

  if( layer._colorFunctor.valid() )
    _colorFunctor = layer._colorFunctor->clone();

  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register members.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::_registerMembers()
{
  SERIALIZE_XML_ADD_MEMBER ( _guid );
  SERIALIZE_XML_ADD_MEMBER ( _name );
  SERIALIZE_XML_ADD_MEMBER ( _primaryKeyColumn );
  SERIALIZE_XML_ADD_MEMBER ( _tablename );
  SERIALIZE_XML_ADD_MEMBER ( _labelColumn );
  SERIALIZE_XML_ADD_MEMBER ( _query );
  SERIALIZE_XML_ADD_MEMBER ( _renderBin );
  SERIALIZE_XML_ADD_MEMBER ( _xOffset );
  SERIALIZE_XML_ADD_MEMBER ( _yOffset );
  SERIALIZE_XML_ADD_MEMBER ( _zOffset );
  //DataObjects _dataObjects;
  SERIALIZE_XML_ADD_MEMBER ( _connection );
  SERIALIZE_XML_ADD_MEMBER ( _colorFunctor );
  SERIALIZE_XML_ADD_MEMBER ( _legendText );
  SERIALIZE_XML_ADD_MEMBER ( _showInLegend );
  SERIALIZE_XML_ADD_MEMBER ( _showLabel );
  SERIALIZE_XML_ADD_MEMBER ( _shown );
  SERIALIZE_XML_ADD_MEMBER ( _labelColor );
  SERIALIZE_XML_ADD_MEMBER ( _labelZOffset );
  SERIALIZE_XML_ADD_MEMBER ( _labelSize );
  SERIALIZE_XML_ADD_MEMBER ( _colorColumn );
  SERIALIZE_XML_ADD_MEMBER ( _customQuery );
  SERIALIZE_XML_ADD_MEMBER ( _legendFlags );
  SERIALIZE_XML_ADD_MEMBER ( _alpha );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Layer::~Layer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accept the visitor.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::accept ( Minerva::Core::Visitor& visitor )
{
  visitor.visit ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse all DataObjects.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::traverse ( Minerva::Core::Visitor& visitor )
{
  for ( DataObjects::iterator iter = _dataObjects.begin(); iter != _dataObjects.end(); ++iter )
    (*iter)->accept ( visitor );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the connection.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::connection ( Minerva::Core::DB::Connection *connection )
{
  Guard guard ( this->mutex() );
  _connection = connection;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the connection.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::DB::Connection* Layer::connection ()
{
  Guard guard ( this->mutex() );
  return _connection;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the tablename.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::tablename( const std::string& table )
{
  Guard guard ( this->mutex() );
  _tablename = table;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the tablename.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& Layer::tablename() const
{
  Guard guard( this->mutex() );
  return _tablename;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the label column.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::labelColumn( const std::string& column )
{
  Guard guard( this->mutex() );
  _labelColumn = column;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the label column.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& Layer::labelColumn() const
{
  Guard guard( this->mutex() );
  return _labelColumn;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the render bin.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::renderBin( Usul::Types::Uint32 bin )
{
  Guard guard( this->mutex() );
  _renderBin = bin;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the render bin.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Types::Uint32 Layer::renderBin( ) const
{
  Guard guard( this->mutex() );
  return _renderBin;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::buildScene( osg::Group* parent )
{
  Guard guard( this->mutex() );

  for( DataObjects::iterator iter = _dataObjects.begin(); iter != _dataObjects.end(); ++iter )
  {
    parent->addChild( (*iter)->buildScene() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the query for data.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::query ( const std::string& query )
{
  Guard guard( this->mutex() );
  _query = query;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the query for data.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& Layer::query ( ) const
{
  Guard guard( this->mutex() );
  return _query;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a data object.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::_addDataObject ( Minerva::Core::DataObjects::DataObject *dataObject )
{
  Guard guard ( this->mutex() );
  _dataObjects.push_back( dataObject );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear all the data objects.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::_clearDataObjects ()
{
  Guard guard ( this->mutex() );
  _dataObjects.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get all the data objects.
//
///////////////////////////////////////////////////////////////////////////////

Layer::DataObjects& Layer::_getDataObjects()
{
  return _dataObjects;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does this layer contain temporal data?
//
///////////////////////////////////////////////////////////////////////////////

bool Layer::isTemporal() const
{
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the color functor.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::colorFunctor( Minerva::Core::Functors::BaseColorFunctor *colorFunctor )
{
  Guard guard ( this->mutex() );
  _colorFunctor = colorFunctor;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the color functor.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Functors::BaseColorFunctor * Layer::colorFunctor()
{
  Guard guard ( this->mutex() );
  return _colorFunctor.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the color functor.
//
///////////////////////////////////////////////////////////////////////////////

const Minerva::Core::Functors::BaseColorFunctor * Layer::colorFunctor() const
{
  Guard guard ( this->mutex() );
  return _colorFunctor.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the x offset.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::xOffset( float f )
{
  Guard guard ( this->mutex() );
  _xOffset = f;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the x offset.
//
///////////////////////////////////////////////////////////////////////////////

float Layer::xOffset( ) const
{
  Guard guard ( this->mutex() );
  return _xOffset;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the y offset.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::yOffset( float f )
{
  Guard guard ( this->mutex() );
  _yOffset = f;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the y offset.
//
///////////////////////////////////////////////////////////////////////////////

float Layer::yOffset( ) const
{
  Guard guard ( this->mutex() );
  return _yOffset;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the z offset.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::zOffset( float f )
{
  Guard guard ( this->mutex() );
  _zOffset = f;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the z offset.
//
///////////////////////////////////////////////////////////////////////////////

float Layer::zOffset( ) const
{
  Guard guard ( this->mutex() );
  return _zOffset;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the legend text.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::legendText( const std::string& text )
{
  Guard guard ( this->mutex() );
  _legendText = text;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the legend text.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& Layer::legendText() const
{
  Guard guard ( this->mutex() );
  return _legendText;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the show label.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::showLabel( bool b )
{
  Guard guard ( this->mutex() );
  _showLabel = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the show label.
//
///////////////////////////////////////////////////////////////////////////////

bool Layer::showLabel() const
{
  Guard guard ( this->mutex() );
  return _showLabel;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the show label.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::showLayer( bool b )
{
  Guard guard ( this->mutex() );
  _shown = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the show label.
//
///////////////////////////////////////////////////////////////////////////////

bool Layer::showLayer() const
{
  Guard guard ( this->mutex() );
  return _shown;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the label color.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::labelColor( const osg::Vec4& color )
{
  Guard guard ( this->mutex() );
  _labelColor = color;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the label color.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec4& Layer::labelColor() const
{
  Guard guard ( this->mutex() );
  return _labelColor;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the label z offset.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::labelZOffset( float offset )
{
  Guard guard ( this->mutex() );
  _labelZOffset = offset;
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the label z offset.
//
///////////////////////////////////////////////////////////////////////////////

float Layer::labelZOffset() const
{
  Guard guard ( this->mutex() );
  return _labelZOffset;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the color column.
//
///////////////////////////////////////////////////////////////////////////////

void  Layer::colorColumn( const std::string& column )
{
  Guard guard ( this->mutex() );
  _colorColumn = column;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the color column.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& Layer::colorColumn() const
{
  Guard guard ( this->mutex() );
  return _colorColumn;
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the number of data objects in this layer.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Layer::number() const
{
  return _dataObjects.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Label the data object.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::_setDataObjectMembers ( Minerva::Core::DataObjects::DataObject* dataObject )
{
  dataObject->renderBin ( this->renderBin() );
  dataObject->connection ( this->connection() );

  // Label parameters.
  dataObject->showLabel ( this->showLabel() );
  dataObject->labelColor( this->labelColor() );
  dataObject->labelSize( _labelSize );

  // If we have a column to use for a label.
  if( this->showLabel() && !this->labelColumn().empty() )
  {
    std::string value ( this->connection()->getColumnDataString( dataObject->tableName(), dataObject->rowId(), this->labelColumn() ) );

    dataObject->label( value );
  }

  osg::Vec3 center;
  unsigned int srid ( 0 );
  Usul::Interfaces::IGeometryCenter::QueryPtr geometryCenter ( dataObject->geometry() );
  if( geometryCenter.valid() )
  {
    center = geometryCenter->geometryCenter( srid );
  }

  Usul::Interfaces::IProjectCoordinates::QueryPtr project ( Usul::Components::Manager::instance().getInterface( Usul::Interfaces::IProjectCoordinates::IID ) );
  Usul::Interfaces::IPlanetCoordinates::QueryPtr  planet  ( Usul::Components::Manager::instance().getInterface( Usul::Interfaces::IPlanetCoordinates::IID ) );

  if( project.valid() )
  {
    Usul::Math::Vec3d orginal;
    orginal[0] = center[0];
    orginal[1] = center[1];
    orginal[2] = this->labelZOffset();
    Usul::Math::Vec3d point;
    project->projectToSpherical( orginal, srid, point );

    if( planet.valid() )
    {
      planet->convertToPlanet( point, point );
      center.set ( point[0], point[1], point[2] );
    }
  }

  dataObject->labelPosition( center );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the label size.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::labelSize( float size )
{
  Guard guard( this->mutex() );
  _labelSize = size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the label size.
//
///////////////////////////////////////////////////////////////////////////////

float Layer::labelSize() const
{
  Guard guard( this->mutex() );
  return _labelSize;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::name( const std::string& name )
{
  Guard guard( this->mutex() );
  _name = name;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name.
//
///////////////////////////////////////////////////////////////////////////////

std::string Layer::name() const
{
  Guard guard ( this->mutex() );
  return _name;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the default query.
//
///////////////////////////////////////////////////////////////////////////////

std::string Layer::defaultQuery() const
{
  // Get the geometry colomn.
  const std::string geomColumn ( this->geometryColumn() );

  // Build the query.
  std::ostringstream query;
  query << "SELECT " << this->primaryKeyColumn() << " as id, srid(" << geomColumn << ") as srid, asBinary(" << geomColumn << ") as geom";
  if ( this->colorColumn().size() > 0 )
  {
    query << ", " << this->colorColumn();
  }
  
  query << " FROM " << this->tablename( );
  return query.str();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the primary key column.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::primaryKeyColumn( const std::string& value )
{
  _primaryKeyColumn = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the primary key column.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& Layer::primaryKeyColumn() const
{
  return _primaryKeyColumn;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the geometry column name.
//
///////////////////////////////////////////////////////////////////////////////

std::string Layer::geometryColumn() const
{
  return "geom";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the custom query flag.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::customQuery( bool value )
{
  _customQuery = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the custom query flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Layer::customQuery() const
{
  return _customQuery;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the guid.
//
///////////////////////////////////////////////////////////////////////////////

std::string Layer::guid() const
{
  return _guid;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set show count in legend.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::showCountLegend( bool b )
{
  if ( b )
    _legendFlags = Usul::Bits::add( _legendFlags, COUNT );
  else
    _legendFlags = Usul::Bits::remove( _legendFlags, COUNT );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get show count in legend.
//
///////////////////////////////////////////////////////////////////////////////

bool Layer::showCountLegend() const
{
  return Usul::Bits::has( _legendFlags, COUNT );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set show min in legend.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::showMinLegend( bool b )
{
  if ( b )
    _legendFlags = Usul::Bits::add( _legendFlags, MIN );
  else
    _legendFlags = Usul::Bits::remove( _legendFlags, MIN );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get show min in legend.
//
///////////////////////////////////////////////////////////////////////////////

bool Layer::showMinLegend() const
{
  return Usul::Bits::has( _legendFlags, MIN );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set show max in legend.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::showMaxLegend( bool b )
{
  if ( b )
    _legendFlags = Usul::Bits::add( _legendFlags, MAX );
  else
    _legendFlags = Usul::Bits::remove( _legendFlags, MAX );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get show max in legend.
//
///////////////////////////////////////////////////////////////////////////////

bool Layer::showMaxLegend() const
{
  return Usul::Bits::has( _legendFlags, MAX );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* Layer::queryInterface( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::ILayer::IID:
    return static_cast < Usul::Interfaces::ILayer* > ( this );
  case Usul::Interfaces::IVectorLayer::IID:
    return static_cast < Usul::Interfaces::IVectorLayer* > ( this );
  case Usul::Interfaces::IAddRowLegend::IID:
    return static_cast < Usul::Interfaces::IAddRowLegend* > ( this );
  case Usul::Interfaces::ISerialize::IID:
    return static_cast < Usul::Interfaces::ISerialize* > ( this );
  case Usul::Interfaces::IClonable::IID:
    return static_cast < Usul::Interfaces::IClonable* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the vector data.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::buildVectorData  ( Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress )
{
  // Clear what we have.
  this->_clearDataObjects();

  // Set the query.
  if( true == _query.empty() && false == this->customQuery() )
    _query = this->defaultQuery();

  // Query for a status bar.
  Usul::Interfaces::IStatusBar::UpdateStatusBar status ( progress );

  // Give feedback.
  status ( "Building " + this->name() );

  // Build the data objects.
  this->buildDataObjects( caller, progress );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Modify the vector data.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::modifyVectorData ( Usul::Interfaces::IUnknown *caller )
{
  this->modify ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a legend row.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::addLegendRow ( OsgTools::Legend::LegendObject* row )
{
  try
  {
    if( 0x0 != row )
    {
      if( 0x0 != this->colorFunctor() )
        row->icon ( this->colorFunctor()->icon( this->queryInterface( Usul::Interfaces::IUnknown::IID ) ) );

      // One columns for the text
      row->columns ( 1 );
      row->at ( 0 )->text ( this->legendText() );
      

      if( this->showCountLegend() )
      {
        unsigned int index ( row->addText ( new OsgTools::Legend::Text ( Usul::Functions::toString( this->number() ) ) ) );
	      row->at ( index )->alignment ( OsgTools::Legend::Text::RIGHT );
        row->percentage( index ) = 0.20;
      }

      if( this->showMinLegend() )
      {
        double min ( _minMax.first );
        bool valid ( min != std::numeric_limits< double >::max() && !Usul::Math::nan( min ) );
        
        std::string text ( valid ? Usul::Functions::toString( min ) : "NA" );
        
        row->addText ( new OsgTools::Legend::Text ( Usul::Functions::toString( min ) ) );
      }

      if( this->showMaxLegend() )
      {
        double max ( _minMax.second );
        bool valid ( max != std::numeric_limits< double >::min() && !Usul::Math::nan( max ) );
        
        std::string text ( valid ? Usul::Functions::toString( max ) : "NA" );
        
        row->addText ( new OsgTools::Legend::Text ( Usul::Functions::toString( max ) ) );
      }

      /// Find out how many columns we have.
      unsigned int numColumns ( row->columns() );

      for( unsigned int i = 0; i < numColumns; ++i )
        row->percentage( i ) = .20;

      row->percentage( 0 ) = 1.0 - ( ( numColumns - 1 ) * .20 );
    }
  }
  catch ( const std::exception& e )
  {
    std::cout << "Error 3665976713: " << e.what() << std::endl;
  }
  catch ( ... )
  {
    std::cout << "Error 4254986090: Unknown exception caught." << std::endl;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the min and max.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::_updateMinMax ( double value )
{
  if( value < _minMax.first )
    _minMax.first = value;
  if ( value > _minMax.second )
    _minMax.second = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the alpha value.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::alpha ( float a )
{
  Guard guard( this->mutex() );
  _alpha = a;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the alpha value.
//
///////////////////////////////////////////////////////////////////////////////

float Layer::alpha () const
{
  Guard guard( this->mutex() );
  return _alpha;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set flag to show layer in legend.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::showInLegend ( bool b )
{
  Guard guard( this->mutex() );
  _showInLegend = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get flag to show layer in legend.
//
///////////////////////////////////////////////////////////////////////////////

bool Layer::showInLegend () const
{
  Guard guard( this->mutex() );
  return _showInLegend;
}
