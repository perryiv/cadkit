
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Authors: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Widgets/Helper/ThreadSafeText.h"

#include "osg/NodeVisitor"

using namespace OsgTools::Widgets::Helper;


///////////////////////////////////////////////////////////////////////////////
//
//  Callback.
//
///////////////////////////////////////////////////////////////////////////////

UpdateTextCallback::UpdateTextCallback ( ) : 
  BaseClass(),
  _mutex ( 0x0 ),
  _dirty ( false ),
  _text ( "" ),
  _sceneText ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the text.
//
///////////////////////////////////////////////////////////////////////////////

void UpdateTextCallback::update ( osg::NodeVisitor *nv, osg::Drawable* )
{ 
  Guard guard ( *_mutex );

  // Only update if it's an update visitor.
  if( nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR )
  {
    if( true == _dirty && 0x0 != _sceneText )
    {
      _sceneText->setText ( _text );
      _sceneText->update ();
      _dirty = false;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the text.
//
///////////////////////////////////////////////////////////////////////////////

void UpdateTextCallback::text ( const std::string& text )
{
  Guard guard ( *_mutex );
  _text = text;
  
  _dirty = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the text drawable.
//
///////////////////////////////////////////////////////////////////////////////

void UpdateTextCallback::text ( osgText::Text * text )
{
  Guard guard ( *_mutex );
  _sceneText = text;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the mutex.
//
///////////////////////////////////////////////////////////////////////////////

void UpdateTextCallback::mutex ( Mutex* mutex )
{
  _mutex = mutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ThreadSafeText::ThreadSafeText () : BaseClass (),
  _mutex ( Mutex::create() ),
  _updateCallback ( new UpdateTextCallback )
{
  _updateCallback->mutex ( _mutex );
  _updateCallback->text ( this );
  this->setUpdateCallback ( _updateCallback.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ThreadSafeText::~ThreadSafeText ()
{
  // Make sure we unreference the update callback before deleting the mutex.
  this->setUpdateCallback( 0x0 );
  _updateCallback = 0x0;

  delete _mutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Call the base class' draw.  Lock the mutex.
//
///////////////////////////////////////////////////////////////////////////////

#if OSG_VERSION_MAJOR <= 1 && OSG_VERSION_MINOR <= 2
void ThreadSafeText::drawImplementation ( osg::State & state ) const
#else
void ThreadSafeText::drawImplementation ( osg::RenderInfo& info ) const
#endif
{
  Guard guard ( *_mutex );

#if OSG_VERSION_MAJOR <= 1 && OSG_VERSION_MINOR <= 2
  BaseClass::drawImplementation ( state );
#else
  BaseClass::drawImplementation ( info );
#endif
}



///////////////////////////////////////////////////////////////////////////////
//
//  Set the text.
//
///////////////////////////////////////////////////////////////////////////////

void ThreadSafeText::setText ( const std::string& text )
{
  _updateCallback->text ( text );
}