
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

#include "Serialize/XML/RegisterCreator.h"

#include "osg/Group"
#include "osg/MatrixTransform"

using namespace Minerva::Core::Layers;

SERIALIZE_XML_DECLARE_TYPE_WRAPPER( OsgTools::Animate::Date );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PointTimeLayer::PointTimeLayer() : BaseClass(),
_primitiveID ( 2 ),
_size ( 5.0 ),
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

Layer* PointTimeLayer::clone() const
{
  return new PointTimeLayer( *this );
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

  this->legendObject()->icon ( this->colorFunctor()->icon() );

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
//  Set data members from given layer.
//
///////////////////////////////////////////////////////////////////////////////

void PointTimeLayer::setDataMembers ( Layer * layer )
{
  BaseClass::setDataMembers ( layer );

  if( PointTimeLayer *pt = dynamic_cast < PointTimeLayer * > ( layer ) )
  {
    this->_primitiveID = pt->_primitiveID;
    this->_size = pt->_size;
    this->_firstDateColumn = pt->_firstDateColumn;
    this->_lastDateColumn = pt->_lastDateColumn;
    this->_minDate = pt->_minDate;
    this->_maxDate = pt->_maxDate;
  }
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

