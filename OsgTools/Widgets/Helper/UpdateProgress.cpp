
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Authors: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Widgets/Helper/UpdateProgress.h"

using namespace OsgTools::Widgets::Helper;

UpdateProgress::UpdateProgress () : 
  BaseClass(),
  _mutex ( Mutex::create() )
{
}

UpdateProgress::~UpdateProgress()
{
  delete _mutex;
}

void UpdateProgress::setBounds ( const osg::Vec2f& ul,  const osg::Vec2f& lr )
{
  Guard guard ( *_mutex );
  _ul = ul;
  _lr = lr;
}

void UpdateProgress::drawImplementation ( osg::RenderInfo& info ) const
{
  Guard guard ( *_mutex );

  // Make new vertex array
  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array() );
  vertices->reserve( 4 );
  vertices->push_back ( osg::Vec3f ( _ul.x(), _lr.y(), 0.0 ) );
  vertices->push_back ( osg::Vec3f ( _lr.x(), _lr.y(), 0.0 ) );
  vertices->push_back ( osg::Vec3f ( _lr.x(), _ul.y(), 0.0 ) );
  vertices->push_back ( osg::Vec3f ( _ul.x(), _ul.y(), 0.0 ) );

  // Const cast voodoo
  ( const_cast < UpdateProgress * > ( this ) )->setVertexArray ( vertices.get() );
  ( const_cast < UpdateProgress * > ( this ) )->dirtyDisplayList ();

  BaseClass::drawImplementation ( info );
}

