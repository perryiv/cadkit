
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


ReaderWriterSTL::ReaderWriterSTL() :
_facets(),
_currentFacet(NULL)
{
}

ReaderWriterSTL::~ReaderWriterSTL()
{
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

  for(unsigned int i = 0; i < _facets.size(); ++i)
  {
    osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
    osg::ref_ptr< osg::Geometry > geometry ( _facets[i]->getGeometry() );
    geode->addDrawable ( geometry.get() );
    root->addChild ( geode.get() );
  }

  return root.release();
}

void  ReaderWriterSTL::_init()
{
  for(unsigned int i = 0; i < _facets.size(); ++i)
    delete _facets[i];
  _facets.clear();
  _currentFacet = NULL;
}

//TODO detect if stl is binary or ascii format
void ReaderWriterSTL::_parse ( std::ifstream &in )
{
  const unsigned int size ( 512 );
  char buf[size];

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
      float n1, n2, n3;
      in >> n1;
      in >> n2;
      in >> n3;
      _currentFacet->setNormal( osg::Vec3(n1, n2, n3) ); 
    }
    else if (type == "outer")
    {
    }
    else if (type == "vertex")
    {
      float n1, n2, n3;
      in >> n1;
      in >> n2;
      in >> n3;
      _currentFacet->setVector( osg::Vec3(n1, n2, n3) );
    }
    else if (type == "endloop")
    {
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
