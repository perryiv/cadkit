
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Abstract base class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_OSG_OBJECT_CLASS_H_
#define _SCENE_GRAPH_OSG_OBJECT_CLASS_H_

#include "Usul/Base/BaseObject.h"
#include "Usul/Interfaces/ISceneGraph.h"

#include <string>


namespace SceneGraph {
namespace OSG {


class Object : 
  public Usul::Base::BaseObject,
  public Usul::Interfaces::SceneGraph::INamedObject,
  public Usul::Interfaces::SceneGraph::IUserData
{
public:

  // Typedefs.
  typedef Usul::Base::BaseObject BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::SceneGraph::INamedObject INamedObject;
  typedef Usul::Interfaces::SceneGraph::IUserData IUserData;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Object );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Usul::Interfaces::SceneGraph::INamedObject
  virtual std::string           getObjectName() const;
  virtual void                  setObjectName ( const std::string & );

  // Usul::Interfaces::SceneGraph::IUserData
  virtual IUnknown::RefPtr      getUserData() const;
  virtual void                  setUserData ( IUnknown::RefPtr );

protected:

  // Default construction.
  Object();

  // Use reference counting.
  virtual ~Object();

  void                          _destroy();

private:

  // Do not copy.
  Object ( const Object & );
  Object &operator = ( const Object & );

  std::string _name;
  IUnknown::RefPtr _userData;
};


} // namespace OSG
} // namespace SceneGraph


#endif // _SCENE_GRAPH_OSG_OBJECT_CLASS_H_
