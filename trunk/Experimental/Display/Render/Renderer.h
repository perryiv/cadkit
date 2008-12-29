
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

#include "Usul/Base/BaseObject.h"

#include "osg/Node"
#include "osg/ref_ptr"


namespace Display {
namespace Render {


class DISPLAY_LIBRARY_EXPORT Renderer : public Usul::Base::BaseObject
{
public:

  // Typedefs.
  typedef Usul::Base::BaseObject BaseClass;
  typedef osg::ref_ptr<osg::Node> NodePtr;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Renderer );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Renderer );

  // Render the scene.
  void                      render();

  // Call this when you want the viewport to resize.
  virtual void              resize ( unsigned int width, unsigned int height );

  // Set the scene.
  virtual void              scene ( NodePtr );

protected:

  // Construction.
  Renderer();

  // Use reference counting.
  virtual ~Renderer();

  NodePtr                   _getScene();
  const NodePtr             _getScene() const;

  virtual void              _preRender();
  virtual void              _postRender();

  virtual void              _render();

private:

  // No copying or assignment.
  Renderer ( const Renderer & );
  Renderer &operator = ( const Renderer & );

  void                      _destroy();

  NodePtr _scene;
};


} // namespace Render
} // namespace Display


#endif // _DISPLAY_LIBRARY_RENDERER_CLASS_H_
