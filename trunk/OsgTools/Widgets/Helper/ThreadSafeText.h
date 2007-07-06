
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Authors: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_WIDGETS_UPDATE_TEXT_CALLBACK_H__
#define __OSG_TOOLS_WIDGETS_UPDATE_TEXT_CALLBACK_H__

#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"

#include "osg/Drawable"
#include "osg/Version"


#include "osgText/Text"

#include <string>

namespace OsgTools {
namespace Widgets {
namespace Helper {

///////////////////////////////////////////////////////////////////////////////
//
//  Callback to set the text of an osgText::Text during an update traversal.
//
///////////////////////////////////////////////////////////////////////////////

class UpdateTextCallback : public osg::Drawable::UpdateCallback
{
public:
  typedef osg::Drawable::UpdateCallback   BaseClass;
  typedef Usul::Threads::Mutex            Mutex;
  typedef Usul::Threads::Guard < Mutex >  Guard;

  UpdateTextCallback ( );

  virtual void update ( osg::NodeVisitor *nv, osg::Drawable* );

  /// Set the text.
  void text ( const std::string& text );

  /// Set the text drawable.
  void text ( osgText::Text * text );

  /// Set the mutex.
  void mutex ( Mutex* mutex );

private:
  Mutex       *_mutex;
  bool         _dirty;
  std::string  _text;

  // Raw pointer to avoid a circular reference.
  osgText::Text *_sceneText;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Class to update the text in a thread safe way.
//
///////////////////////////////////////////////////////////////////////////////

class ThreadSafeText : public osgText::Text
{
public:
  typedef osgText::Text                   BaseClass;
  typedef Usul::Threads::Mutex            Mutex;
  typedef Usul::Threads::Guard < Mutex >  Guard;

  ThreadSafeText ();

#if OSG_VERSION_MAJOR <= 1 && OSG_VERSION_MINOR <= 2
  virtual void drawImplementation ( osg::State& state ) const;
#else
  virtual void drawImplementation ( osg::RenderInfo& info ) const;
#endif

  void setText ( const std::string& text );

protected:
  virtual ~ThreadSafeText ();

private:
  Mutex *_mutex;
  osg::ref_ptr < UpdateTextCallback > _updateCallback;
};

}
}
}

#endif // __OSG_TOOLS_WIDGETS_UPDATE_TEXT_CALLBACK_H__
