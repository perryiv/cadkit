
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "LabelReader.h"

#include "osgText/Font"
#include "osgText/Text"
#include "osg/Geometry"
#include "osg/StateSet"
#include "osg/MatrixTransform"
#include "osg/Matrix"

#include <iostream>

//USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( LabelReader, LabelReader::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

LabelReader::LabelReader():
_scene ( new osg::Group )

{
	
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

LabelReader::~LabelReader()
{
	
}

/////////////////
//  PUBLIC
/////////////////

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void LabelReader::read( const std::string & filename )
{
  this->_read( filename );
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * LabelReader::buildScene( bool box )
{
  
  _scene->removeChild( 0, _scene->getNumChildren() );
#if 0
  // Feedback
  std::cout << "Building the scene... " << std::endl;
  for( unsigned int i = 0; i < _locations.size(); ++i )
  {
    _scene->addChild( this->_createText( i ) );
  }
  if( true == box )
    _scene->addChild( this->_drawBox() );
  std::cout << "Done! " << std::endl;
#else
  // Feedback
  std::cout << "Building the scene... " << std::endl;
  osg::ref_ptr< osg::MatrixTransform > mt ( new osg::MatrixTransform );
  //mt->setMatrix( osg::Matrix::scale( 12.0, 12.0, 1.0 ) );
  for( unsigned int i = 0; i < _locations.size(); ++i )
  {
    mt->addChild( this->_createText( i ) );
  }
  if( true == box )
    mt->addChild( this->_drawBox() );
  std::cout << "Done! " << std::endl;
#endif
  _scene->addChild( mt.get() );
  return _scene.get();
}


/////////////////
//  PROTECTED
/////////////////

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void LabelReader::_read( const std::string & filename )
{

  // Feedback
  std::cout << "Reading input file: " << filename << std::endl;

  const unsigned long int bufSize ( 4095 );
  char buffer[bufSize+1];
  std::ifstream in;
  in.rdbuf()->pubsetbuf ( buffer, bufSize );
  in.open ( filename.c_str(), std::ios::in  );
  if ( !in.is_open() )
    throw std::runtime_error ( "Error 1099205557: Failed to open file: " + filename );

  // Read the 1st entry
  if ( EOF != in.peek() )
  {
    Location loc;
    in >> loc.pos[0] >> loc.pos[1] >> loc.pos[2];
    loc.pos[0] = loc.pos[0] * 10.0;
    loc.pos[1] = loc.pos[1] * 10.0;

    _min[0] = loc.pos[0];
    _min[1] = loc.pos[1];
    _min[2] = loc.pos[2];

    _max[0] = loc.pos[0];
    _max[1] = loc.pos[1];
    _max[2] = loc.pos[2];


    in.getline ( buffer, bufSize );
    loc.text = buffer;
    if ( false == loc.text.empty() )
    {
      if ( '\n' == loc.text[loc.text.size()-1] )
      {
        loc.text.resize ( loc.text.size() - 1 );
      }
    }

    // Feedback
    std::cout << "Found Location: " << loc.text << " at " 
              << loc.pos[0] << ", " << loc.pos[1] << ", " << loc.pos[2] << std::endl;

    

    _locations.push_back( loc );
  }
  while ( EOF != in.peek() )
  {
    Location loc;
    in >> loc.pos[0] >> loc.pos[1] >> loc.pos[2]; 
    loc.pos[0] = loc.pos[0] * 10.0;
    loc.pos[1] = loc.pos[1] * 10.0;
    

    if( loc.pos[0] < _min[0] )
      _min[0] = loc.pos[0]; 
    if( loc.pos[1] < _min[1] )
      _min[1] = loc.pos[1]; 
    if( loc.pos[2] < _min[2] )
      _min[2] = loc.pos[2]; 

    if( loc.pos[0] > _max[0] )
      _max[0] = loc.pos[0]; 
    if( loc.pos[1] > _max[1] )
      _max[1] = loc.pos[1]; 
    if( loc.pos[2] > _max[2] )
      _max[2] = loc.pos[2]; 
    
    in.getline ( buffer, bufSize );
    loc.text = buffer;
    if ( false == loc.text.empty() )
    {
      if ( '\n' == loc.text[loc.text.size()-1] )
      {
        loc.text.resize ( loc.text.size() - 1 );
      }
    }

    // Feedback
    std::cout << "Found Location: " << loc.text << " at " 
              << loc.pos[0] << ", " << loc.pos[1] << ", " << loc.pos[2] << std::endl;

    
    _locations.push_back( loc );

    
  }
      

}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* LabelReader::_drawBox()
{
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );

  //osg::ref_ptr< osg::StateSet > stateset ( geometry->getOrCreateStateSet() );
 
  osg::ref_ptr< osg::Vec4Array > black ( new osg::Vec4Array() );  
  black->push_back ( osg::Vec4 ( 0.0f, 0.0f, 0.0f, 1.0f ) );

  osg::ref_ptr< osg::Vec3Array > normal ( new osg::Vec3Array() );
  normal->push_back ( osg::Vec3 ( 0.0f, 0.0f, 1.0f ) );

  osg::Vec3f p0 ( _min[0], _min[1], _min[2] );
  osg::Vec3f p1 ( _max[0], _min[1], _min[2] );
  osg::Vec3f p2 ( _min[0], _max[1], _min[2] );
  osg::Vec3f p3 ( _max[0], _max[1], _min[2] );

  osg::Vec3f p4 ( _min[0], _min[1], _max[2] );
  osg::Vec3f p5 ( _max[0], _min[1], _max[2] );
  osg::Vec3f p6 ( _min[0], _max[1], _max[2] );
  osg::Vec3f p7 ( _max[0], _max[1], _max[2] );

  /*osg::Vec3f p0 ( 683000, 885000, 1500 );
  osg::Vec3f p1 ( 695000, 885000, 1500 );
  osg::Vec3f p2 ( 683000, 900000, 1500 );
  osg::Vec3f p3 ( 695000, 900000, 1500 );
  osg::Vec3f p4 ( 683000, 885000, 2500 );
  osg::Vec3f p5 ( 695000, 885000, 2500 );
  osg::Vec3f p6 ( 683000, 900000, 2500 );
  osg::Vec3f p7 ( 695000, 900000, 2500 );*/

  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array() );
  vertices->push_back ( p0 );vertices->push_back ( p2 );vertices->push_back ( p3 );
  vertices->push_back ( p0 );vertices->push_back ( p3 );vertices->push_back ( p1 );
  vertices->push_back ( p0 );vertices->push_back ( p1 );vertices->push_back ( p5 );
  vertices->push_back ( p0 );vertices->push_back ( p5 );vertices->push_back ( p4 );
  vertices->push_back ( p0 );vertices->push_back ( p4 );vertices->push_back ( p6 );
  vertices->push_back ( p0 );vertices->push_back ( p6 );vertices->push_back ( p2 );
  vertices->push_back ( p7 );vertices->push_back ( p6 );vertices->push_back ( p4 );
  vertices->push_back ( p7 );vertices->push_back ( p4 );vertices->push_back ( p5 );
  vertices->push_back ( p7 );vertices->push_back ( p5 );vertices->push_back ( p1 );
  vertices->push_back ( p7 );vertices->push_back ( p1 );vertices->push_back ( p3 );
  vertices->push_back ( p7 );vertices->push_back ( p3 );vertices->push_back ( p2 );
  vertices->push_back ( p7 );vertices->push_back ( p2 );vertices->push_back ( p6 );


  
  geometry->setVertexArray ( vertices.get() );

  geometry->setColorArray ( black.get() );
  geometry->setColorBinding ( osg::Geometry::BIND_OVERALL );
        
 /* geometry->setNormalArray ( normal.get() );
  geometry->setNormalBinding ( osg::Geometry::BIND_OVERALL );*/

  geometry->addPrimitiveSet ( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, 0, vertices->size() ) );


  geode->addDrawable( geometry.get() );
  return geode.release();
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * LabelReader::_createText( unsigned int index )
{
  //osg::ref_ptr< osg::Group > group ( new osg::Group );

  osg::ref_ptr< osgText::Font > font ( osgText::readFontFile ( "fonts/arial.ttf" ) );
  osg::ref_ptr< osgText::Text  > text ( new osgText::Text() );
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );

  osg::ref_ptr< osg::StateSet > stateset ( geode->getOrCreateStateSet() );
 
  stateset->setMode ( GL_BLEND, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
  stateset->setMode ( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );  
  stateset->setTextureMode( 0, osg::StateAttribute::TEXTURE, osg::StateAttribute::PROTECTED );

  text->setFont( font.get() );
  text->setColor( osg::Vec4f( 0.841, 0.763, 0.371, 1 ) );
  text->setCharacterSizeMode( osgText::Text::SCREEN_COORDS );
  text->setUseDisplayList( false );
  
  text->setCharacterSize( 30 );
  text->setPosition( _locations.at( index ).pos );
  text->setLayout( osgText::Text::LEFT_TO_RIGHT );
  text->setFontResolution ( 32, 32 );
  text->setMaximumHeight( 50 );
  
  text->setText( _locations.at( index ).text );
  text->setAutoRotateToScreen( true );


  geode->setStateSet( stateset.get() );
  geode->addDrawable( text.get() );
    
  return geode.release();
}

/////////////////
//  PRIVATE
/////////////////

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////


