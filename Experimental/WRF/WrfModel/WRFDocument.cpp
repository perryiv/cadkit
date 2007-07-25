
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "WRFDocument.h"

#include "Usul/File/Path.h"
#include "Usul/Strings/Case.h"
#include "Usul/Strings/Convert.h"
#include "Usul/Predicates/CloseFloat.h"
#include "Usul/Functions/Color.h"

#include "XmlTree/Document.h"

#include "OsgTools/Volume/Texture3DVolume.h"

#include "osg/MatrixTransform"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( WRFDocument, WRFDocument::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

WRFDocument::WRFDocument() : BaseClass ( "WRF Document" ),
  _parser (),
  _filename ( "" ),
  _currentTimestep ( 0 ),
  _currentChannel ( 0 ),
  _timesteps ( 0 ),
  _channels ( 0 ),
  _x ( 0 ),
  _y ( 0 ),
  _z ( 0 ),
  _channelInfo ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

WRFDocument::~WRFDocument()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *WRFDocument::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document exports given extension
//
///////////////////////////////////////////////////////////////////////////////

bool WRFDocument::canExport ( const std::string &file ) const
{
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document inserts given extension
//
///////////////////////////////////////////////////////////////////////////////

bool WRFDocument::canInsert ( const std::string &file ) const
{
  return this->canOpen ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document reads given extension
//
///////////////////////////////////////////////////////////////////////////////

bool WRFDocument::canOpen ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "wrf" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document writes given extension
//
///////////////////////////////////////////////////////////////////////////////

bool WRFDocument::canSave ( const std::string &file ) const
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "wrf" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::write ( const std::string &name, Unknown *caller  ) const
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::read ( const std::string &name, Unknown *caller )
{
  XmlTree::Document::RefPtr document ( new XmlTree::Document );
  document->load ( name );

  if ( "wrf" == document->name() )
  {
    this->_read ( *document );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::_read ( XmlTree::Node &node )
{
  typedef XmlTree::Document::Attributes Attributes;
  typedef XmlTree::Document::Children Children;

  Attributes& attributes ( node.attributes() );
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    if ( "timesteps" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, _timesteps );
    }
    if ( "channels" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, _channels );
    }
    if ( "x" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, _x );
    }
    if ( "y" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, _y );
    }
    if ( "z" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, _z );
    }
    if ( "file" == iter->first )
    {
      _filename = iter->second;
    }
  }

  // Set the parser's datamembers.
  _parser.filename ( _filename );
  _parser.timesteps ( _timesteps );
  _parser.channels ( _channels );
  _parser.setSizes ( _x, _y, _z );

  // Make enough room for the channel information.
  _channelInfo.resize ( _channels );

  Children& children ( node.children() );
	
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    if ( "channel" == node->name() )
    {
      this->_parseChannel ( *node );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a channel.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::_parseChannel ( XmlTree::Node &node )
{
  typedef XmlTree::Document::Attributes Attributes;

  ChannelInfo info;

  Attributes& attributes ( node.attributes() );
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    if ( "name" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, info.name );
    }
    if ( "index" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, info.index );
    }
    if ( "min" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, info.min );
    }
    if ( "max" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, info.max );
    }
  }

  _channelInfo.at ( info.index ) = info;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void WRFDocument::clear ( Usul::Interfaces::IUnknown *caller )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

