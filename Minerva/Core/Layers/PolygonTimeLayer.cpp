
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Layers/PolygonTimeLayer.h"
#include "Minerva/Core/DataObjects/Polygon.h"
#include "Minerva/Core/DataObjects/Point.h"
#include "Minerva/Core/postGIS/Point.h"
#include "Minerva/Core/postGIS/Polygon.h"

#include "Usul/Interfaces/GUI/IProgressBar.h"

#include "OsgTools/Animate/DateGroup.h"

#include "osg/MatrixTransform"
#include "osg/Group"

using namespace Minerva::Core::Layers;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PolygonTimeLayer::PolygonTimeLayer() : BaseClass(),
_mode ( DATE ),
_drawMode ( CYLINDER ),
_stepColumn(),
_zScaleFactor( 1.0 ),
_xOffset( 0.0 ),
_yOffset( 0.0 ),
_temporalMap(),
_numberMap()
{
  this->name( "PolygonTimeLayer" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PolygonTimeLayer::PolygonTimeLayer ( const PolygonTimeLayer& layer ) : BaseClass ( layer ),
_mode ( layer._mode ),
_drawMode ( layer._drawMode ),
_stepColumn( layer._stepColumn ),
_zScaleFactor( layer._zScaleFactor ),
_xOffset( layer._xOffset ),
_yOffset( layer._yOffset ),
_temporalMap( layer._temporalMap ),
_numberMap( layer._numberMap )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone this layer.
//
///////////////////////////////////////////////////////////////////////////////

Layer* PolygonTimeLayer::clone() const
{
  return new PolygonTimeLayer( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

PolygonTimeLayer::~PolygonTimeLayer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonTimeLayer::buildScene( osg::Group* parent )
{
  if( _mode == DATE )
    this->_buildSceneDateMode( parent );
  else if ( _mode == NUMBER )
    this->_buildSceneNumberMode ( parent );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonTimeLayer::_buildSceneDateMode   ( osg::Group* parent )
{
  /*OsgTools::Animate::DateGroup *dateGroup = dynamic_cast < OsgTools::Animate::DateGroup* > ( parent );

  for ( TemporalMap::iterator iter = _temporalMap.begin(); iter != _temporalMap.end(); ++iter )
  {
    osg::ref_ptr< osg::Group > group ( new osg::Group );

    for( DataObjects::iterator doIter = iter->second.begin(); doIter != iter->second.end(); ++doIter )
    {
      group->addChild( (*doIter)->buildScene() );
    }

    if( dateGroup )
      dateGroup->addDate( iter->first, group.get() );
    else
      parent->addChild( group.get() );
  }*/
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonTimeLayer::_buildSceneNumberMode ( osg::Group* parent )
{
  for ( NumberMap::iterator iter = _numberMap.begin(); iter != _numberMap.end(); ++iter )
  {
    osg::ref_ptr< osg::MatrixTransform > group ( new osg::MatrixTransform );
    osg::Matrix matrix ( osg::Matrix::translate( this->xOffset(), this->yOffset(), 0.0 ) );
    group->setMatrix( matrix );

    for( DataObjects::iterator doIter = iter->second.begin(); doIter != iter->second.end(); ++doIter )
    {
      group->addChild( (*doIter)->buildScene() );
    }
    parent->addChild( group.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the data objects.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonTimeLayer::buildDataObjects( Usul::Interfaces::IUnknown *caller )
{
  Usul::Interfaces::IProgressBar::QueryPtr progress ( caller );

  this->legendObject()->icon( this->colorFunctor()->icon() );

  std::string dataTable ( this->tablename() );

  // Execute the query.  This will return data to draw.
  pqxx::result geometryResult ( this->connection()->executeQuery ( this->query() ) );

  if( progress.valid() )
    progress->setTotalProgressBar( geometryResult.size() );

  for ( pqxx::result::const_iterator iter = geometryResult.begin(); iter != geometryResult.end(); ++ iter )
  {
    // Id for row
    int id ( iter["id"].as < int > () );
    int srid ( iter["srid"].as < int > () );

    double fieldValue ( 0.0 );

    if( !this->colorColumn().empty() && !iter[this->colorColumn()].is_null() )
    {
      fieldValue = iter[ this->colorColumn() ].as < double > ();
    }

    if ( _drawMode == POLYGON )
    {
      Minerva::Core::postGIS::Geometry::RefPtr geometry ( new Minerva::Core::postGIS::Polygon ( this->connection(), dataTable, id, srid, iter["geom"] ) );

      Minerva::Core::DataObjects::Boundary::RefPtr data ( new Minerva::Core::DataObjects::Boundary );

      data->geometry( geometry.get() );
      data->color( this->_color( iter ) );
      data->renderBin( this->renderBin() );
      data->tableName ( dataTable );
      data->rowId ( id );

      data->buildScene();

      if( _mode == DATE )
      {
        std::string date ( iter[this->stepColumn()].as < std::string > () );
        _temporalMap[OsgTools::Animate::Date ( date )].push_back ( data.get() );
      }
      else if ( _mode == NUMBER )
      {
        int num ( iter[this->stepColumn()].as < int > () );
        _numberMap[num].push_back( data.get() );
      }
    }
    else if ( _drawMode == CYLINDER )
    {
      Minerva::Core::DataObjects::Point::RefPtr data ( new Minerva::Core::DataObjects::Point );

      Minerva::Core::postGIS::Geometry::RefPtr geometry ( new Minerva::Core::postGIS::Point ( this->connection(), dataTable, id, srid, iter["geom"] ) );
      data->geometry( geometry.get() );
      data->size( fieldValue * _zScaleFactor );
      data->primitiveId ( Minerva::Core::DataObjects::Point::CYLINDER );
      data->color( this->_color( iter ) );
      data->renderBin( this->renderBin() );
      data->tableName ( dataTable );
      data->rowId ( id );

      data->buildScene();

      if( _mode == DATE )
      {
        std::string date ( iter[this->stepColumn()].as < std::string > () );
        _temporalMap[OsgTools::Animate::Date ( date )].push_back ( data.get() );
      }
      else if ( _mode == NUMBER )
      {
        int num ( iter[this->stepColumn()].as < int > () );
        _numberMap[num].push_back( data.get() );
      }
    }

    if( progress.valid() )
    {
      unsigned int num ( iter - geometryResult.begin() );
      progress->updateProgressBar( num );
    }
  }
}


void PolygonTimeLayer::modify( Usul::Interfaces::IUnknown *caller )
{
  _temporalMap.clear();
  _numberMap.clear();

  this->buildDataObjects ( caller );
}


/// Get/Set the mode.
void PolygonTimeLayer::mode ( Mode mode )
{
  _mode = mode;
}


PolygonTimeLayer::Mode PolygonTimeLayer::mode() const
{
  return _mode;
}


/// Get/Set Date column name.
void PolygonTimeLayer::stepColumn( const std::string& column )
{
  _stepColumn = column;
}

const std::string& PolygonTimeLayer::stepColumn() const
{
  return _stepColumn;
}


/// Is this layer temporal.
bool PolygonTimeLayer::isTemporal() const
{
  return true;
}

void PolygonTimeLayer::zScaleFactor( float f )
{
  _zScaleFactor = f;
}

float PolygonTimeLayer::zScaleFactor() const
{
  return _zScaleFactor;
}


void PolygonTimeLayer::xOffset( float f )
{
  _xOffset = f;
}

float PolygonTimeLayer::xOffset() const
{
  return _xOffset;
}

void PolygonTimeLayer::yOffset( float f )
{
  _yOffset = f;
}

float PolygonTimeLayer::yOffset() const
{
  return _yOffset;
}


std::string PolygonTimeLayer::defaultQuery() const
{
  std::ostringstream query;
  if ( false != this->labelColumn().empty() )
  {
    query << "SELECT id, " << this->labelColumn() << ", " << this->stepColumn() 
          << ", srid(" << this->geometryColumn() << ") as srid, " 
          << "asBinary(" << this->geometryColumn() << ") as geom " 
          << "FROM " << this->tablename();
  }
  else
    return BaseClass::defaultQuery();

  return query.str();
}
