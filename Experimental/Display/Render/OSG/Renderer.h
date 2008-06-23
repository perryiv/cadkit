
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
//  Open scene graph renderer.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DISPLAY_LIBRARY_OSG_RENDERER_CLASS_H_
#define _DISPLAY_LIBRARY_OSG_RENDERER_CLASS_H_

#include "Display/Render/Renderer.h"


namespace Display {
namespace Render {
namespace OSG {


class DISPLAY_LIBRARY_EXPORT Renderer : public Display::Render::Renderer
{
public:

  // Typedefs.
  typedef Display::Render::Renderer BaseClass;
  typedef BaseClass::Mutex Mutex;
  typedef BaseClass::Guard Guard;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Renderer );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Renderer );

  // Construction.
  Renderer();

protected:

  // Use reference counting.
  virtual ~Renderer();

private:

  // No copying or assignment.
  Renderer ( const Renderer & );
  Renderer &operator = ( const Renderer & );

  void                      _destroy();
};


} // namespace OSG
} // namespace Render
} // namespace Display


#endif // _DISPLAY_LIBRARY_OSG_RENDERER_CLASS_H_
