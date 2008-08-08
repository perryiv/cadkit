
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

  // Constructor.
  Trackball();
  Trackball ( const Trackball &, const osg::CopyOp & );

  // Plumbing.
  META_Object ( OsgTools, Trackball );

  // Set/get the center.
  void                    center ( const osg::Vec3 &c );
  osg::Vec3               center() const;

  // Set/get the distance.
  void                    distance ( double d );
  double                  distance() const;

  // Get the inverse matrix of the manipulator.
  virtual osg::Matrixd    getInverseMatrix() const;

  // Calculate the matrix for the manipulator.
  static osg::Matrixd     matrix ( const osg::Vec3d &center, const osg::Quat &rotation, double distance );

  // Handle events.
  virtual bool            handle ( const osgGA::GUIEventAdapter &, osgGA::GUIActionAdapter & );

  // Move the camera to the default position. 
  virtual void            home ( const osgGA::GUIEventAdapter &, osgGA::GUIActionAdapter & );

  // Get/set the trackball's minimum zoom scale.
  double                  minimumZoomScale() const;
  void                    minimumZoomScale ( double mn );

  // Get/set the model's scale.
  double                  modelScale() const;
  void                    modelScale ( double ms );

  // See if the geometry was "thrown". (This is not about C++ exceptions.)
  bool                    thrown() const { return _thrown; }

  // Set/get the rotation.
  void                    rotation ( const osg::Quat &r );
  osg::Quat               rotation() const;

protected:

  // Use reference counting.
  virtual ~Trackball();
};


} // namespace Render
} // namespace OsgTools


#endif // _OPEN_SCENE_GRAPH_TOOLS_TRACKBALL_MANIPULATOR_H_
