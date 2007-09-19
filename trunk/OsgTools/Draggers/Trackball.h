
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  2D translate dragger.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_DRAG_TRACKBALL_DRAGGER_CLASS_H_
#define _OSG_TOOLS_DRAG_TRACKBALL_DRAGGER_CLASS_H_

#include "OsgTools/Draggers/Translate.h"

#include "Usul/Interfaces/IViewport.h"

#include "osg/Vec2d"


namespace OsgTools {
namespace Draggers {


class OSG_TOOLS_EXPORT Trackball : public Translate
{
public :

  // Useful typedefs
  typedef Translate BaseClass;
  typedef osg::ref_ptr<Trackball> Ptr;

  // Constructors.
  Trackball( Usul::Interfaces::IUnknown *caller );
  Trackball ( const Trackball &, const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY );

  // Do not use.
  Trackball () { }

  // Plumbing.
  META_Node ( OsgTools, Trackball );

  // Execute the command.
  virtual void      execute ( Command c, const osg::Vec3d &pt0, const osg::Vec3d &pt1 );

  // File I/O.
  static bool       read  (       osg::Object &, osgDB::Input  & );
  static bool       write ( const osg::Object &, osgDB::Output & );

protected :

  // Use reference counting.
  virtual ~Trackball();

  bool              _closest ( const osg::Vec3d &pt0, const osg::Vec3d &pt1, osg::Vec3d &point );
  void              _common  ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 );
  void              _finish  ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 );
  void              _move    ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 );
  void              _start   ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 );

  void trackball(osg::Vec3& axis,float& angle, float p1x, float p1y, float p2x, float p2y);
  float tb_project_to_sphere(float r, float x, float y);

private:

  osg::Vec3d _diff;
  float _trackballSize;
  Usul::Interfaces::IViewport::QueryPtr _viewer;

  osg::Vec3 _v0;
};


}; // namespace Draggers
}; // namespace OsgTools


#endif // _OSG_TOOLS_DRAG_TRACKBALL_DRAGGER_CLASS_H_
