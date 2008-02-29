
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/PostGIS/PointTimeLayer.h"
#include "Minerva/Layers/PostGIS/BinaryParser.h"

#include "Minerva/Core/DataObjects/Point.h"
#include "Minerva/Core/Visitor.h"

#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Factory/RegisterCreator.h"

#include "osg/Group"
#include "osg/MatrixTransform"

#include "pqxx/pqxx"

using namespace Minerva::Layers::PostGIS;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS( PointTimeLayer, PointTimeLayer::BaseClass );
USUL_FACTORY_REGISTER_CREATOR ( PointTimeLayer );

SERIALIZE_XML_DECLARE_TYPE_WRAPPER( Minerva::Core::Animate::Date );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PointTimeLayer::PointTimeLayer() : 
  BaseClass(),
  _firstDateColumn(),
  _lastDateColumn(),
  _minDate( boost::date_time::min_date_time ),
  _maxDate( boost::date_time::max_date_time )
{
  USUL_TRACE_SCOPE;

  this->name( "PointTimeLayer" );

  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PointTimeLayer::PointTimeLayer ( const PointTimeLayer& layer ) : 
  BaseClass ( layer ),
  _firstDateColumn( layer._firstDateColumn ),
  _lastDateColumn( layer._lastDateColumn ),
  _minDate( layer._minDate ),
  _maxDate( layer._maxDate )
{
  USUL_TRACE_SCOPE;

  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register members.
//
///////////////////////////////////////////////////////////////////////////////

void PointTimeLayer::_registerMembers()
{
  BaseClass::_registerMembers();
  SERIALIZE_XML_ADD_MEMBER ( _firstDateColumn );
  SERIALIZE_XML_ADD_MEMBER ( _lastDateColumn );
  //SERIALIZE_XML_ADD_MEMBER ( _minDate );
  //SERIALIZE_XML_ADD_MEMBER ( _maxDate );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone this layer.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* PointTimeLayer::clone() const
{
  Usul::Interfaces::IUnknown::QueryPtr copy ( new PointTimeLayer( *this ) );
  return copy.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

PointTimeLayer::~PointTimeLayer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accept the visitor.
//
///////////////////////////////////////////////////////////////////////////////

void PointTimeLayer::accept ( Minerva::Core::Visitor& visitor )
{
  visitor.visit ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the data objects.
//
///////////////////////////////////////////////////////////////////////////////

void PointTimeLayer::buildDataObjects( Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *p )
{
  USUL_TRACE_SCOPE;

  // Throw now if there is no connection.
  if ( 0x0 == this->connection () )
    throw std::runtime_error ( "Error 1770160824: A valid connection is needed to build data objects for point time layer." );

  Connection::ScopedConnection scopedConnection ( *this->connection() );

  Usul::Interfaces::IProgressBar::QueryPtr progress ( p );

  // Make copies of data needed in loop.
  const std::string  tableName ( this->tablename() );
  const std::string  firstDateColumn ( this->firstDateColumn () );
  const std::string  lastDateColumn ( this->lastDateColumn () );
  const std::string  primitiveSizeColumn ( this->primitiveSizeColumn () );
  
  const osg::Vec3    offset ( this->xOffset(), this->yOffset(), this->zOffset() );

  const float        size ( this->size () );
  const unsigned int primitiveID ( this->primitiveID () );
  const unsigned int renderBin ( this->renderBin () );
  const float        quality ( this->quality () );
  const bool         autotransform ( this->autotransform () );
  const float        secondarySize ( this->secondarySize () );

  // Execute the query.
  pqxx::result geometryResult ( this->connection()->executeQuery ( this->query() ) );

  if( progress.valid() )
    progress->setTotalProgressBar( geometryResult.size() );

  // Go through out results.
  for ( pqxx::result::const_iterator i = geometryResult.begin(); i != geometryResult.end(); ++ i )
  {
    try
    {
      if ( false == i["srid"].is_null() || false == i["geom"].is_null() )
      {
        // get date and id.
        std::string firstDate ( i[ firstDateColumn ].as < std::string > () );
        std::string lastDate  ( i[ lastDateColumn  ].as < std::string > () );

        // Update min max.
        this->_updateMinMaxDate( firstDate, lastDate );

        int id ( i["id"].as< int > () );
        int srid ( i["srid"].as< int> () );

        pqxx::binarystring buffer ( i["geom"] );
        BinaryParser parser;
        BinaryParser::Geometries geometries ( parser ( &buffer.front() ) );

        for ( BinaryParser::Geometries::iterator geom = geometries.begin(); geom != geometries.end(); ++geom )
        {
          (*geom)->srid( srid );
          Usul::Interfaces::IUnknown::QueryPtr unknown ( *geom );
          Usul::Interfaces::IOffset::QueryPtr so ( unknown );

          if( so.valid () )
          {
            so->spatialOffset( offset );
          }

          if( unknown.valid() )
          {
            Minerva::Core::DataObjects::Point::RefPtr data ( new Minerva::Core::DataObjects::Point );
            data->firstDate ( DataObject::Date ( firstDate ) );
            data->lastDate  ( DataObject::Date ( lastDate ) );
            data->geometry( unknown.get () );
            data->color ( this->_color ( i ) );
            data->size ( size );
            data->primitiveId ( primitiveID );
            data->renderBin ( renderBin );
            data->dataSource ( Usul::Interfaces::IUnknown::QueryPtr ( this->connection() ) );
            data->objectId ( Usul::Strings::format ( id ) );
            data->quality ( quality );
            data->autotransform ( autotransform );
            data->secondarySize ( secondarySize );

            if( primitiveSizeColumn.size() > 0 )
            {
              float value ( i [ primitiveSizeColumn ].as < float > () );
              data->size( size * value );
              this->_updateMinMax( value );
            }

            data->preBuildScene ( caller );

            // Also add to the vector of data objects.  This allows for faster updating.
            this->addDataObject( data.get() );
          }
        }
        if( progress.valid() )
        {
          unsigned int num ( i - geometryResult.begin() );
          progress->updateProgressBar( num );
        }
      }
    }
    catch ( const std::exception &e )
    {
      std::cout << "Error 5412926260: " << e.what() << std::endl;
    }
    catch ( ... )
    {
      std::cout << "Error 4160517991: exception generated while adding point-time layer" << std::endl;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Modify the layer.
//
///////////////////////////////////////////////////////////////////////////////

void PointTimeLayer::modify( Usul::Interfaces::IUnknown *caller )
{
  // Clear what we have...
  this->clearDataObjects();

  // For now rebuild the data objects.
  // In the future need to check if the query has changed, and then go get new data objects.
  // If the query is the same, then just modify the current data objects.
  this->buildDataObjects( caller, 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this layer temporal?
//
///////////////////////////////////////////////////////////////////////////////

bool PointTimeLayer::isTemporal() const
{
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the date column.
//
///////////////////////////////////////////////////////////////////////////////

void PointTimeLayer::firstDateColumn( const std::string& dateColumn )
{
  Guard guard ( this );
  _firstDateColumn = dateColumn;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the date column.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& PointTimeLayer::firstDateColumn() const
{
  Guard guard ( this );
  return _firstDateColumn;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the date column.
//
///////////////////////////////////////////////////////////////////////////////

void PointTimeLayer::lastDateColumn( const std::string& dateColumn )
{
  Guard guard ( this );
  _lastDateColumn = dateColumn;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the date column.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& PointTimeLayer::lastDateColumn() const
{
  Guard guard ( this );
  return _lastDateColumn;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the default query.
//
///////////////////////////////////////////////////////////////////////////////

std::string PointTimeLayer::defaultQuery() const
{
  std::ostringstream query;
  query << "SELECT id, srid(" << this->geometryColumn() << ") as srid, asBinary(" << this->geometryColumn() << ") as geom";

  if ( this->firstDateColumn().size() > 0 )
    query << ", " << this->firstDateColumn();

  if ( this->lastDateColumn().size() > 0 )
    query << ", " + this->lastDateColumn();

  if ( this->colorColumn().size() > 0 )
    query << ", " << this->colorColumn();

  if ( this->primitiveSizeColumn().size() > 0 )
    query << ", " << this->primitiveSizeColumn();
  
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

std::string PointTimeLayer::_whereClause() const
{
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
//  Set the min date.
//
///////////////////////////////////////////////////////////////////////////////

void PointTimeLayer::minDate( const Date& date )
{
  Guard guard ( this );
  _minDate = date;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the min date.
//
///////////////////////////////////////////////////////////////////////////////

void PointTimeLayer::minDate( unsigned int day, unsigned int month, unsigned int year )
{
  std::ostringstream os;
  os << year << "/" << month << "/" << day;
  _minDate = Date( os.str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the min date.
//
///////////////////////////////////////////////////////////////////////////////

const PointTimeLayer::Date& PointTimeLayer::minDate() const
{
  Guard guard ( this );
  return _minDate;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the max date.
//
///////////////////////////////////////////////////////////////////////////////

void PointTimeLayer::maxDate ( const Date& date )
{
  Guard guard ( this );
  _maxDate = date;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the max date.
//
///////////////////////////////////////////////////////////////////////////////

void PointTimeLayer::maxDate (unsigned int day, unsigned int month, unsigned int year )
{
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

const PointTimeLayer::Date& PointTimeLayer::maxDate() const
{
  Guard guard ( this );
  return _maxDate;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* PointTimeLayer::queryInterface( unsigned long iid )
{
  switch ( iid )
  {
  //case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::ITemporalData::IID:
    return static_cast < Usul::Interfaces::ITemporalData* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update min max dates.
//
///////////////////////////////////////////////////////////////////////////////

void PointTimeLayer::_updateMinMaxDate ( const std::string& min, const std::string& max )
{
  Guard guard ( this );

  Date d0 ( min );
  Date d1 ( max );

  if( d0 < _minDate )
    _minDate = d0;

  if( d1 > _maxDate )
    _maxDate = d1;
}
