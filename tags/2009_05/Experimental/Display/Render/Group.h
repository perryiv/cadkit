
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

#include "Display/Export/Export.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/ISceneGraph.h"

#include <vector>


namespace Display {
namespace Render {


class DISPLAY_LIBRARY_EXPORT Group :
  public Usul::Base::Object,
  public Usul::Interfaces::SceneGraph::IPreRender,
  public Usul::Interfaces::SceneGraph::IRender,
  public Usul::Interfaces::SceneGraph::IPostRender
{
public:

  // Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::SceneGraph::IPreRender IPreRender;
  typedef Usul::Interfaces::SceneGraph::IRender IRender;
  typedef Usul::Interfaces::SceneGraph::IPostRender IPostRender;
  typedef std::vector<IUnknown::RefPtr> Renderers;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Group );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Group );

  // Usul::Interfaces::IUnknown
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Usul::Interfaces::SceneGraph::IPostRender
  virtual void              postRender ( IUnknown::RefPtr projection, IUnknown::RefPtr scene );

  // Usul::Interfaces::SceneGraph::IPreRender
  virtual void              preRender ( IUnknown::RefPtr projection, IUnknown::RefPtr scene );

  // Usul::Interfaces::SceneGraph::IRender
  virtual void              render ( IUnknown::RefPtr projection, IUnknown::RefPtr scene );

  // Set the renderers and return the existing ones.
  Renderers                 renderers ( const Renderers &r = Renderers() );

protected:

  // Construction.
  Group();

  // Use reference counting.
  virtual ~Group();

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
