
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "Experimental/FieldViewReader/FieldViewReader/FieldViewReaderDocument.h"

#include "Usul/Interfaces/IDisplaylists.h"
#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Interfaces/IViewPort.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Predicates/FileExists.h"

#include "Usul/Strings/Convert.h"
#include "Usul/Strings/Case.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Trace/Trace.h"
#include "Usul/File/Path.h"
#include "Usul/File/Find.h"
#include "Usul/File/Slash.h"
#include "Usul/Print/Matrix.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Math/MinMax.h"

#include "OsgTools/DisplayLists.h"
#include "OsgTools/Group.h"
#include "OsgTools/Convert.h"
#include "OsgTools/State/StateSet.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
#include "MenuKit/ToggleButton.h"
#include "MenuKit/RadioButton.h"

#include "osgDB/ReadFile"
#include "osg/Group"
#include "osg/Switch"
#include "osg/MatrixTransform"
#include "osgDB/WriteFile"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/StateSet"
#include "osg/Texture2D"
#include "osg/BoundingBox"

#include <iterator>
#include <vector>
#include <fstream>
#include <iostream>


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( FieldViewReaderDocument, FieldViewReaderDocument::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

FieldViewReaderDocument::FieldViewReaderDocument() : 
  BaseClass ( "Field View Reader" ),
  _root( new osg::Group ),
  _vertices( 0 )
{
  USUL_TRACE_SCOPE;
   
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

FieldViewReaderDocument::~FieldViewReaderDocument()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void FieldViewReaderDocument::clear ( Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *FieldViewReaderDocument::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;

  switch ( iid )
  {
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  case Usul::Interfaces::IUpdateListener::IID:
    return static_cast < Usul::Interfaces::IUpdateListener* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document exports given extension
//
///////////////////////////////////////////////////////////////////////////////

bool FieldViewReaderDocument::canExport ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document inserts given extension
//
///////////////////////////////////////////////////////////////////////////////

bool FieldViewReaderDocument::canInsert ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return this->canOpen ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document reads given extension
//
///////////////////////////////////////////////////////////////////////////////

bool FieldViewReaderDocument::canOpen ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "fvcfd" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document writes given extension
//
///////////////////////////////////////////////////////////////////////////////

bool FieldViewReaderDocument::canSave ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "fvcfd" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void FieldViewReaderDocument::write ( const std::string &name, Unknown *caller, Unknown *progress ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  
  std::string filename = Usul::File::directory( name, true );
  filename = filename + Usul::File::base( name );
  std::cout << "Writing file " << filename << "..." << std::endl;
  osgDB::writeNodeFile( *_root.get(), filename+ ".ive" );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void FieldViewReaderDocument::read ( const std::string &name, Unknown *caller, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  this->_readColorTable( name );
  this->_readVertices( name );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

FieldViewReaderDocument::Filters FieldViewReaderDocument::filtersSave() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  filters.push_back ( Filter ( "CFD Visualization (*.fvcfd)", "*.fvcfd" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

FieldViewReaderDocument::Filters FieldViewReaderDocument::filtersExport() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

FieldViewReaderDocument::Filters FieldViewReaderDocument::filtersOpen() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  filters.push_back ( Filter ( "CFD Visualization (*.fvcfd)", "*.fvcfd" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

FieldViewReaderDocument::Filters FieldViewReaderDocument::filtersInsert() const
{
  USUL_TRACE_SCOPE;
  return this->filtersOpen();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *FieldViewReaderDocument::buildScene ( const BaseClass::Options &options, Unknown *caller )
{
  USUL_TRACE_SCOPE;
  _root->removeChild( 0, _root->getNumChildren() );

  _root->addChild( this->_buildVectorArrows( true, 2.0, caller ) );
#if 1
  this->write( _filename, caller );
#endif
  std::cout << "Buildscene complete!" << std::endl;
  return _root.get();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void FieldViewReaderDocument::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;

}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the document.
//
///////////////////////////////////////////////////////////////////////////////

void FieldViewReaderDocument::_openDocument ( const std::string &file, Usul::Documents::Document *document, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  //Guard guard ( this->mutex() );
  if ( 0x0 == document )
    return;
#if 1
  if( true == document->canOpen( file ) )
    document->open ( file, caller );
  else
    std::cout << "Unable to open document for file: " << file << std::endl;
#else
    document->open ( file, caller );
#endif
  std::cout << "Done" << std::endl;
  
	
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function to set status bar and flush events.
//
///////////////////////////////////////////////////////////////////////////////

void FieldViewReaderDocument::_setStatusBar ( const std::string &text, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  Usul::Interfaces::IStatusBar::QueryPtr status ( caller );
  if ( status.valid() )
    status->setStatusBarText ( text, true );
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void FieldViewReaderDocument::_readVertices( const std::string& filename, Unknown *progress )
{
 USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  std::ifstream in;
  in.open( filename.c_str() );
  std::string temp;
  const unsigned int bufSize ( 2048 ); // should be big enough
  char buffer[bufSize];

  //trim header information
#if 0
  //skip the first line
  in.getline ( buffer, bufSize );
  unsigned int numsets = 0;
  in >> numsets;
  in.getline ( buffer, bufSize );
  //skip the object poly counts
  for( unsigned int i = 0; i < numsets; ++i )
  {
    in.getline ( buffer, bufSize );
  }
#else
  //skip the first line
  in.getline ( buffer, bufSize );
#endif
  //get the number of points
  unsigned int numPoints = 0;
  in >> numPoints;
  in.getline ( buffer, bufSize );
  // Set the progress numbers.
  _progress.first = 0;
  _progress.second = numPoints;

  Usul::Policies::TimeBased update ( 1000 ); // Every second.
  
  // skip the column header
  in.getline ( buffer, bufSize );

  //Feedback
  std::cout << "Reading vertices..." << std::endl;  

  //assuming number of columns is 4
  for( unsigned int i = 0; i <  numPoints ; ++i )
  {

    // 1st point for the triangle
    double x = 0, y = 0, z = 0, s = 0, u = 0, v = 0, w = 0, t = 0;

    in >> x >> y >> z >> s >> u >> v >> w >> t; 
    in.getline ( buffer, bufSize );

    CFDPoint p;
    p.pos = Vec3d( x, y, z );
    p.dir = Vec3d( u, v, w );

    _vertices.push_back( p );

    _vertices.push_back( p );
 
    // Feedback.
    this->_incrementProgress ( update(), progress );

  }

  
  in.close();
}

void FieldViewReaderDocument::_readColorTable( const std::string& filename, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  
  std::string directory = Usul::File::directory( filename, true );
  std::string basename = Usul::File::base( filename );

  std::string file = directory + basename + ".color";

  std::ifstream in;
  in.open( file.c_str() );
  _colorTable.resize( 256 );
  for( unsigned int i = 0; i < 256; i ++ )
  {
    double temp;
    char comma;

    in >> temp >> comma
       >> _colorTable.at( i )[0] >> comma 
       >> _colorTable.at( i )[1] >> comma
       >> _colorTable.at( i )[2] >> comma
       >> _colorTable.at( i )[3];
  }
  in.close();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void FieldViewReaderDocument::_incrementProgress ( bool state, Unknown *progress )
{
  unsigned int &numerator   ( _progress.first  );
  unsigned int &denominator ( _progress.second );
  this->setProgressBar ( state, numerator, denominator, progress );
  ++numerator;
  USUL_ASSERT ( numerator <= denominator );
}



///////////////////////////////////////////////////////////////////////////////
//
// Build vector arrows from input vector file.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* FieldViewReaderDocument::_buildVectorArrows( bool drawMirror, double arrowSize, Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  osg::ref_ptr< osg::Group > group ( new osg::Group );
  osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
  osg::ref_ptr< osg::DrawElementsUInt > indices ( new osg::DrawElementsUInt( GL_LINES ) );

  for( unsigned int i = 0; i < _vertices.size(); ++i )
  {
    
    osg::Vec3d start( _vertices.at( i ).pos[0], _vertices.at( i ).pos[1], _vertices.at( i ).pos[2] );
    osg::Vec3d end ( _vertices.at( i ).dir[0], _vertices.at( i ).dir[1], _vertices.at( i ).dir[2] );
    double magnitude = end.length();
    _values.push_back( magnitude );

    if( true == _firstPass )
    {
      _firstPass = false;
      _colorRange[0] = _colorRange[1] = magnitude;
    }
    else
    {
      _colorRange[0] = Usul::Math::minimum( _colorRange[0], magnitude );
      _colorRange[1] = Usul::Math::maximum( _colorRange[1], magnitude );
    }

    end.normalize();
    end *= arrowSize;
    vertices->push_back( start );
    vertices->push_back( start + end );
    
    if( true == drawMirror )
    {
      const osg::Vec3d startMirror( start.x(), start.y(), start.z() * -1 );
      const osg::Vec3d endMirror( end.x(), end.y(), end.z() * -1 );
      vertices->push_back( startMirror );
      vertices->push_back( startMirror + endMirror ); 
    
      indices->push_back( i * 4 );
      indices->push_back( ( i * 4 ) + 1 );
      indices->push_back( ( i * 4 ) + 2 );
      indices->push_back( ( i * 4 ) + 3 );
    }
    else
    {
      indices->push_back( i * 2 );
      indices->push_back( ( i * 2 ) + 1 );
    }
     
  }

  OsgTools::State::StateSet::setLineWidth( geometry->getOrCreateStateSet(), 2 );
  OsgTools::State::StateSet::setLighting( geometry->getOrCreateStateSet(), false );
  osg::ref_ptr< osg::Vec4Array > colors ( new osg::Vec4Array() );
  colors = this->_getColorArray( drawMirror );
  geometry->setColorArray( colors.get() );
  geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
  geometry->addPrimitiveSet( indices.get() );
  geometry->setVertexArray( vertices.get() );
  geode->addDrawable( geometry.get() );
  group->addChild( geode.get() );
  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the read in color table.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec4Array * FieldViewReaderDocument::_getColorArray( bool drawMirror )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  osg::ref_ptr< osg::Vec4Array > colors ( new osg::Vec4Array() );
  double maxRange = _colorRange[1];
  for( unsigned int i = 0; i < _values.size(); ++i )
  {
    double value = _values.at( i ) - _colorRange[0];
    Color4 color = this->_getInterpolatedColorValue( value / maxRange );
    unsigned int numj = 0;
    if( true == drawMirror )
      numj = 4;
    else
      numj = 2;
    for( unsigned int j = 0; j < numj; ++j )
    {
      colors->push_back( osg::Vec4f( 0.0, 0.0, 0.0, 1.0 ) );
      colors->push_back( osg::Vec4f( static_cast< float > ( color[0] / 255 ),
                                     static_cast< float > ( color[1] / 255 ),
                                     static_cast< float > ( color[2] / 255 ),
                                     static_cast< float > ( color[3] ) ) );
      j++;
    }
  }

  return colors.release();
}



///////////////////////////////////////////////////////////////////////////////
//
// Get the color value at a given magnitude value.
//
///////////////////////////////////////////////////////////////////////////////

FieldViewReaderDocument::Color4 FieldViewReaderDocument::_getInterpolatedColorValue( double value )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  Color4 c( 0, 0, 0, 1 );

  double cvalue = value * 255;

  unsigned int minValue = static_cast< unsigned int > ( floor( cvalue ) );
  unsigned int maxValue = minValue + 1;

  minValue = Usul::Math::minimum( minValue, static_cast< unsigned int > ( 255 ) );
  maxValue = Usul::Math::minimum( maxValue, static_cast< unsigned int > ( 255 ) );

  
  Color4 cmin = _colorTable.at( minValue );
  Color4 cmax = _colorTable.at( maxValue );

  c = cmin + ( ( cmax - cmin ) * 0.5 );
 
  return c;
}

