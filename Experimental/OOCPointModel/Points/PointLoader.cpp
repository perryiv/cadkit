
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "PointLoader.h"

#include "Usul/Trace/Trace.h"
#include "Usul/Components/Manager.h"
#include "Usul/System/Host.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Math/Functions.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Scope/CurrentDirectory.h"
#include "Usul/File/Stats.h"

#include "Usul/Interfaces/IMemoryPool.h"
#include "Usul/Interfaces/IMpdNavigator.h"

#include "OsgTools/Triangles/TriangleSet.h"

#include <fstream>

//USUL_IMPLEMENT_COMMAND ( PointLoader );


///////////////////////////////////////////////////////////////////////////////
//
//  Model Presentation Job Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PointLoader::PointLoader ( const std::string &path, Usul::Types::Uint64 numPoints ) :
  BaseClass ( false ),
  _path( path ),
  _numPoints( numPoints ),
  _foundNewData( false ),
  _vertices( new osg::Vec3Array )
{
  USUL_TRACE_SCOPE;
}

  
///////////////////////////////////////////////////////////////////////////////
//
//  Model Presentation Job Destructor.
//
///////////////////////////////////////////////////////////////////////////////

PointLoader::~PointLoader()
{
  USUL_TRACE_SCOPE;

  // clean up circular references
}



///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to shorten the lines.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class T > inline void checkCancelledState ( T *t )
  {
    if ( 0x0 != t )
    {
      if ( true == t->canceled() )
      {
        t->cancel();
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the job.
//
///////////////////////////////////////////////////////////////////////////////

void PointLoader::_started ()
{
  Helper::checkCancelledState ( this );

  // Read the file
  std::ifstream infile ( _path.c_str(),  std::ofstream::in | std::ofstream::binary );

  Usul::Types::Uint64 fileSize ( Usul::File::size( _path ) );

  Helper::checkCancelledState ( this );

  // Initialize the points holder
  osg::ref_ptr< osg::Vec3Array > points ( new osg::Vec3Array );
  points->resize ( _numPoints, osg::Vec3f( 0.0f, 0.0f, 0.0f ) );

  Helper::checkCancelledState ( this );

  // Read the points
  infile.read( reinterpret_cast<char *> ( &((*points)[0]) ), fileSize );

  Helper::checkCancelledState ( this );

  if( false == _vertices.valid() )
    _vertices = new osg::Vec3Array;

  _vertices = points;

  _foundNewData = true;

  Helper::checkCancelledState ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the stored data for this job
//
///////////////////////////////////////////////////////////////////////////////

osg::ref_ptr< osg::Vec3Array > PointLoader::getData()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  return osg::ref_ptr< osg::Vec3Array > ( _vertices );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the state of new data discovery
//
///////////////////////////////////////////////////////////////////////////////

bool PointLoader::foundNewData()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  return _foundNewData;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state of new data discovery
//
///////////////////////////////////////////////////////////////////////////////

void PointLoader::foundNewData( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  _foundNewData = state;
}
