
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Layers/LineLayer.h"
#include "Minerva/Core/DataObjects/Line.h"
#include "Minerva/Core/postGIS/Line.h"

#include "Serialize/XML/RegisterCreator.h"

#include "Usul/Interfaces/GUI/IProgressBar.h"

using namespace Minerva::Core::Layers;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS( LineLayer, LineLayer::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LineLayer::LineLayer() : BaseClass(),
_lineWidth ( 1.0f )
{
  this->name( "LineLayer" );
  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LineLayer::LineLayer ( const LineLayer& layer ) : BaseClass ( layer ),
_lineWidth ( layer._lineWidth )
{
  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register members.
//
///////////////////////////////////////////////////////////////////////////////

void LineLayer::_registerMembers()
{
  SERIALIZE_XML_ADD_MEMBER ( _lineWidth );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone this layer.
//
///////////////////////////////////////////////////////////////////////////////

Layer* LineLayer::clone() const
{
  return new LineLayer( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

LineLayer::~LineLayer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the line width.
//
///////////////////////////////////////////////////////////////////////////////

void LineLayer::lineWidth( float width )
{
  Guard guard( _mutex );
  _lineWidth = width;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the line width.
//
///////////////////////////////////////////////////////////////////////////////

float LineLayer::lineWidth() const
{
  return _lineWidth;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the data objects.
//
///////////////////////////////////////////////////////////////////////////////

void LineLayer::buildDataObjects( Usul::Interfaces::IUnknown *caller )
{
  // Lock the mutex.
  Guard guard( _mutex );

  // Query for a progress bar.
  Usul::Interfaces::IProgressBar::QueryPtr progress ( caller );

  // Execute the query.
  pqxx::result geometryResult ( this->connection()->executeQuery ( this->query() ) );

  // Set the total number of items.
  if( progress.valid() )
    progress->setTotalProgressBar( geometryResult.size() );

  // The data table.
  std::string dataTable ( this->tablename() );

  // Loop through the results.
  for ( pqxx::result::const_iterator iter = geometryResult.begin(); iter != geometryResult.end(); ++ iter )
  {
    // Get the id.
    int id ( iter["id"].as < int > () );
    int srid ( iter["srid"].as < int > () );

    Usul::Interfaces::IUnknown::QueryPtr geometry ( new Minerva::Core::postGIS::Line ( this->connection(), dataTable, id, srid, iter["geom"] ) );

    Usul::Interfaces::IOffset::QueryPtr offset ( geometry );

    if( offset.valid() )
      offset->spatialOffset( osg::Vec3f ( 0.0, 0.0, this->zOffset() ) );

    Minerva::Core::DataObjects::Line::RefPtr data ( new Minerva::Core::DataObjects::Line( ) );
    data->color ( this->_color ( iter ) );
    data->width( _lineWidth );
    data->geometry( geometry );
    data->renderBin( this->renderBin() );
    data->tableName( dataTable );
    data->rowId( id );

    /// Set the label.
    this->_labelDataObject( data.get() );

    data->buildScene();

    // Add the data object.
    this->_addDataObject( data.get() );

    // Update progress.
    if( progress.valid() )
    {
      unsigned int num ( iter - geometryResult.begin() );
      progress->updateProgressBar( num );
    }
  }

  // Update the legend.
  this->_updateLegendObject();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Modify layer.
//
///////////////////////////////////////////////////////////////////////////////

void LineLayer::modify( Usul::Interfaces::IUnknown *caller )
{
  // For now get what we have, clear and then rebuild.
  // Need a way to tell if the query has changed.  Then I think this can be handled better.
  DataObjects &dataObjects ( this->_getDataObjects() );
  dataObjects.clear();

  this->buildDataObjects ( caller );

  //// Lock the mutex.
  //Guard guard( _mutex );

  //this->legendObject()->icon ( this->colorFunctor()->icon() );

  //// Query for the progress bar.
  //Usul::Interfaces::IProgressBar::QueryPtr progress ( caller );

  ///// Get the data objects.
  //DataObjects &dataObjects ( this->_getDataObjects() );

  //if( progress.valid() )
  //  progress->setTotalProgressBar( dataObjects.size() );

  //for ( DataObjects::iterator iter = dataObjects.begin(); iter != dataObjects.end(); ++iter )
  //{
  //  Minerva::DataObjects::Line::RefPtr data ( dynamic_cast < Minerva::DataObjects::Line* > ( iter->get() ) );
  //  data->color ( (*this->colorFunctor()) ( 0.0 ) );
  //  data->width( _lineWidth );
  //  data->renderBin( this->renderBin() );
  //  data->geometry()->zOffset( this->zOffset() );

  //  // Clear the label.
  //  data->label( "" );

  //  // If we have a column to use for a label.
  //  if( this->showLabel() && !this->labelColumn().empty() )
  //  {
  //    std::string value ( this->connection()->getColumnDataString( this->tablename(), data->rowId(), this->labelColumn() ) );

  //    data->label( value );
  //    data->labelColor( this->labelColor() );

  //    osg::Vec3 center ( data->geometry()->getCenter( 0.0, 0.0, this->labelZOffset() ) );
  //    data->labelPosition( center );
  //  }

  //  // Update progress
  //  if( progress.valid() )
  //  {
  //    unsigned int num ( iter - dataObjects.begin() );
  //    progress->updateProgressBar( num );
  //  }
  //}
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* LineLayer::queryInterface( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::ILineLayer::IID:
    return static_cast < Usul::Interfaces::ILineLayer* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}
