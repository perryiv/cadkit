
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Authors: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Widgets/Helper/UpdateProgress.h"

#include "osg/NodeVisitor"

using namespace OsgTools::Widgets::Helper;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

UpdateProgressCallback::UpdateProgressCallback ( ) : 
  BaseClass(),
  _mutex ( 0x0 ),
  _dirty ( false ),
  _geometry ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the bar progression
//
///////////////////////////////////////////////////////////////////////////////

void UpdateProgressCallback::update ( osg::NodeVisitor *nv, osg::Drawable* )
{
  Guard guard ( *_mutex );

  // Only update if it's an update visitor.
  if( nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR )
  {
    if( true == _dirty && 0x0 != _geometry )
    {
      osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array() );
      vertices->reserve( 4 );
      vertices->push_back ( osg::Vec3f ( _ul.x(), _lr.y(), 0.0 ) );
      vertices->push_back ( osg::Vec3f ( _lr.x(), _lr.y(), 0.0 ) );
      vertices->push_back ( osg::Vec3f ( _lr.x(), _ul.y(), 0.0 ) );
      vertices->push_back ( osg::Vec3f ( _ul.x(), _ul.y(), 0.0 ) );
      _dirty = false;

      _geometry->setVertexArray ( vertices.get() );

    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assign the private member geometry object
//
///////////////////////////////////////////////////////////////////////////////

void UpdateProgressCallback::setGeometry ( osg::Geometry * geometry )
{
  Guard guard ( *_mutex );
  _geometry = geometry;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the bounds of the geometry object.
//
///////////////////////////////////////////////////////////////////////////////

void UpdateProgressCallback::setBounds ( const osg::Vec2f& ul,  const osg::Vec2f& lr )
{
  Guard guard ( *_mutex );
  _ul = ul;
  _lr = lr;

  _dirty = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the mutex.
//
///////////////////////////////////////////////////////////////////////////////

void UpdateProgressCallback::mutex ( Mutex* mutex )
{
  _mutex = mutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

UpdateProgress::UpdateProgress () : 
  BaseClass(),
  _mutex ( Mutex::create() ),
  _updateCallback ( new UpdateProgressCallback )
{
  _updateCallback->mutex ( _mutex );
  _updateCallback->setGeometry( this );
  this->setUpdateCallback ( _updateCallback.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

UpdateProgress::~UpdateProgress()
{
  // Make sure we unreference the update callback before deleting the mutex.
  this->setUpdateCallback( 0x0 );
  _updateCallback = 0x0;

  delete _mutex;
}


void UpdateProgress::setBounds ( const osg::Vec2f& ul,  const osg::Vec2f& lr )
{
  _updateCallback->setBounds ( ul, lr );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Call the base class' draw.  Lock the mutex.
//
///////////////////////////////////////////////////////////////////////////////

#if OSG_VERSION_MAJOR <= 1 && OSG_VERSION_MINOR <= 2
void UpdateProgress::drawImplementation ( osg::State & state ) const
#else
void UpdateProgress::drawImplementation ( osg::RenderInfo& info ) const
#endif
{
  Guard guard ( *_mutex );

#if OSG_VERSION_MAJOR <= 1 && OSG_VERSION_MINOR <= 2
  BaseClass::drawImplementation ( state );
#else
  BaseClass::drawImplementation ( info );
#endif
}

