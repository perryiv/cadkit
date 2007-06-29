
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Layers/PolygonLayer.h"
#include "Minerva/Core/postGIS/Polygon.h"
#include "Minerva/Core/DataObjects/Polygon.h"

#include "Usul/Interfaces/GUI/IProgressBar.h"

#include "Serialize/XML/RegisterCreator.h"

#include "osg/Group"

#include <algorithm>

SERIALIZE_XML_DECLARE_VECTOR_4_WRAPPER ( osg::Vec4 );

using namespace Minerva::Core::Layers;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS( PolygonLayer, PolygonLayer::BaseClass );
SERIALIZE_XML_REGISTER_CREATOR ( PolygonLayer );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PolygonLayer::PolygonLayer() : BaseClass(), 
_format(),
_showInterior( true ),
_showBorder( false ),
_borderColor(),
_borderWidth ( 1.0f )
{
  this->name( "PolygonLayer" );

  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PolygonLayer::PolygonLayer ( const PolygonLayer& layer ) : BaseClass ( layer ),
_format ( layer._format ),
_showInterior( layer._showInterior ),
_showBorder ( layer._showBorder ),
_borderColor ( layer._borderColor ),
_borderWidth ( layer._borderWidth )
{
  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register members.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonLayer::_registerMembers()
{
  SERIALIZE_XML_ADD_MEMBER ( _format );
  SERIALIZE_XML_ADD_MEMBER ( _showInterior );
  SERIALIZE_XML_ADD_MEMBER ( _showBorder );
  SERIALIZE_XML_ADD_MEMBER ( _borderColor );
  SERIALIZE_XML_ADD_MEMBER ( _borderWidth );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone this layer.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* PolygonLayer::clone() const
{
  Usul::Interfaces::IUnknown::QueryPtr copy  ( new PolygonLayer( *this ) );
  return copy.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

PolygonLayer::~PolygonLayer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the data objects.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonLayer::buildDataObjects( Usul::Interfaces::IUnknown *caller )
{
  // Guard this section of code.
  Guard guard ( _mutex);

  Minerva::Core::DB::Connection::ScopedConnection scopedConnection ( *this->connection() );

  Usul::Interfaces::IProgressBar::QueryPtr progress ( caller );

  std::string dataTable ( this->tablename() );

  // Execute the query.  This will return data to draw.
  pqxx::result geometryResult ( this->connection()->executeQuery ( this->query() ) );

  if( progress.valid() )
    progress->setTotalProgressBar( geometryResult.size() );

  for ( pqxx::result::const_iterator iter = geometryResult.begin(); iter != geometryResult.end(); ++ iter )
  {
    try
    {
      // Id for row
      int id ( iter["id"].as < int > () );
      int srid ( iter["srid"].as < int > () );

      // Create the geometry.
      Usul::Interfaces::IUnknown::QueryPtr geometry ( new Minerva::Core::postGIS::Polygon ( this->connection(), dataTable, id, srid, iter["geom"] ) );

      Usul::Interfaces::IOffset::QueryPtr offset ( geometry );

      if( offset.valid() )
        offset->spatialOffset( osg::Vec3f ( 0.0, 0.0, this->zOffset() ) );

      // Create the Data Object.
      Minerva::Core::DataObjects::Polygon::RefPtr data ( new Minerva::Core::DataObjects::Polygon );

      data->width ( this->borderWidth() );
      data->showBorder( this->showBorder() );
      data->showInterior ( this->showInterior() );
      data->geometry ( geometry.get() );
      data->color( this->_color ( iter ) );
      data->renderBin( this->renderBin() );
      data->connection ( this->connection() );
      data->tableName ( dataTable );
      data->rowId ( id );

      /// Set the label.
      this->_labelDataObject( data.get() );

      // Pre build the scene.
      data->preBuildScene();

      this->_addDataObject( data.get() );
    }
    catch ( const std::exception& e )
    {
      std::cout << "Error 3149586695: " << e.what() << std::endl;
    }
    catch ( ... )
    {
      std::cout << "Error 3420474749: Exception caught while adding data to layer." << std::endl;
    }

    // Update the progess.
    if( progress.valid() )
    {
      unsigned int num ( iter - geometryResult.begin() );
      progress->updateProgressBar( num );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Modify the data objects.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonLayer::modify( Usul::Interfaces::IUnknown *caller )
{
  // For now get what we have, clear and then rebuild.
  // Need a way to tell if the query has changed.  Then I think this can be handled better.
  DataObjects &dataObjects ( this->_getDataObjects() );
  dataObjects.clear();

  this->buildDataObjects ( caller );

  // Guard this section of code.
  //Guard guard ( _mutex);

  //this->legendObject()->icon( this->colorFunctor()->icon() );

  //Usul::Interfaces::IProgressBar::QueryPtr progress ( caller );

  //DataObjects &dataObjects ( this->_getDataObjects() );

  //if( progress.valid() )
  //  progress->setTotalProgressBar( dataObjects.size() );

  //for ( DataObjects::iterator iter = dataObjects.begin(); iter != dataObjects.end(); ++iter )
  //{
  //  // Update the render bin.
  //  (*iter)->renderBin( this->renderBin() );

  //  std::ostringstream query;
  //  query << "SELECT " << this->colorColumn() << " FROM " << this->tablename() << " WHERE id=" << (*iter)->rowId();
  //  pqxx::result result ( this->connection()->executeQuery ( query.str() ) );

  //  if( !result.empty() )
  //    (*iter)->color ( this->_color( result.begin() ) );

  //  if( progress.valid() )
  //  {
  //    unsigned int num ( iter - dataObjects.begin() );
  //    progress->updateProgressBar( num );
  //  }
  //}
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the format string.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonLayer::format( const std::string& format )
{
  _format = format;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the format string.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& PolygonLayer::format() const
{
  return _format;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the show border flag.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonLayer::showBorder( bool b )
{
  _showBorder = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the show border flag.
//
///////////////////////////////////////////////////////////////////////////////

bool PolygonLayer::showBorder() const
{
  return _showBorder;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the border color.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonLayer::borderColor( const osg::Vec4& color )
{
  _borderColor = color;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the border color.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec4& PolygonLayer::borderColor() const
{
  return _borderColor;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the show interior flag.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonLayer::showInterior( bool b )
{
  _showInterior = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the show interior flag.
//
///////////////////////////////////////////////////////////////////////////////

bool PolygonLayer::showInterior() const
{
  return _showInterior;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the border width flag.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonLayer::borderWidth( float width )
{
  _borderWidth = width;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the border width flag.
//
///////////////////////////////////////////////////////////////////////////////

float PolygonLayer::borderWidth() const
{
  return _borderWidth;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* PolygonLayer::queryInterface( unsigned long iid )
{
  switch ( iid )
  {
  //case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPolygonLayer::IID:
    return static_cast < Usul::Interfaces::IPolygonLayer* > ( this );
  case Minerva::Core::IPolygonLayerRawPointer::IID:
    return static_cast < Minerva::Core::IPolygonLayerRawPointer* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the raw pointer.
//
///////////////////////////////////////////////////////////////////////////////

PolygonLayer* PolygonLayer::getRawPointer()
{
  return this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the raw pointer.
//
///////////////////////////////////////////////////////////////////////////////

const PolygonLayer* PolygonLayer::getRawPointer() const
{
  return this;
}
