
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Experimental/WRF/WrfModel/LoadDataJob.h"
#include "Experimental/WRF/WrfModel/WRFDocument.h"

#include "Usul/Predicates/CloseFloat.h"

#include "osg/Image"

///////////////////////////////////////////////////////////////////////////////
//
//  Normalize.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{  
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
}

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LoadDataJob::LoadDataJob ( const ReadRequests& requests, WRFDocument* document, const Parser& parser ) :
  BaseClass (),
  _requests ( requests ),
  _document ( document ),
  _parser ( parser ),
  _x ( 0 ),
  _y ( 0 ),
  _z ( 0 )
{
  USUL_TRACE_SCOPE;

  if ( 0x0 != _document )
    _document->ref ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

LoadDataJob::~LoadDataJob ()
{
  USUL_TRACE_SCOPE;

  if ( 0x0 != _document )
    _document->unref ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the data.
//
///////////////////////////////////////////////////////////////////////////////

void LoadDataJob::_started ()
{
  USUL_TRACE_SCOPE;

  // Return if we don't have any.
  if ( _requests.empty() || 0x0 == _document )
    return;

  // Process all the requests.
  while ( false == _requests.empty() )
  {
    ReadRequest request ( _requests.front() );
    _requests.pop_front ();

    // Vector for raw floating point data.
    Parser::Data data;

    osg::ref_ptr < osg::Image > image ( this->_createImage ( request, data ) );

    unsigned int timestep ( request.first );
    unsigned int channel ( request.second->index () );

    _document->addVolume ( data, image.get(), timestep, channel );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  All done.
//
///////////////////////////////////////////////////////////////////////////////

void LoadDataJob::_finished ()
{
  USUL_TRACE_SCOPE;

  if ( 0x0 != _document )
    _document->loadJobFinished ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the sizes..
//
///////////////////////////////////////////////////////////////////////////////

void LoadDataJob::setSize ( unsigned int x, unsigned int y, unsigned int z )
{
  USUL_TRACE_SCOPE;

  _x = x;
  _y = y;
  _z = z;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the image.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* LoadDataJob::_createImage ( const ReadRequest& request, Parser::Data& data )
{
  USUL_TRACE_SCOPE;

  Channel::RefPtr info ( request.second );
  unsigned int timestep ( request.first );
  unsigned int channel ( info->index () );

  std::cout << "Reading data.  Timestep: " << timestep << " Channel: " << channel << std::endl;

  _parser.data ( data, timestep, channel );

  // Normalize the data to unsigned char.
  std::vector < unsigned char > chars;
  Detail::normalize ( chars, data, info->min (), info->max () );

  unsigned int width ( _x ), height ( _y ), depth ( _z );

  osg::ref_ptr < osg::Image > image ( new osg::Image );
  image->allocateImage ( width, height, depth, GL_LUMINANCE, GL_UNSIGNED_BYTE );

  unsigned char *pixels ( image->data() );
  std::copy ( chars.begin(), chars.end(), pixels );
#if _MSC_VER
  osg::ref_ptr < osg::Image > scaled ( new osg::Image );
  scaled->allocateImage ( 512, 512, 128, GL_LUMINANCE, GL_UNSIGNED_BYTE );
  ::memset ( scaled->data(), 0, 512 * 512 * 128 );

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

  return scaled.release ();
#else
  return image.release();
#endif
}
