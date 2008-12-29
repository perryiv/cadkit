
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Group of renderers.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DISPLAY_LIBRARY_RENDERER_GROUP_CLASS_H_
#define _DISPLAY_LIBRARY_RENDERER_GROUP_CLASS_H_

#include "Display/Render/Renderer.h"

#include <vector>


namespace Display {
namespace Render {


class DISPLAY_LIBRARY_EXPORT Group : public Display::Render::Renderer
{
public:

  // Typedefs.
  typedef Display::Render::Renderer BaseClass;
  typedef std::vector<BaseClass::RefPtr> Renderers;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Group );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Group );

  // Call this when you want the viewport to resize.
  virtual void              resize ( unsigned int width, unsigned int height );

  // Set the scene.
  virtual void              scene ( NodePtr );

protected:

  // Construction.
  Group();

  // Use reference counting.
  virtual ~Group();

  // Pre/post render notifications.
  virtual void              _preRender();
  virtual void              _postRender();

  // Render the scene.
  virtual void              _render();

private:

  // No copying or assignment.
  Group ( const Group & );
  Group &operator = ( const Group & );

  void                      _destroy();

  Renderers _renderers;
};


} // namespace Render
} // namespace Display


#endif // _DISPLAY_LIBRARY_RENDERER_GROUP_CLASS_H_
