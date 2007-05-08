
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "KmlWriter.h"

#include "Minerva/Core/DataObjects/Point.h"
#include "Minerva/Core/DataObjects/Line.h"
#include "Minerva/Core/DataObjects/Polygon.h"

#include "Usul/Functions/Guid.h"
#include "Usul/Interfaces/IVectorLayer.h"
#include "Usul/Interfaces/IOssimPlanetLayer.h"
#include "Usul/Interfaces/IDataObjects.h"
#include "Usul/Interfaces/IPointData.h"
#include "Usul/Interfaces/IOffset.h"

#include <fstream>

using namespace Minerva::Document;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

KmlWriter::KmlWriter( const std::string& filename, Unknown* caller, const MinervaDocument* doc ) :
_filename ( filename ),
_caller ( caller ),
_document ( const_cast < MinervaDocument* > ( doc ) )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

KmlWriter::~KmlWriter()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the file.
//
///////////////////////////////////////////////////////////////////////////////

void KmlWriter::operator() ()
{
  std::ofstream out ( _filename.c_str(), std::fstream::binary | std::fstream::out );

  if( false == out.is_open() )
    return;

  typedef MinervaDocument::Layers Layers;
  Layers& layers ( _document->layers() );

  this->_writeHeader( out );

  for( Layers::iterator iter = layers.begin(); iter != layers.end(); ++iter )
  {
    this->_writeLayer( out, (*iter).get() );
  }

  this->_writeFooter( out );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the header.
//
///////////////////////////////////////////////////////////////////////////////

void KmlWriter::_writeHeader( std::ofstream& out ) const
{
  out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
  out << "<kml xmlns=\"http://earth.google.com/kml/2.1\">" << std::endl;

  // Start of the folder.
  out << "\t<Folder id =\"" << Usul::Functions::GUID::generate() << "\">" << std::endl;

  // Write the name.
  out << "\t\t<name>" << _document->fileName() << "</name>" << std::endl;

  // Write the visibility.
  out << "\t\t<visibility>1</visibility>" << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the footer.
//
///////////////////////////////////////////////////////////////////////////////

void KmlWriter::_writeFooter( std::ofstream& out ) const
{
  // End of the folder.
  out << "\t</Folder>" << std::endl;

  out << "</kml>" << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write a layer.
//
///////////////////////////////////////////////////////////////////////////////

void KmlWriter::_writeLayer ( std::ofstream& out, Usul::Interfaces::ILayer* layer ) const
{
  Usul::Interfaces::IOssimPlanetLayer::QueryPtr image ( layer );
  Usul::Interfaces::IVectorLayer::QueryPtr      vector ( layer );

  // Start of the folder.
  out << "\t<Folder id =\"" << layer->guid() << "\">" << std::endl;

  // Write the name.
  out << "\t\t<name>" << layer->name() << "</name>" << std::endl;

  // Write the visibility.
  out << "\t\t<visibility>" << layer->showLayer() << "</visibility>" << std::endl;

  // Write an image layer.
  if( image.valid() )
    this->_writeImageLayer( out, layer );

  // Write a vector layer.
  if( vector.valid() )
    this->_writeVectorLayer( out, layer );

  // End of the folder.
  out << "\t</Folder>" << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write an image layer.
//
///////////////////////////////////////////////////////////////////////////////

void KmlWriter::_writeImageLayer     ( std::ofstream& out, Usul::Interfaces::ILayer* layer ) const
{
  // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write a vector layer.
//
///////////////////////////////////////////////////////////////////////////////

void KmlWriter::_writeVectorLayer    ( std::ofstream& out, Usul::Interfaces::ILayer* layer ) const
{
  Usul::Interfaces::IDataObjects::QueryPtr dataObjects ( layer );
  if( dataObjects.valid() )
  {
    unsigned int size ( dataObjects->numberDataObjects () );
    for ( unsigned int i = 0; i < size; ++i )
    {
      this->_writeDataObject( out, dataObjects->dataObject( i ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write a point.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void writePoint ( std::ofstream& out, Minerva::Core::DataObjects::Point * point )
  {
    Usul::Interfaces::IPointData::QueryPtr data ( point->geometry() );

    if( data.valid() )
    {
      Usul::Math::Vec3d p ( data->pointData() );

      /// Reset the offset if there is one.  This will negate any effect from terrain.
      Usul::Interfaces::IOffset::QueryPtr offset ( data );
      if( offset.valid() )
        p[2] = offset->spatialOffset()[2];

      out << "\t\t\t\t<Point>" << std::endl;
      out << "\t\t\t\t\t<altitudeMode>relativeToGround</altitudeMode>" << std::endl;
      out << "\t\t\t\t\t<coordinates>" << p[0] << "," << p[1] << "," << p[2] << "</coordinates>" << std::endl;
      out << "\t\t\t\t</Point>" << std::endl;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write a Line.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void writeLine ( std::ofstream& out, Minerva::Core::DataObjects::Line * line )
  {
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write a polygon.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void writePolygon ( std::ofstream& out, Minerva::Core::DataObjects::Polygon * polygon )
  {
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write a data object.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void writeDataObject ( std::ofstream& out, Minerva::Core::DataObjects::DataObject * dataObject )
  {
    typedef Minerva::Core::DataObjects::Point    Point;
    typedef Minerva::Core::DataObjects::Line     Line;
    typedef Minerva::Core::DataObjects::Polygon  Polygon;

    // Is it a polygon?  Because of the inheritance tree, polygon should be first.
    if( Polygon *polygon = dynamic_cast < Polygon* > ( dataObject ) )
      writePolygon( out, polygon );

      // Is it a line?
    else if( Line *line = dynamic_cast < Line* > ( dataObject ) )
      writeLine( out, line );

    // Is it a point?
    else if( Point *point = dynamic_cast < Point* > ( dataObject ) )
      writePoint( out, point );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write a data object.
//
///////////////////////////////////////////////////////////////////////////////

void KmlWriter::_writeDataObject ( std::ofstream& out, Minerva::Core::DataObjects::DataObject * dataObject ) const
{
  // Start of the the place mark.
  out << "\t\t\t<Placemark>" << std::endl;

  /// Write the geometry.
  Helper::writeDataObject( out, dataObject );

  // End of the the place mark.
  out << "\t\t\t</Placemark>" << std::endl;
}
