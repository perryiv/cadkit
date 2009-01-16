
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
#include "Usul/Interfaces/ISceneGraph.h"
#include "Usul/Math/Matrix44.h"


namespace Display {
namespace Render {


class DISPLAY_LIBRARY_EXPORT Renderer : public Usul::Base::BaseObject
{
public:

  // Typedefs.
  typedef Usul::Base::BaseObject BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Math::Matrix44d Matrix;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Renderer );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Renderer );

  // Render the scene.
  void                      render ( IUnknown::RefPtr projection, IUnknown::RefPtr scene );

protected:

  // Construction.
  Renderer();

  // Use reference counting.
  virtual ~Renderer();

  virtual void              _preRender ( IUnknown::RefPtr scene );
  virtual void              _postRender ( IUnknown::RefPtr scene );

  virtual void              _render ( IUnknown::RefPtr scene );

private:

  // No copying or assignment.
  Renderer ( const Renderer & );
  Renderer &operator = ( const Renderer & );

  void                      _destroy();
};


} // namespace Render
} // namespace Display


#endif // _DISPLAY_LIBRARY_RENDERER_CLASS_H_
