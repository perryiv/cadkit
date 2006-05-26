
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_RENDER_SCENE_MANAGER_H__
#define __OSG_TOOLS_RENDER_SCENE_MANAGER_H__

#include "OsgTools/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include "osg/ref_ptr"
#include "osg/Group"
#include "osg/ClipNode"
#include "osg/Projection"

namespace OsgTools {
namespace Render {

class OSG_TOOLS_EXPORT SceneManager : public Usul::Base::Referenced
{
public:
  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( SceneManager );

  // Useful typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef osg::Group Group;

  SceneManager();

  /// Get the scene
  osg::Node * scene();
  const osg::Node * scene() const;

  /// Set the Model
  void model( osg::Node * );

  /// Get the model
  osg::Node * model();
  const osg::Node * model () const;

  // Add/Remove group from scene
  osg::Group*           groupGet    ( const std::string& );
  void                  groupRemove ( const std::string& );
  bool                  groupHas    ( const std::string& ) const;

  /// Add/Remove group from projection node
  osg::Group*           projectionGroupGet    ( const std::string& );
  void                  projectionGroupRemove ( const std::string& );
  bool                  projectionGroupHas    ( const std::string& ) const;

  /// Get the projection node
  osg::Projection*      projection();

  /// Get the clip node
  osg::ClipNode*        clipNode();
protected:

  /// Use reference counting.
  ~SceneManager();

private:

  typedef osg::ref_ptr<Group> GroupPtr;
  typedef osg::ref_ptr < osg::ClipNode > ClipNodePtr;
  typedef osg::ref_ptr < osg::Projection > ProjectionPtr;

  typedef std::map < std::string, GroupPtr > GroupMap;

  GroupPtr _scene;
  ClipNodePtr _clipNode;
  ProjectionPtr _projectionNode;

  GroupMap _groupMap;
  GroupMap _projectionMap;
};


}
}

#endif // __OSG_TOOLS_RENDER_SCENE_MANAGER_H__


