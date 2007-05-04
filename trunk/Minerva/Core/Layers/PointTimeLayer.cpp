
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Layers/PointTimeLayer.h"
#include "Minerva/Core/DataObjects/PointTime.h"
#include "Minerva/Core/postGIS/Point.h"

#include "Usul/Interfaces/GUI/IProgressBar.h"

#include "OsgTools/Animate/DateGroup.h"
#include "OsgTools/Animate/DateCallback.h"

#include "osg/Group"
#include "osg/MatrixTransform"

using namespace Minerva::Core::Layers;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS( PointTimeLayer, PointTimeLayer::BaseClass );

SERIALIZE_XML_DECLARE_TYPE_WRAPPER( OsgTools::Animate::Date );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PointTimeLayer::PointTimeLayer() : BaseClass(),
_primitiveID ( 2 ),
_size ( 5.0 ),
_quality ( 0.8 ),
_primitiveSizeColumn ( "" ),
_firstDateColumn(),
_lastDateColumn(),
_minDate( boost::date_time::min_date_time ),
_maxDate( boost::date_time::max_date_time )
{
  this->name( "PointTimeLayer" );

  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PointTimeLayer::PointTimeLayer ( const PointTimeLayer& layer ) : BaseClass ( layer ),
_primitiveID ( layer._primitiveID ),
_size ( layer._size ),
_quality ( layer._quality ),
_primitiveSizeColumn ( layer._primitiveSizeColumn ),
_firstDateColumn( layer._firstDateColumn ),
_lastDateColumn( layer._lastDateColumn ),
_minDate( layer._minDate ),
_maxDate( layer._maxDate )
{
  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register members.
//
///////////////////////////////////////////////////////////////////////////////

void PointTimeLayer::_registerMembers()
{
  SERIALIZE_XML_ADD_MEMBER ( _primitiveID );
  SERIALIZE_XML_ADD_MEMBER ( _size );
  SERIALIZE_XML_ADD_MEMBER ( _quality );
  SERIALIZE_XML_ADD_MEMBER ( _primitiveSizeColumn );
  SERIALIZE_XML_ADD_MEMBER ( _firstDateColumn );
  SERIALIZE_XML_ADD_MEMBER ( _lastDateColumn );
  SERIALIZE_XML_ADD_MEMBER ( _minDate );
  SERIALIZE_XML_ADD_MEMBER ( _maxDate );
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
//  Set the primitive id.
//
///////////////////////////////////////////////////////////////////////////////

void PointTimeLayer::primitiveID( Usul::Types::Uint32 primitiveId )
{
  _primitiveID = primitiveId;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the primitive id.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Types::Uint32 PointTimeLayer::primitiveID() const
{
  return _primitiveID;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

void PointTimeLayer::size( float size )
{
  _size = size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the size.
//
///////////////////////////////////////////////////////////////////////////////

float PointTimeLayer::size() const
{
  return _size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void PointTimeLayer::buildScene( osg::Group* parent )
{
  if ( OsgTools::Animate::DateGroup *dateGroup = dynamic_cast < OsgTools::Animate::DateGroup* > ( parent ) )
  {
    dateGroup->updateMinMax( _minDate, _maxDate );

    // Get the data objects.
    DataObjects &dataObjects ( this->_getDataObjects() );

    for ( DataObjects::iterator iter = dataObjects.begin(); iter != dataObjects.end(); ++iter )
    {
      osg::ref_ptr< osg::Node > node ( (*iter)->buildScene() );

      Minerva::Core::DataObjects::PointTime *pt ( static_cast < Minerva::Core::DataObjects::PointTime* >( iter->get() ) );

      osg::ref_ptr < OsgTools::Animate::DateCallback > cb ( new OsgTools::Animate::DateCallback ( dateGroup->settings(), pt->firstDate(), pt->lastDate() ) );
      node->setCullCallback( cb.get() );
      parent->addChild( node.get() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the data objects.
//
///////////////////////////////////////////////////////////////////////////////

void PointTimeLayer::buildDataObjects( Usul::Interfaces::IUnknown *caller )
{
  // Guard this section of code.
  Guard guard ( _mutex);

  Usul::Interfaces::IProgressBar::QueryPtr progress ( caller );

  const std::string tableName ( this->tablename() );
  
  pqxx::result geometryResult ( this->connection()->executeQuery ( this->query() ) );

  if( progress.valid() )
    progress->setTotalProgressBar( geometryResult.size() );

  for ( pqxx::result::const_iterator i = geometryResult.begin(); i != geometryResult.end(); ++ i )
  {
    try
    {
      if ( false == i["srid"].is_null() || false == i["geom"].is_null() )
      {
        // get date and id.
        std::string firstDate ( i[this->firstDateColumn()].as < std::string > () );
        std::string lastDate ( i[this->lastDateColumn()].as < std::string > () );
        int id ( i["id"].as< int > () );
        int srid ( i["srid"].as< int> () );

        Usul::Interfaces::IUnknown::QueryPtr geometry ( new Minerva::Core::postGIS::Point ( this->connection(), this->tablename(), id, srid, i["geom"] ) );

        Usul::Interfaces::IOffset::QueryPtr offset ( geometry );

        if( offset.valid () )
        {
          offset->spatialOffset( osg::Vec3f ( this->xOffset(), this->yOffset(), this->zOffset() ) );
        }

        if( geometry.valid() )
        {
          Minerva::Core::DataObjects::PointTime::RefPtr data ( new Minerva::Core::DataObjects::PointTime( firstDate, lastDate ) );
          data->geometry( geometry );
          data->color ( this->_color ( i ) );
          data->size ( this->size() );
          data->primitiveId ( this->primitiveID() );
          data->renderBin ( this->renderBin() );
          data->buildScene();
          data->connection ( this->connection() );
          data->tableName ( this->tablename() );
          data->rowId ( id );
          data->quality ( this->quality() );

          if( this->primitiveSizeColumn().size() > 0 )
          {
            float value ( i [ this->primitiveSizeColumn() ].as < float > () );
            data->size( this->size() * value );
          }

          // Also add to the vector of data objects.  This allows for faster updating.
          this->_addDataObject( data.get() );
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
  // Guard this section of code.
  Guard guard ( _mutex);

  // Get the data objects.
  DataObjects &dataObjects ( this->_getDataObjects() );

  // Clear what we have...
  dataObjects.clear();

  // For now rebuild the data objects.
  // In the future need to check if the query has changed, and then go get new data objects.
  // If the query is the same, then just modify the current data objects.
  this->buildDataObjects( caller );
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
  _firstDateColumn = dateColumn;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the date column.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& PointTimeLayer::firstDateColumn() const
{
  return _firstDateColumn;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the date column.
//
///////////////////////////////////////////////////////////////////////////////

void PointTimeLayer::lastDateColumn( const std::string& dateColumn )
{
  _lastDateColumn = dateColumn;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the date column.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& PointTimeLayer::lastDateColumn() const
{
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


/// Get/Set the min date.
void PointTimeLayer::minDate( const OsgTools::Animate::Date& date )
{
  _minDate = date;
}

void PointTimeLayer::minDate( unsigned int day, unsigned int month, unsigned int year )
{
  std::ostringstream os;
  os << year << "/" << month << "/" << day;
  _minDate = OsgTools::Animate::Date( os.str() );
}

const OsgTools::Animate::Date& PointTimeLayer::minDate() const
{
  return _minDate;
}

/// Get/Set the max date.
void PointTimeLayer::maxDate ( const OsgTools::Animate::Date& date )
{
  _maxDate = date;
}

void PointTimeLayer::maxDate (unsigned int day, unsigned int month, unsigned int year )
{
  std::ostringstream os;
  os << year << "/" << month << "/" << day;
  _maxDate = OsgTools::Animate::Date( os.str() );
}

const OsgTools::Animate::Date& PointTimeLayer::maxDate() const
{
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
  case Usul::Interfaces::IPointLayer::IID:
    return static_cast < Usul::Interfaces::IPointLayer* > ( this );
  case Usul::Interfaces::ITemporalData::IID:
    return static_cast < Usul::Interfaces::ITemporalData* > ( this );
  case Minerva::Core::IPointTimeLayerRawPointer::IID:
    return static_cast < Minerva::Core::IPointTimeLayerRawPointer * > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the point time layer.
//
///////////////////////////////////////////////////////////////////////////////

PointTimeLayer* PointTimeLayer::getRawPointer()
{
  return this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the point time layer.
//
///////////////////////////////////////////////////////////////////////////////

const PointTimeLayer* PointTimeLayer::getRawPointer() const
{
  return this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set quality.
//
///////////////////////////////////////////////////////////////////////////////

void PointTimeLayer::quality( float value )
{
  _quality = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get quality.
//
///////////////////////////////////////////////////////////////////////////////

float PointTimeLayer::quality() const
{
  return _quality;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set quality.
//
///////////////////////////////////////////////////////////////////////////////

void PointTimeLayer::primitiveSizeColumn( const std::string& value )
{
  _primitiveSizeColumn = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get quality.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& PointTimeLayer::primitiveSizeColumn() const
{
  return _primitiveSizeColumn;
}
