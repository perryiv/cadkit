
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The group class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_OSG_GROUP_CLASS_H_
#define _SCENE_GRAPH_OSG_GROUP_CLASS_H_

#include "SceneGraph/OpenSceneGraph/Node.h"

#include "osg/Group"
#include "osg/ref_ptr"


namespace SceneGraph {
namespace OSG {


class Group : 
  public SceneGraph::OSG::Node,
  public Usul::Interfaces::SceneGraph::IGroup
{
public:

  // Typedefs.
  typedef SceneGraph::OSG::Node BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::SceneGraph::IGroup IGroup;
  typedef osg::ref_ptr<osg::Group> GroupPtr;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Group );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Construction.
  Group ( GroupPtr );

  // Usul::Interfaces::SceneGraph::IGroup
  virtual void                  appendChild ( IUnknown::RefPtr );
  virtual unsigned int          findChild ( IUnknown::RefPtr ) const;
  virtual IUnknown::RefPtr      getChild ( unsigned int ) const;
  virtual void                  insertChild ( unsigned int, IUnknown::RefPtr );
  virtual void                  prependChild ( IUnknown::RefPtr );
  virtual void                  removeChild ( unsigned int );
  virtual void                  removeAllChildren();
  virtual unsigned int          getNumChildren() const;

protected:

  // Use reference counting.
  virtual ~Group();

  void                          _destroy();

private:

  // Do not copy.
  Group ( const Group & );
  Group &operator = ( const Group & );

  GroupPtr _group;
};


} // namespace OSG
} // namespace SceneGraph


#endif // _SCENE_GRAPH_OSG_GROUP_CLASS_H_
