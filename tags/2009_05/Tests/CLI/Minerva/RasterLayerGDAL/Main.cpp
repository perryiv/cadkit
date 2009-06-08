
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Program to test creating a texture for a tile using RasterLayerGDAL.
//  TODO: Need to add command line options.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/GDAL/RasterLayerGDAL.h"

#include "osgDB/WriteFile"

int main ( int argc, char** argv )
{
  if ( argc < 2 )
  {
    std::cout << "Must provide an input file." << std::endl;
    return -1;
  }

  // Make the layer.
  Minerva::Layers::GDAL::RasterLayerGDAL::RefPtr layer ( new Minerva::Layers::GDAL::RasterLayerGDAL );
  layer->read ( argv[1] );

  // Get the extents of the layer.
  typedef Minerva::Layers::GDAL::RasterLayerGDAL::Extents Extents;
  Extents e ( layer->extents() );

  Extents::Vertex c ( e.center() );
  Extents::ValueType quarterX ( ( e.maximum()[0] - e.minimum()[0] ) / 4.0 );
  Extents::ValueType quarterY ( ( e.maximum()[1] - e.minimum()[1] ) / 4.0 );

  Extents request ( c[0] - quarterX, c[1] - quarterY, c[0] + quarterX, c[1] + quarterY );
  std::cout << request.minimum()[0] << " " << request.minimum()[1] << "" << request.maximum()[0] << " " << request.maximum()[1] << std::endl;

  osg::ref_ptr<osg::Image> result ( layer->texture ( request, 17, 17, 0, 0x0, 0x0 ) );

  osgDB::writeImageFile ( *result, "out.tif" );

  return 0;
}
