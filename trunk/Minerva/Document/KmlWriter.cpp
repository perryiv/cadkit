
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "KmlWriter.h"

//#include "Minerva/Core/DataObjects/Point.h"
//#include "Minerva/Core/DataObjects/Line.h"
//#include "Minerva/Core/DataObjects/Polygon.h"

#include "Usul/Functions/GUID.h"

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
  Visitor::RefPtr visitor ( new Visitor ( _filename ) );

  _document->accept ( *visitor );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

KmlWriter::Visitor::Visitor ( const std::string& filename ) : BaseClass (),
_filename ( filename ),
_out ( filename.c_str () )
{
  _out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
  _out << "<kml xmlns=\"http://earth.google.com/kml/2.1\">" << std::endl;

  // Start of the folder.
  _out << "\t<Folder id =\"" << Usul::Functions::GUID::generate() << "\">" << std::endl;

  // Write the name.
  _out << "\t\t<name>" << filename << "</name>" << std::endl;

  // Write the visibility.
  _out << "\t\t<visibility>1</visibility>" << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

KmlWriter::Visitor::~Visitor ()
{
  // End of the folder.
  _out << "\t</Folder>" << std::endl;

  _out << "</kml>" << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write out a line.
//
///////////////////////////////////////////////////////////////////////////////
#if 0 
void KmlWriter::Visitor::visit ( Minerva::Core::DataObjects::Line &line )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write out a point.
//
///////////////////////////////////////////////////////////////////////////////

void KmlWriter::Visitor::visit ( Minerva::Core::DataObjects::Point &point )
{
#if 0
  // Start of the the place mark.
  _out << "\t\t\t<Placemark>" << std::endl;

  /// Write the geometry.
  osg::Vec3 p ( point.center () );

  /// Reset the offset if there is one.  This will negate any effect from terrain.
  /*Usul::Interfaces::IOffset::QueryPtr offset ( data );
  if( offset.valid() )
    p[2] = offset->spatialOffset()[2];*/

  _out << "\t\t\t\t<Point>" << std::endl;
  _out << "\t\t\t\t\t<altitudeMode>relativeToGround</altitudeMode>" << std::endl;
  _out << "\t\t\t\t\t<coordinates>" << p[0] << "," << p[1] << "," << p[2] << "</coordinates>" << std::endl;
  _out << "\t\t\t\t</Point>" << std::endl;

  // End of the the place mark.
  _out << "\t\t\t</Placemark>" << std::endl;
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write out a polygon.
//
///////////////////////////////////////////////////////////////////////////////

void KmlWriter::Visitor::visit ( Minerva::Core::DataObjects::Polygon &polygon )
{
}
#endif

///////////////////////////////////////////////////////////////////////////////
//
//  Write out a layer.
//
///////////////////////////////////////////////////////////////////////////////

void KmlWriter::Visitor::visit ( Minerva::Core::Data::Container& layer )
{
  // Start of the folder.
  _out << "\t<Folder id =\"" << layer.guid() << "\">" << std::endl;

  // Write the name.
  _out << "\t\t<name>" << layer.name() << "</name>" << std::endl;

  // Write the visibility.
  _out << "\t\t<visibility>" << layer.showLayer() << "</visibility>" << std::endl;

  // Traverse
  layer.traverse ( *this );

  // End of the folder.
  _out << "\t</Folder>" << std::endl;
}
