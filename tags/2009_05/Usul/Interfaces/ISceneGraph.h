
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interfaces for working with a scene graph.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_SCENE_GRAPH_H_
#define _USUL_INTERFACES_SCENE_GRAPH_H_

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Matrix44.h"

namespace Usul {
namespace Interfaces {
namespace SceneGraph {


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs for this namespace.
//
///////////////////////////////////////////////////////////////////////////////

typedef Usul::Interfaces::IUnknown IUnknown;


///////////////////////////////////////////////////////////////////////////////
//
//  Named object.
//
///////////////////////////////////////////////////////////////////////////////

struct INamedObject : public IUnknown
{
  USUL_DECLARE_QUERY_POINTERS ( INamedObject );
  enum { IID = 4107396073u };

  virtual std::string       getObjectName() const = 0;
  virtual void              setObjectName ( const std::string & ) = 0;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Access to user-data.
//
///////////////////////////////////////////////////////////////////////////////

struct IUserData : public IUnknown
{
  USUL_DECLARE_QUERY_POINTERS ( IUserData );
  enum { IID = 2589423703u };

  virtual IUnknown::RefPtr  getUserData() const = 0;
  virtual void              setUserData ( IUnknown::RefPtr ) = 0;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Factory object.
//
///////////////////////////////////////////////////////////////////////////////

struct IFactory : public IUnknown
{
  USUL_DECLARE_QUERY_POINTERS ( IFactory );
  enum { IID = 4016132368u };

  virtual IUnknown::RefPtr  createObject ( unsigned long iid ) const = 0;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Child interface.
//
///////////////////////////////////////////////////////////////////////////////

struct IChild : public IUnknown
{
  USUL_DECLARE_QUERY_POINTERS ( IChild );
  enum { IID = 1212327434u };

  virtual unsigned int      getNumParents() const = 0;
  virtual IUnknown::RefPtr  getParent ( unsigned int ) const = 0;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Group interface.
//
///////////////////////////////////////////////////////////////////////////////

struct IGroup : public IUnknown
{
  USUL_DECLARE_QUERY_POINTERS ( IGroup );
  enum { IID = 1879448713u };

  virtual void              appendChild ( IUnknown::RefPtr ) = 0;

  virtual unsigned int      findChild ( IUnknown::RefPtr ) const = 0;

  virtual IUnknown::RefPtr  getChild ( unsigned int ) const = 0;

  virtual void              insertChild ( unsigned int, IUnknown::RefPtr ) = 0;

  virtual void              prependChild ( IUnknown::RefPtr ) = 0;

  virtual void              removeChild ( unsigned int ) = 0;
  virtual void              removeAllChildren() = 0;

  virtual unsigned int      getNumChildren() const = 0;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Transform group interface.
//
///////////////////////////////////////////////////////////////////////////////

struct ITransformGroup : public IUnknown
{
  USUL_DECLARE_QUERY_POINTERS ( ITransformGroup );
  enum { IID = 1288243410u };
};


///////////////////////////////////////////////////////////////////////////////
//
//  Clipped group interface.
//
///////////////////////////////////////////////////////////////////////////////

struct IClippedGroup : public IUnknown
{
  USUL_DECLARE_QUERY_POINTERS ( IClippedGroup );
  enum { IID = 1140845390u };
};


///////////////////////////////////////////////////////////////////////////////
//
//  Transform interface.
//
///////////////////////////////////////////////////////////////////////////////

struct ITransformMatrix : public IUnknown
{
  typedef Usul::Math::Matrix44d Matrix;

  USUL_DECLARE_QUERY_POINTERS ( ITransformMatrix );
  enum { IID = 1273118550u };

  virtual Matrix            getTransformMatrix() const = 0;
  virtual void              setTransformMatrix ( const Matrix & ) = 0;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Navigation matrix interface.
//
///////////////////////////////////////////////////////////////////////////////

struct INavigationMatrix : public IUnknown
{
  typedef Usul::Math::Matrix44d Matrix;