WRFDocument::Filters WRFDocument::filtersSave() const
{
  Filters filters;
  filters.push_back ( Filter ( "WRF xml (*.wrf)", "*.wrf" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

WRFDocument::Filters WRFDocument::filtersExport() const
{
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

WRFDocument::Filters WRFDocument::filtersOpen() const
{
  Filters filters;
  filters.push_back ( Filter ( "WRF xml (*.wrf)", "*.wrf" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

WRFDocument::Filters WRFDocument::filtersInsert() const
{
  return this->filtersOpen();
}


namespace Detail
{
  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Normalize.
  //
  ///////////////////////////////////////////////////////////////////////////////

  void normalize ( std::vector < unsigned char >& out, const std::vector < float >& in, double min, double max )
  {
    out.resize ( in.size() );

    Usul::Predicates::CloseFloat< double > close ( 10 );

    for ( unsigned int i = 0; i < in.size(); ++i )
    {
      double value ( in.at( i ) );
      bool inValid ( !Usul::Math::finite ( value ) || Usul::Math::nan ( value ) || close ( value, 1.0e+035f ) );
      
      if( false == inValid )
      {
        value = ( value - min ) / ( max - min );
        out.at( i ) = static_cast < unsigned char > ( value * 255 );
      }
      else
        out.at ( i ) = 0;
    }
  }

  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Build the transferFunction.
  //
  ///////////////////////////////////////////////////////////////////////////////

  osg::Image* buildTransferFunction ( )
  {
    osg::ref_ptr < osg::Image > image ( new osg::Image );
    image->allocateImage ( 256, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE );

    unsigned char *data ( image->data() );

    for ( unsigned int i = 0; i < 256; ++i )
    {
  #if 0
      *data++ = i;
      *data++ = i;
      *data++ = i;
      *data++ = i;//i == 0 ? 0 : 5;
  #else
      float value ( static_cast < float > ( i ) / 255 );
      float r ( 0.0 ), g ( 0.0 ), b ( 0.0 );
      Usul::Functions::hsvToRgb ( r, g, b, value * 360, 1.0f, 1.0f );
      *data++ = static_cast < unsigned char > ( r * 255 );
      *data++ = static_cast < unsigned char > ( g * 255 );
      *data++ = static_cast < unsigned char > ( b * 255 );
      *data++ = ( i < 10 ? 0 : 10 );
  #endif
    }

    return image.release();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *WRFDocument::buildScene ( const BaseClass::Options &options, Unknown *caller )
{
  Parser::Data data;

  _parser.data ( data, _currentChannel, _currentTimestep );

  ChannelInfo info ( _channelInfo.at ( _currentChannel ) );

  // Normalize the data to unsigned char.
  std::vector < unsigned char > chars;
  Detail::normalize ( chars, data, info.min, info.max );

  // Create the bounding box.
  double xLength ( _x * 10 );
  double yLength ( _y * 10 );
  double zLength ( _z * 3 );

  double d ( xLength );

  xLength /= d;
  yLength /= d;
  zLength /= d;

  float xHalf ( static_cast < float > ( xLength ) / 2.0f );
  float yHalf ( static_cast < float > ( yLength ) / 2.0f );
  float zHalf ( static_cast < float > ( zLength ) / 2.0f );

  osg::BoundingBox bb ( -xHalf, -yHalf, -zHalf, xHalf, yHalf, zHalf );

  osg::ref_ptr < osg::MatrixTransform > root ( new osg::MatrixTransform );
  root->addChild ( this->_buildVolume ( bb, chars ) );

  return root.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the volume.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* WRFDocument::_buildVolume( const osg::BoundingBox& bb, const std::vector < unsigned char >& data )
{
  unsigned int width ( _y ), height ( _x ), depth ( _z );

  osg::ref_ptr < osg::Image > image ( new osg::Image );
  image->allocateImage ( width, height, depth, GL_LUMINANCE, GL_UNSIGNED_BYTE );

  unsigned char *pixels ( image->data() );
  std::copy ( data.begin(), data.end(), pixels );

  osg::ref_ptr < osg::Image > scaled ( new osg::Image );
  scaled->allocateImage ( 256, 256, 256, GL_LUMINANCE, GL_UNSIGNED_BYTE );
  ::memset ( scaled->data(), 0, 256 * 256 * 256 );

  for ( unsigned int r = 0; r < depth; ++r )
  {
    for ( unsigned int t = 0; t < height; ++ t )
    {
      for ( unsigned int s = 0; s < width; ++ s )
      {
        *scaled->data ( s, t, r ) = *image->data ( s, t, r );
      }
    }
  }

  osg::ref_ptr < OsgTools::Volume::Texture3DVolume > volume ( new OsgTools::Volume::Texture3DVolume );
  volume->numPlanes ( 256 );
  volume->image ( scaled.get() );
  volume->boundingBox ( bb );
  volume->transferFunction ( Detail::buildTransferFunction() );

  return volume.release();
}
