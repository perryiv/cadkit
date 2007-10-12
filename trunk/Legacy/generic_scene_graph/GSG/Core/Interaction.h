
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interaction functors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_INTERACTION_FUNCTORS_H_
#define _GENERIC_SCENE_GRAPH_CORE_INTERACTION_FUNCTORS_H_

#include "GSG/Core/Referenced.h"


namespace GSG {

class Viewer;


///////////////////////////////////////////////////////////////////////////////
//
//  Base interaction functor.
//
///////////////////////////////////////////////////////////////////////////////

struct GSG_CORE_EXPORT Interaction : public Referenced
{
  GSG_DECLARE_LOCAL_TYPEDEFS ( Interaction, Referenced );
  virtual void operator () ( Viewer &v ) = 0;

protected:

  explicit Interaction();
  Interaction ( const Interaction & );
  virtual ~Interaction();
};


///////////////////////////////////////////////////////////////////////////////
//
//  Trackball-rotation functor.
//
///////////////////////////////////////////////////////////////////////////////

struct GSG_CORE_EXPORT Trackball : public Interaction
{
  GSG_DECLARE_REFERENCED ( Trackball );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Trackball, Interaction );

  explicit Trackball();
  Trackball ( const Trackball & );
  virtual void operator () ( Viewer &v );

protected:

  virtual ~Trackball();
};


///////////////////////////////////////////////////////////////////////////////
//
//  Translation-interaction functor.
//
///////////////////////////////////////////////////////////////////////////////

struct GSG_CORE_EXPORT Translate : public Interaction
{
  GSG_DECLARE_REFERENCED ( Translate );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Translate, Interaction );

  explicit Translate();
  Translate ( const Translate & );
  virtual void operator () ( Viewer &v );

protected:

  virtual ~Translate();
};


///////////////////////////////////////////////////////////////////////////////
//
//  Scale-interaction functor.
//
///////////////////////////////////////////////////////////////////////////////

struct GSG_CORE_EXPORT Scale : public Interaction
{
  GSG_DECLARE_REFERENCED ( Scale );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Scale, Interaction );

  explicit Scale();
  Scale ( const Scale & );
  virtual void operator () ( Viewer &v );

protected:

  virtual ~Scale();
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_INTERACTION_FUNCTORS_H_
