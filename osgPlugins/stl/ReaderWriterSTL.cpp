
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#include "ReaderWriterSTL.h"


#include "osgDB/ReadFile"
#include "osgDB/Registry"
#include "osgDB/FileNameUtils"
#include "osgDB/FileUtils"

#include "osg/ref_ptr"
#include "osg/Geode"
#include "osg/Geometry"

#include <sstream>
#include <iostream>
#include <cassert>

ReaderWriterSTL::ReaderWriterSTL() :
  _facets(),
  _currentFacet(NULL),
  _polygons()
{
}

ReaderWriterSTL::~ReaderWriterSTL()
{
  // This will delete the facets.
  this->_init();
}

bool ReaderWriterSTL::acceptsExtension ( const std::string &ext )
{
  return osgDB::equalCaseInsensitive ( ext, "stl" );
}

const char* ReaderWriterSTL::className()
{
  return "STL Reader";
}

ReaderWriterSTL::Result ReaderWriterSTL::readNode ( const std::string &file, const Options *options )
{
  try
  {
    return this->_read ( file, options );
  }

  // Catch known exceptions.
  catch ( const ReaderWriterSTL::Result &r )
  {
    return r;
  }

  // Catch standard exceptions.
  catch ( const std::exception &e )
  {
    return ReaderWriterSTL::Result ( e.what() );
  }

  // Catch all other exceptions.
  catch ( ... )
  {
    return ReaderWriterSTL::Result ( "Unknown exception caught" );
  }

}

osg::Group * ReaderWriterSTL::_build() const
{
  // The scene root.
  osg::ref_ptr<osg::Group> root ( new osg::Group );

#if 0

  for(unsigned int i = 0; i < _facets.size(); ++i)
  {
    osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
    Facet *facet = _facets.at(i);
    osg::ref_ptr< osg::Geometry > geometry ( facet->getGeometry() );
    geode->addDrawable ( geometry.get() );
    root->addChild ( geode.get() );
  }

#else

  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
  root->addChild ( geode.get() );

  // Make triangles.
  Polygons::const_iterator i = _polygons.find ( 3 );
  if ( _polygons.end() != i )
  {
    const Vertices &v = i->second.first;
    const Normals  &n = i->second.second;
    assert ( n.size() * 3 == v.size() );

    osg::ref_ptr<osg::Vec3Array> vertices ( new osg::Vec3Array ( v.begin(), v.end() ) );
    osg::ref_ptr<osg::Vec3Array> normals  ( new osg::Vec3Array ( n.begin(), n.end() ) );

    osg::ref_ptr<osg::Geometry> geom  ( new osg::Geometry );
    geode->addDrawable ( geom.get() );

    geom->setVertexArray ( vertices.get() );
    geom->setNormalArray ( normals.get() );
    geom->setNormalBinding ( osg::Geometry::BIND_PER_PRIMITIVE );
    geom->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::TRIANGLES, 0, vertices->size() ) );
  }

#endif

  return root.release();
}

void  ReaderWriterSTL::_init()
{
  for(unsigned int i = 0; i < _facets.size(); ++i)
    delete _facets[i];
  _facets.clear();
  _currentFacet = NULL;
  _polygons.clear();
}

//TODO detect if stl is binary or ascii format
void ReaderWriterSTL::_parse ( std::ifstream &in )
{
  const unsigned int size ( 512 );
  char buf[size];

  in.read(buf, 80);
  buf[80] = 0;

  bool binaryFlag = false;
  for(unsigned int i = 0; i < 80; ++i)
  {
    if(buf[i] == 0x0)
      binaryFlag = true;
  }
  if( binaryFlag )
  {
    _parseBinaryFile ( in );
  }
  else
  {
    in.seekg(0, std::ios_base::beg );
    _parseAsciiFile( in );
  }
}

void ReaderWriterSTL::_parseBinaryFile( std::ifstream &in )
{
  const unsigned int size ( 512 );
  char buf[size];

  int numFacets = 0;
  
  in.read(buf, 4);

  memcpy(&numFacets, buf, 4);

  while ( numFacets > 0 )
  {
    float f[3];

    _currentFacet = new Facet();
    in.read(buf, 50);
    memcpy(f, buf, 12); // copy first 12 bytes for normal
    osg::Vec3 n ( f[0], f[1], f[2] );
    _currentFacet->setNormal ( n );
    _polygons[3].second.push_back ( n );

    memcpy(f, buf+12, 12);  // copy first vertex
    osg::Vec3 v0 ( f[0], f[1], f[2] );
    _currentFacet->setVertex ( v0 );
    _polygons[3].first.push_back ( v0 );

    memcpy(f, buf+24, 12);  // copy second vertex
    osg::Vec3 v1 ( f[0], f[1], f[2] );
    _currentFacet->setVertex ( v1 );
    _polygons[3].first.push_back ( v1 );

    memcpy(f, buf+36, 12);  // copy third vertex
    osg::Vec3 v2 ( f[0], f[1], f[2] );
    _currentFacet->setVertex ( v2 );
    _polygons[3].first.push_back ( v2 );
    
    _facets.push_back(_currentFacet);
    _currentFacet = NULL;
    numFacets--;
  }
}
  
void ReaderWriterSTL::_parseAsciiFile( std::ifstream &in )
{
  const unsigned int size ( 512 );
  char buf[size];
  Vertices vertices;
  Normals normals;

  while( !in.eof() )
  {
    in.getline(buf, size - 1);

    std::istringstream in ( buf );

    std::string type;
    in >> type;

    if(type == "solid")
    {
    }
    else if (type == "facet")
    {
      _currentFacet = new Facet();
      std::string normal;
      in >> normal;
      osg::Vec3 n;
      in >> n[0] >> n[1] >> n[2];
      _currentFacet->setNormal ( n );
      normals.push_back ( n );
    }
    else if (type == "outer")
    {
    }
    else if (type == "vertex")
    {
      osg::Vec3 v;
      in >> v[0] >> v[1] >> v[2];
      _currentFacet->setVertex ( v );
      vertices.push_back ( v );
    }
    else if (type == "endloop")
    {
      // Make sure we have one of these.
      _polygons[vertices.size()];

      // Now get short-cuts.
      Vertices &v = _polygons[vertices.size()].first;
      Normals  &n = _polygons[vertices.size()].second;

      // Append the new vertices and normals.
      v.insert ( v.end(), vertices.begin(), vertices.end() );
      n.insert ( n.end(), normals.begin(),  normals.end()  );

      // Clear the temporary lists.
      vertices.clear();
      normals.clear();
    }
    else if (type == "endfacet")
    {
      _facets.push_back(_currentFacet);
      _currentFacet = NULL;
    }
    else if (type == "endsolid")
    {
    }
  }
}

ReaderWriterSTL::Result ReaderWriterSTL::_read ( const std::string &file, const Options *options )
{
  // Make sure the internal data members are initialized.
  this->_init();

  // Make sure we handle files with this extension.
  if ( !this->acceptsExtension ( osgDB::getFileExtension ( file ) ) )
    return ReadResult::FILE_NOT_HANDLED;

  // Open the file.
  std::ifstream in ( file.c_str() );

  // Make sure it opened.
  if ( !in.is_open() )
    return ReadResult::ERROR_IN_READING_FILE;
  
  // Parse all the file and build internal data.
  this->_parse ( in );

  // Build the scene.
  osg::ref_ptr<osg::Group> root ( _build() );

  // Initialized again to free accumulated data.
  this->_init();

  // Return the scene
  return root.release();
}

osgDB::RegisterReaderWriterProxy<ReaderWriterSTL> g_ReaderWriter_STL_proxy;
