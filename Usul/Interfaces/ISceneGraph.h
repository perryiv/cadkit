
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
//  Modifying visitor interface.
//
///////////////////////////////////////////////////////////////////////////////

template < class VisitedType > struct IVisitorModifier : public IUnknown
{
  typedef VisitedType Visited;

  USUL_DECLARE_QUERY_POINTERS ( IVisitorModifier );
  enum { IID = 2061177761u };

  virtual void              visitObject ( Visited * ) = 0;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Modifying visitor interface.
//
///////////////////////////////////////////////////////////////////////////////

template < class VisitedType > struct IVisitorNonModifier : public IUnknown
{
  typedef VisitedType Visited;

  USUL_DECLARE_QUERY_POINTERS ( IVisitorNonModifier );
  enum { IID = 2981621883u };

  virtual void              visitObject ( const Visited * ) = 0;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Object interface.
//
///////////////////////////////////////////////////////////////////////////////

struct IAcceptVisitorModifier : public IUnknown
{
  USUL_DECLARE_QUERY_POINTERS ( IAcceptVisitorModifier );
  enum { IID = 2588866817u };

  virtual void              acceptVisitorModifier ( IUnknown::RefPtr ) = 0;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Object interface.
//
///////////////////////////////////////////////////////////////////////////////

struct IAcceptVisitorNonModifier : public IUnknown
{
  USUL_DECLARE_QUERY_POINTERS ( IAcceptVisitorNonModifier );
  enum { IID = 2058597607u };

  virtual void              acceptVisitorNonModifier ( IUnknown::RefPtr ) const = 0;
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

template < class MatrixType > struct ITransformMatrix : public IUnknown
{
  typedef MatrixType Matrix;

  USUL_DECLARE_QUERY_POINTERS ( ITransformMatrix );
  enum { IID = 1273118550u };

  virtual Matrix            getTransformMatrix() const = 0;
  virtual void              setTransformMatrix ( const Matrix & ) = 0;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Projection interface.
//
///////////////////////////////////////////////////////////////////////////////

template < class MatrixType > struct IProjectionMatrix : public IUnknown
{
  typedef MatrixType Matrix;

  USUL_DECLARE_QUERY_POINTERS ( IProjectionMatrix );
  enum { IID = 4176345966u };

  virtual Matrix            getProjectionMatrix() const = 0;
  virtual void              setProjectionMatrix ( const Matrix & ) = 0;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Perspective projection interface.
//
///////////////////////////////////////////////////////////////////////////////

template < class MatrixType > struct IPerspectiveProjection : public IUnknown
{
  typedef MatrixType Matrix;
  typedef typename Matrix::value_type ValueType;

  USUL_DECLARE_QUERY_POINTERS ( IPerspectiveProjection );
  enum { IID = 1653183730u };

  virtual Matrix            makePerspectiveProjection ( ValueType fieldOfViewDegrees, 
                                                        ValueType aspectRatio, 
                                                        ValueType zNear, 
                                                        ValueType zFar ) const = 0;
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


} // End namespace SceneGraph
} // End namespace Interfaces
} // End namespace Usul


#endif // _USUL_INTERFACES_SCENE_GRAPH_H_
