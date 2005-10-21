
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Trackball manipulator.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OPEN_SCENE_GRAPH_TOOLS_TRACKBALL_MANIPULATOR_H_
#define _OPEN_SCENE_GRAPH_TOOLS_TRACKBALL_MANIPULATOR_H_

#include "OsgTools/Export.h"

#include "osgGA/TrackballManipulator"

namespace osgGA { class GUIEventAdapter; class GUIActionAdapter; }


namespace OsgTools {
namespace Render {


class OSG_TOOLS_EXPORT Trackball : public osgGA::TrackballManipulator
{
public:

  // Useful typedefs.
  typedef osgGA::TrackballManipulator BaseClass;
  typedef osg::ref_ptr<Trackball> Ptr;
  typedef osg::Vec3d Vec3;

  // Constructor.
  Trackball();
  Trackball ( const Trackball &, const osg::CopyOp & );

  // Plumbing.
  META_Object ( OsgFox, Trackball );

  // Set/get the center.
  void                    center ( const osg::Vec3 &c ) { _center = c; }
  const Vec3 &            center() const { return _center; }

  // Set/get the distance.
  void                    distance ( float d ) { _distance = d; }
  float                   distance() const { return _distance; }

  // Get the inverse matrix of the manipulator.
  virtual osg::Matrixd    getInverseMatrix() const;

  // Handle events.
  virtual bool            handle ( const osgGA::GUIEventAdapter &, osgGA::GUIActionAdapter & );

  // Move the camera to the default position. 
  virtual void            home ( const osgGA::GUIEventAdapter &, osgGA::GUIActionAdapter & );

  // See if the geometry was "thrown". (This is not about C++ exceptions.)
  bool                    thrown() const { return _thrown; }

  // Set/get the rotation.
  void                    rotation ( const osg::Quat &r ) { _rotation = r; }
  const osg::Quat &       rotation() const { return _rotation; }

protected:

  // Use reference counting.
  virtual ~Trackball();
};


} // namespace Render
} // namespace OsgTools


#endif // _OPEN_SCENE_GRAPH_TOOLS_TRACKBALL_MANIPULATOR_H_