  USUL_DECLARE_QUERY_POINTERS ( INavigationMatrix );
  enum { IID = 2158510630u };

  virtual Matrix            getNavigationMatrix() const = 0;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Get matrix to view the node.
//
///////////////////////////////////////////////////////////////////////////////

struct IViewAllMatrix : public IUnknown
{
  typedef Usul::Math::Matrix44d Matrix;

  USUL_DECLARE_QUERY_POINTERS ( IViewAllMatrix );
  enum { IID = 1871371997u };

  virtual Matrix            getViewAllMatrix ( double zScale = 3 ) const = 0;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Projection interface.
//
///////////////////////////////////////////////////////////////////////////////

struct IProjectionMatrix : public IUnknown
{
  typedef Usul::Math::Matrix44d Matrix;

  USUL_DECLARE_QUERY_POINTERS ( IProjectionMatrix );
  enum { IID = 4176345966u };

  virtual Matrix            getProjectionMatrix() const = 0;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Perspective projection interface.
//
///////////////////////////////////////////////////////////////////////////////

struct IPerspectiveProjection : public IUnknown
{
  USUL_DECLARE_QUERY_POINTERS ( IPerspectiveProjection );
  enum { IID = 1653183730u };

  virtual void              getPerspectiveProjection ( double &fieldOfViewDegrees, 
                                                       double &aspectRatio, 
                                                       double &zNear, 
                                                       double &zFar ) const = 0;

  virtual void              setPerspectiveProjection ( double fieldOfViewDegrees, 
                                                       double aspectRatio, 
                                                       double zNear, 
                                                       double zFar ) = 0;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Set the viewport.
//
///////////////////////////////////////////////////////////////////////////////

struct IViewportSet : public IUnknown
{
  USUL_DECLARE_QUERY_POINTERS ( IViewportSet );
  enum { IID = 2282351597u };

  virtual void              setViewport ( double width, double height ) = 0;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewport.
//
///////////////////////////////////////////////////////////////////////////////

struct IViewportGet : public IUnknown
{
  USUL_DECLARE_QUERY_POINTERS ( IViewportGet );
  enum { IID = 4275607030u };

  virtual void              getViewport ( double &width, double &height ) const = 0;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Render interface.
//
///////////////////////////////////////////////////////////////////////////////

struct IRender : public IUnknown
{
  USUL_DECLARE_QUERY_POINTERS ( IRender );
  enum { IID = 4203951770u };

  virtual void              render ( IUnknown::RefPtr projection, IUnknown::RefPtr scene ) = 0;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-render interface.
//
///////////////////////////////////////////////////////////////////////////////

struct IPreRender : public IUnknown
{
  USUL_DECLARE_QUERY_POINTERS ( IPreRender );
  enum { IID = 2903982808u };

  virtual void              preRender ( IUnknown::RefPtr projection, IUnknown::RefPtr scene ) = 0;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Post-render interface.
//
///////////////////////////////////////////////////////////////////////////////

struct IPostRender : public IUnknown
{
  USUL_DECLARE_QUERY_POINTERS ( IPostRender );
  enum { IID = 3994375470u };

  virtual void              postRender ( IUnknown::RefPtr projection, IUnknown::RefPtr scene ) = 0;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Get the OpenGL context.
//
///////////////////////////////////////////////////////////////////////////////

struct IOpenGLContextGet : public IUnknown
{
  USUL_DECLARE_QUERY_POINTERS ( IOpenGLContextGet );
  enum { IID = 4107159078u };

  virtual IUnknown::RefPtr  getOpenGLContext() const = 0;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Set the OpenGL context.
//
///////////////////////////////////////////////////////////////////////////////

struct IOpenGLContextSet : public IUnknown
{
  USUL_DECLARE_QUERY_POINTERS ( IOpenGLContextGet );
  enum { IID = 2890418586u };

  virtual void              setOpenGLContext ( IUnknown::RefPtr ) = 0;
};


} // End namespace SceneGraph
} // End namespace Interfaces
} // End namespace Usul


#endif // _USUL_INTERFACES_SCENE_GRAPH_H_
