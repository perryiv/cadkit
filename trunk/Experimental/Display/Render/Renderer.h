
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
//  Base class for all renderers.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DISPLAY_LIBRARY_RENDERER_CLASS_H_
#define _DISPLAY_LIBRARY_RENDERER_CLASS_H_

#include "Display/Export/Export.h"

#include "Usul/Base/Object.h"

namespace osg { class Node; }


namespace Display {
namespace Render {


class DISPLAY_LIBRARY_EXPORT Renderer : public Usul::Base::Object
{
public:

  // Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef BaseClass::Mutex Mutex;
  typedef BaseClass::Guard Guard;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Renderer );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Renderer );

  // Render the scene.
  void                      render ( osg::Node * );

protected:

  // Construction.
  Renderer();

  // Use reference counting.
  virtual ~Renderer();

  // Pre/post render notifications.
  virtual void              _preRender();
  virtual void              _postRender();

  // Render the scene.
  virtual void              _render ( osg::Node * );

private:

  // No copying or assignment.
  Renderer ( const Renderer & );
  Renderer &operator = ( const Renderer & );

  void                      _destroy();
};


} // namespace Render
} // namespace Display


#endif // _DISPLAY_LIBRARY_RENDERER_CLASS_H_
