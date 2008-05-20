
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/PostGIS/Layer.h"
#include "Minerva/Layers/PostGIS/BinaryParser.h"

#include "Minerva/Core/Visitor.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Components/Manager.h"
#include "Usul/Convert/Vector2.h"
#include "Usul/Functions/GUID.h"
#include "Usul/Interfaces/IProjectCoordinates.h"
#include "Usul/Interfaces/GUI/IProgressBarFactory.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Math/NaN.h"
#include "Usul/Resources/Constants.h"
#include "Usul/Resources/Manager.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Strings/Format.h"
#include "Usul/Strings/Split.h"

#include "OsgTools/Legend/Text.h"
#include "OsgTools/Legend/LegendObject.h"

#include "osg/Group"
#include "osg/MatrixTransform"

#include "osgText/Text"

#include "pqxx/pqxx"

#include "boost/algorithm/string/erase.hpp"

#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>

using namespace Minerva::Layers::PostGIS;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Layer, Layer::BaseClass );

USUL_IO_TEXT_DEFINE_READER_TYPE_VECTOR_4 ( osg::Vec4 );
USUL_IO_TEXT_DEFINE_WRITER_TYPE_VECTOR_4 ( osg::Vec4 );
SERIALIZE_XML_DECLARE_VECTOR_4_WRAPPER ( osg::Vec4 );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Layer::Layer() : BaseClass(),
  _primaryKeyColumn( "id" ),
  _tablename(),
  _labelColumn(),
  _query(),
  _renderBin ( 0 ),
  _xOffset ( 0.0 ),
  _yOffset ( 0.0 ),
  _zOffset ( 0.0 ),
  _connection(),
  _colorFunctor( 0x0 ),
  _legendText( "" ),
  _showInLegend ( true ),
  _showLabel ( false ),
  _labelColor( 1.0, 1.0, 1.0, 1.0 ),
  _labelZOffset( 1000.0 ),
  _labelSize ( 25.0f ),
  _colorColumn(),
  _customQuery ( false ),
  _legendFlags ( 0 ),
  _minMax ( std::numeric_limits< double >::max(), std::numeric_limits< double >::min() ),
  _alpha ( 1.0f ),
  _updating ( false ),
  _firstDateColumn(),
  _lastDateColumn(),
  _minDate( boost::date_time::min_date_time ),
  _maxDate( boost::date_time::max_date_time )
{
  USUL_TRACE_SCOPE;

  this->_registerMembers();
  
  this->dirtyData ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Layer::Layer( const Layer& layer )  : 
  BaseClass( layer ),
  _primaryKeyColumn( layer._primaryKeyColumn ),
  _tablename( layer._tablename ),
  _labelColumn( layer._labelColumn ),
  _query( layer._query ),
  _renderBin ( layer._renderBin ),
  _xOffset ( layer._xOffset ),
  _yOffset ( layer._yOffset ),
  _zOffset ( layer._zOffset ),
  _connection( layer._connection ),
  _colorFunctor( 0x0 ),
  _legendText( layer._legendText ),
  _showInLegend ( layer._showInLegend ),
  _showLabel ( layer._showLabel ),
  _labelColor( layer._labelColor ),
  _labelZOffset( layer._labelZOffset ),
  _labelSize ( layer._labelSize ),
  _colorColumn( layer._colorColumn ),
  _customQuery( layer._customQuery ),
  _legendFlags ( layer._legendFlags ),
  _minMax( layer._minMax ),
  _alpha ( layer._alpha ),
  _updating ( false ),
  _firstDateColumn( layer._firstDateColumn ),
  _lastDateColumn( layer._lastDateColumn ),
  _minDate( layer._minDate ),
  _maxDate( layer._maxDate )
{
  USUL_TRACE_SCOPE;

  if( layer._colorFunctor.valid() )
    _colorFunctor = layer._colorFunctor->clone();

  this->_registerMembers();
  
  this->dirtyData ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register members.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::_registerMembers()
{
  SERIALIZE_XML_ADD_MEMBER ( _primaryKeyColumn );
  SERIALIZE_XML_ADD_MEMBER ( _tablename );
  SERIALIZE_XML_ADD_MEMBER ( _labelColumn );
  SERIALIZE_XML_ADD_MEMBER ( _query );
  SERIALIZE_XML_ADD_MEMBER ( _renderBin );
  SERIALIZE_XML_ADD_MEMBER ( _xOffset );
  SERIALIZE_XML_ADD_MEMBER ( _yOffset );
  SERIALIZE_XML_ADD_MEMBER ( _zOffset );
  SERIALIZE_XML_ADD_MEMBER ( _connection );
  SERIALIZE_XML_ADD_MEMBER ( _colorFunctor );
  SERIALIZE_XML_ADD_MEMBER ( _legendText );
  SERIALIZE_XML_ADD_MEMBER ( _showInLegend );
  SERIALIZE_XML_ADD_MEMBER ( _showLabel );
  SERIALIZE_XML_ADD_MEMBER ( _labelColor );
  SERIALIZE_XML_ADD_MEMBER ( _labelZOffset );
  SERIALIZE_XML_ADD_MEMBER ( _labelSize );
  SERIALIZE_XML_ADD_MEMBER ( _colorColumn );
  SERIALIZE_XML_ADD_MEMBER ( _customQuery );
  SERIALIZE_XML_ADD_MEMBER ( _legendFlags );
  SERIALIZE_XML_ADD_MEMBER ( _alpha );
  SERIALIZE_XML_ADD_MEMBER ( _firstDateColumn );
  SERIALIZE_XML_ADD_MEMBER ( _lastDateColumn );
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
//  Set the connection.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::connection ( Connection *connection )
{
  Guard guard ( this->mutex() );
  _connection = connection;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the connection.
//
///////////////////////////////////////////////////////////////////////////////

Layer::Connection* Layer::connection ()
{
  Guard guard ( this->mutex() );
  return _connection;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the connection.
//
///////////////////////////////////////////////////////////////////////////////

const Layer::Connection* Layer::connection () const
{
  Guard guard ( this->mutex() );
  return _connection.get();
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
//  Set the query for data.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::query ( const std::string& query )
{
  Guard guard( this->mutex() );
  
  if ( query != _query )
  {
    _query = query;
    this->dirtyData ( true );
  }
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
//  Label the data object.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::_setDataObjectMembers ( Minerva::Core::DataObjects::DataObject* dataObject, Usul::Interfaces::IUnknown* caller )
{
  dataObject->dataSource ( Usul::Interfaces::IUnknown::QueryPtr ( this->connection() ) );

  // Label parameters.
  dataObject->showLabel ( this->showLabel() );
  dataObject->labelColor( this->labelColor() );
  dataObject->labelSize( this->labelSize() );
  
  // Tablename.
  std::string tablename ( this->tablename() );

  // If we have a column to use for a label.
  if( this->showLabel() && !this->labelColumn().empty() )
  {
    int id ( Usul::Convert::Type<std::string, int>::convert ( dataObject->objectId() ) );
    std::string value ( this->connection()->getColumnDataString( this->tablename(), id, this->labelColumn() ) );

    dataObject->label( value );

    // Get the srid.
    unsigned int srid ( this->srid() );
    
    // Build the query.
    std::ostringstream os;
    os << "SELECT x(centroid(" << tablename << ".geom)) as x_c, y(centroid(" << tablename << ".geom)) as y_c, srid(geom) as srid FROM " << tablename << " WHERE id = " << id;
    
    osg::Vec3f center ( 0.0, 0.0, 0.0 );
    
    if( 0x0 != _connection.get() )
    {
      pqxx::result r ( _connection->executeQuery( os.str() ) );
      
      if( !r.empty() && !r[0][0].is_null() && !r[0][1].is_null() )
      {
        Usul::Types::Float64 x ( r[0][0].as< float > () );
        Usul::Types::Float64 y ( r[0][1].as< float > () );
        
        center.set( static_cast<osg::Vec3f::value_type> ( this->xOffset() + x ), static_cast<osg::Vec3f::value_type> ( this->yOffset() + y ), this->zOffset() );
      }
    }
    
    Usul::Interfaces::IProjectCoordinates::QueryPtr project ( Usul::Components::Manager::instance().getInterface( Usul::Interfaces::IProjectCoordinates::IID ) );
    
    if( project.valid() )
    {
      Usul::Math::Vec3d orginal;
      orginal[0] = center[0];
      orginal[1] = center[1];
      orginal[2] = this->labelZOffset();
      Usul::Math::Vec3d point;
      project->projectToSpherical( orginal, srid, point );
      
      center.set ( point[0], point[1], point[2] );
    }
    
    dataObject->labelPosition( center );
  }
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
  
  if ( this->firstDateColumn().size() > 0 )
    query << ", " << this->firstDateColumn();
  
  if ( this->lastDateColumn().size() > 0 )
    query << ", " + this->lastDateColumn();
    
  query << " FROM " << this->tablename();
  
  std::string whereClause ( this->_whereClause() );
  
  if ( whereClause.size() > 0 )
    query << " WHERE " << whereClause;
  
  return query.str();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the where clause.
//
///////////////////////////////////////////////////////////////////////////////

std::string Layer::_whereClause() const
{
  USUL_TRACE_SCOPE;
  
  std::ostringstream whereClause;
  
  if ( this->firstDateColumn().size() > 0 )
  {
    whereClause << this->firstDateColumn() << " >= '" << _minDate.toString() << "'";
    whereClause << " AND " << this->firstDateColumn() << " <= '" << _maxDate.toString() << "'";
  }
  
  if ( this->lastDateColumn().size() > 0 )
  {
    if ( whereClause.str().size() > 0 )
      whereClause << " AND ";
    
    whereClause << this->lastDateColumn() << " >= '" << _minDate.toString() << "'";
    whereClause << " AND " << this->lastDateColumn() << " <= '" << _maxDate.toString() << "'";
  }
  
  return whereClause.str();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Set the primary key column.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::primaryKeyColumn( const std::string& value )
{
  Guard guard ( this->mutex() );
  _primaryKeyColumn = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the primary key column.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& Layer::primaryKeyColumn() const
{
  Guard guard ( this->mutex() );
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
  Guard guard ( this->mutex() );
  _customQuery = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the custom query flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Layer::customQuery() const
{
  Guard guard ( this->mutex() );
  return _customQuery;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set show count in legend.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::showCountLegend( bool b )
{
  Guard guard ( this->mutex() );
  _legendFlags = Usul::Bits::set( _legendFlags, COUNT, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get show count in legend.
//
///////////////////////////////////////////////////////////////////////////////

bool Layer::showCountLegend() const
{
  Guard guard ( this->mutex() );
  return Usul::Bits::has( _legendFlags, COUNT );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set show min in legend.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::showMinLegend( bool b )
{
  Guard guard ( this->mutex() );
  _legendFlags = Usul::Bits::set ( _legendFlags, MIN, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get show min in legend.
//
///////////////////////////////////////////////////////////////////////////////

bool Layer::showMinLegend() const
{
  Guard guard ( this->mutex() );
  return Usul::Bits::has( _legendFlags, MIN );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set show max in legend.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::showMaxLegend( bool b )
{
  Guard guard ( this->mutex() );
  _legendFlags = Usul::Bits::set ( _legendFlags, MAX, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get show max in legend.
//
///////////////////////////////////////////////////////////////////////////////

bool Layer::showMaxLegend() const
{
  Guard guard ( this->mutex() );
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
  case Minerva::Interfaces::IVectorLayer::IID:
    return static_cast < Minerva::Interfaces::IVectorLayer* > ( this );
  case Usul::Interfaces::IAddRowLegend::IID:
    return static_cast < Usul::Interfaces::IAddRowLegend* > ( this );
  case Usul::Interfaces::IClonable::IID:
    return static_cast < Usul::Interfaces::IClonable* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the vector data.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::buildVectorData ( Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress )
{
  // Help shorten lines.
  namespace UA = Usul::Adaptors;
  
  // Scope the reading flag.
  Usul::Scope::Caller::RefPtr scope ( Usul::Scope::makeCaller ( UA::bind1 ( true,  UA::memberFunction ( this, &Layer::updating ) ), 
                                                                UA::bind1 ( false, UA::memberFunction ( this, &Layer::updating ) ) ) );
  
  // Clear what we have.
  this->clear();

  // Set the query.
  if( true == _query.empty() && false == this->customQuery() )
    _query = this->defaultQuery();

  // Query for a status bar.
  Usul::Interfaces::IStatusBar::UpdateStatusBar status ( progress );

  // Give feedback.
  status ( "Building " + this->name() );

  // Build the data objects.
  this->_buildDataObjects( caller, progress );
  
  // Our data is no longer dirty.
  this->dirtyData ( false );
  
  // Our scene needs rebuilt.
  this->dirtyScene ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Modify the vector data.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::modifyVectorData ( Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress )
{
  // For now clear what we have and then rebuild.
  // Need a way to tell if the query has changed.  Then I think this can be handled better.
  this->clear();
  this->buildVectorData ( caller, 0x0 );
  
  // Our data is no longer dirty.
  this->dirtyData ( false );
  
  // Our scene needs rebuilt.
  this->dirtyScene ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  // See if our data is dirty.
  if ( true == this->dirtyData() && false == this->isUpdating() )
  {
    // Get the progress bar factory.
    Usul::Interfaces::IProgressBarFactory::QueryPtr factory ( Usul::Resources::Manager::instance().find ( Usul::Resources::Contants::PROGRESS_BAR_FACTORY ) );

    // Make the progress bar.
    Usul::Interfaces::IUnknown::QueryPtr progress ( factory.valid() ? factory->createProgressBar() : 0x0 );

    // Create a job to update the file.
    Usul::Jobs::Job::RefPtr job ( Usul::Jobs::create (  Usul::Adaptors::bind2 ( caller, progress.get(),
                                                                               Usul::Adaptors::memberFunction ( this, &Layer::modifyVectorData ) ) ) );

    if ( true == job.valid() )
    {
      // Set the job's progress bar so the progress bar doesn't get deleted before job executes.
      job->progress ( progress.get() );

      // Set the updating flag now so we don't launch another job before this one starts.
      this->updating ( true );

      // Add job to manager.
      Usul::Jobs::Manager::instance().addJob ( job.get() );
    }
  }

  BaseClass::updateNotify ( caller );
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
        unsigned int index ( row->addText ( new OsgTools::Legend::Text ( Usul::Strings::format( this->number() ) ) ) );
	      row->at ( index )->alignment ( OsgTools::Legend::Text::RIGHT );
        row->percentage( index ) = 0.20;
      }

      if( this->showMinLegend() )
      {
        double min ( _minMax.first );
        bool valid ( min != std::numeric_limits< double >::max() && !Usul::Math::nan( min ) );
        
        std::string text ( valid ? Usul::Strings::format( min ) : "NA" );
        
        row->addText ( new OsgTools::Legend::Text ( text ) );
      }

      if( this->showMaxLegend() )
      {
        double max ( _minMax.second );
        bool valid ( max != std::numeric_limits< double >::min() && !Usul::Math::nan( max ) );
        
        std::string text ( valid ? Usul::Strings::format( max ) : "NA" );
        
        row->addText ( new OsgTools::Legend::Text ( text ) );
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


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the box.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  void parseBox ( const std::string& text, Usul::Math::Vec2d& lowerLeft, Usul::Math::Vec2d& upperRight )
  {
    std::string box ( text );
    
    boost::algorithm::erase_first ( box, "BOX(" );
    boost::algorithm::erase_first ( box, ")" );
    
    std::vector<std::string> split;
    Usul::Strings::split ( box, ",", false, split );
    
    if ( 2 == split.size() )
    {
      lowerLeft  = Usul::Convert::Type<std::string, Usul::Math::Vec2d>::convert ( split[0] );
      upperRight = Usul::Convert::Type<std::string, Usul::Math::Vec2d>::convert ( split[1] );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the extents.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::_calculateExtents ( Usul::Math::Vec2d& lowerLeft, Usul::Math::Vec2d& upperRight ) const
{
  // Get the connection.
  Connection::RefPtr connection ( const_cast <Connection*> ( this->connection() ) );
  
  if ( false == connection.valid() )
    return;
  
  // Tablename.
  std::string tablename ( this->tablename() );
  
  // Build the query.
  std::string query ( Usul::Strings::format ( "SELECT box2d(geom) FROM ", tablename ) );
  
  // Execute the query.
  pqxx::result result ( connection->executeQuery ( query ) );
  
  // Return if we didn't get any results.
  if ( result.empty() )
    return;
  
  lowerLeft.set  ( std::numeric_limits<double>::max(), std::numeric_limits<double>::max() );
  upperRight.set ( std::numeric_limits<double>::min(), std::numeric_limits<double>::min() );
  
  // Loop through the results.
  for ( pqxx::result::const_iterator iter = result.begin(); iter != result.end(); ++iter )
  {
    Usul::Math::Vec2d ll, ur;
    
    Detail::parseBox ( (*iter)[0].as<std::string>(), ll, ur );
    
    if ( ll[0] < lowerLeft[0] )
      lowerLeft[0] = ll[0];
    if ( ll[1] < lowerLeft[1] )
      lowerLeft[1] = ll[1];
    
    if ( ur[0] > upperRight[0] )
      upperRight[0] = ur[0];
    if ( ur[1] > upperRight[1] )
      upperRight[1] = ur[1];
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  PostGIS metadata tables.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  const std::string GEOMETRY_COLUMNS ( "geometry_columns" );
  const std::string SPATIAL_REF_SYS  ( "spatial_ref_sys" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Split the text into a schema and table.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail 
{
  void split ( const std::string& text, std::string& schema, std::string& table )
  {
    // See if there is a schema specified.
    std::string::size_type pos ( text.find_first_of ( '.' ) );
    
    // Trim the table if we should.
    if( std::string::npos != pos )
    {
      schema = text.substr ( 0, pos );
      table = text.substr ( pos + 1, text.length() - pos );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the Spatial Reference id (srid).
//
///////////////////////////////////////////////////////////////////////////////

int Layer::srid() const
{
  // Get the connection.
  Connection::RefPtr connection ( const_cast <Connection*> ( this->connection() ) );
  
  if ( false == connection.valid() )
    return -1;
  
  // Scope the connection.
  Connection::ScopedConnection scoped ( *connection );
  
  // Get the schema and table name.
  std::string schema ( "public" ), table ( this->tablename() );
  Detail::split ( this->tablename(), schema, table );
  
  // Build the query.
  std::string query ( Usul::Strings::format ( "SELECT srid FROM ", Detail::GEOMETRY_COLUMNS, " WHERE f_table_name='", table, "' AND f_table_schema='", schema, "'" ) );
  
  // Execute the query.
  pqxx::result result ( connection->executeQuery ( query ) );
  
  // Return if we didn't get any results.
  if ( result.empty() )
    return -1;
  
  // Return the result.
  return result[0]["srid"].as < int > ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the projection as "Well Known Text".
//
///////////////////////////////////////////////////////////////////////////////

std::string Layer::projectionWKT() const
{
  // Get the srid.
  int srid ( this->srid() );
  
  return this->projectionWKT ( srid );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the projection as "Well Known Text".
//
///////////////////////////////////////////////////////////////////////////////

std::string Layer::projectionWKT( int srid ) const
{
  // Get the connection.
  Connection::RefPtr connection ( const_cast <Connection*> ( this->connection() ) );
  
  if ( false == connection.valid() )
    return std::string();

  // Scope the connection.
  Connection::ScopedConnection scoped ( *connection );
  
  // Get the tablename.
  std::string tablename ( this->tablename() );
  
  // Build the query.
  std::string query ( Usul::Strings::format ( "SELECT srtext FROM ", Detail::SPATIAL_REF_SYS, " WHERE srid=", srid ) );
  
  // Execute the query.
  pqxx::result result ( connection->executeQuery ( query ) );
  
  // Return if we didn't get any results.
  if ( result.empty() )
    return std::string();
  
  // Return the result.
  return result[0]["srtext"].as < std::string > ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the updating state.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::updating( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _updating = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the updating state.
//
///////////////////////////////////////////////////////////////////////////////

bool Layer::isUpdating() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _updating;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the data objects.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::_buildDataObjects( Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *p )
{
  USUL_TRACE_SCOPE;
  
  // Throw now if there is no connection.
  if ( 0x0 == this->connection () )
    throw std::runtime_error ( "Error 1770160824: A valid connection is needed to build data objects." );
  
  // Make a scoped connection.
  Connection::ScopedConnection scopedConnection ( *this->connection() );
  
  // Execute the query.
  pqxx::result geometryResult ( this->connection()->executeQuery ( this->query() ) );
  
  // Query for the progress bar.
  Usul::Interfaces::IProgressBar::QueryPtr progress ( p );
  if( progress.valid() )
    progress->setTotalProgressBar( geometryResult.size() );
  
  // The data table.
  std::string dataTable ( this->tablename() );
  
  // Loop through the results.
  for ( pqxx::result::const_iterator iter = geometryResult.begin(); iter != geometryResult.end(); ++ iter )
  {
    try
    {
      // Get the id.
      const int id ( iter["id"].as < int > () );
      const int srid ( iter["srid"].as < int > () );
      
      pqxx::binarystring buffer ( iter["geom"] );
      BinaryParser parser;
      BinaryParser::Geometries geometries ( parser ( &buffer.front() ) );
      
      Minerva::Core::DataObjects::DataObject::RefPtr data ( new Minerva::Core::DataObjects::DataObject );
      data->objectId ( Usul::Strings::format ( id ) );
      
      // Set date parameters.
      const std::string  firstDateColumn ( this->firstDateColumn () );
      const std::string  lastDateColumn ( this->lastDateColumn () );

      // Get first and last date if we have valid columns for them.
      if ( false == firstDateColumn.empty() && false == lastDateColumn.empty() )
      {
        // Get first and last date.
        std::string firstDate ( iter[ firstDateColumn ].as < std::string > () );
        std::string lastDate  ( iter[ lastDateColumn  ].as < std::string > () );
       
        // Update min max.
        this->_updateMinMaxDate( firstDate, lastDate );
        
        // Increment last day so animation works properly.
        DataObject::Date last ( lastDate ); 
        last.increment();
       
        data->firstDate ( DataObject::Date ( firstDate ) );
        data->lastDate  ( last );
      }
      
      for ( BinaryParser::Geometries::iterator geom = geometries.begin(); geom != geometries.end(); ++geom )
      {
        Minerva::Core::Geometry::Geometry::RefPtr geometry ( *geom );
        
        // Set the geometry's data.
        geometry->srid( srid );
        geometry->spatialOffset( osg::Vec3f ( this->xOffset(), this->yOffset(), this->zOffset() ) );
        geometry->color( this->_color ( iter ) );
        geometry->renderBin ( this->renderBin() );
        
        // Set primitive specific data members.
        this->_setGeometryMembers( geometry, iter );
        
        // Add the geometry to the data object.
        data->addGeometry( geometry );
      }
      
      /// Set the common members.
      this->_setDataObjectMembers( data.get(), caller );
      
      // Pre build the scene.
      data->preBuildScene( caller );
      
      this->add ( Usul::Interfaces::IUnknown::QueryPtr ( data.get() ) );
    }
    catch ( const std::exception& e )
    {
      std::cout << "Error 6442903120: " << e.what() << std::endl;
    }
    catch ( ... )
    {
      std::cout << "Error 1112177078: Exception caught while adding data to layer." << std::endl;
    }
    
    if( progress.valid() )
    {
      unsigned int num ( iter - geometryResult.begin() );
      progress->updateProgressBar( num );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the date column.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::firstDateColumn( const std::string& dateColumn )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _firstDateColumn = dateColumn;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the date column.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& Layer::firstDateColumn() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _firstDateColumn;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the date column.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::lastDateColumn( const std::string& dateColumn )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _lastDateColumn = dateColumn;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the date column.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& Layer::lastDateColumn() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _lastDateColumn;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the min date.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::minDate( const Date& date )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _minDate = date;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the min date.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::minDate( unsigned int day, unsigned int month, unsigned int year )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  std::ostringstream os;
  os << year << "/" << month << "/" << day;
  _minDate = Date( os.str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the min date.
//
///////////////////////////////////////////////////////////////////////////////

const Layer::Date& Layer::minDate() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _minDate;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the max date.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::maxDate ( const Date& date )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _maxDate = date;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the max date.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::maxDate (unsigned int day, unsigned int month, unsigned int year )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  std::ostringstream os;
  os << year << "/" << month << "/" << day;
  _maxDate = Date( os.str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the max date.
//
///////////////////////////////////////////////////////////////////////////////

const Layer::Date& Layer::maxDate() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _maxDate;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update min max dates.
//
///////////////////////////////////////////////////////////////////////////////

void Layer::_updateMinMaxDate ( const std::string& min, const std::string& max )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  Date d0 ( min );
  Date d1 ( max );
  
  if( d0 < _minDate )
    _minDate = d0;
  
  if( d1 > _maxDate )
    _maxDate = d1;
}
