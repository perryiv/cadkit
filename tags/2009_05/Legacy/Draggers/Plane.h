
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Dragger to use with planes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_DRAG_PLANE_CLASS_H_
#define _OSG_TOOLS_DRAG_PLANE_CLASS_H_

#include "OsgTools/Draggers/Dragger.h"

#include "OsgTools/Draggers/Rotate1.h"
#include "OsgTools/Draggers/Translate1.h"

namespace OsgTools {
namespace Draggers {


class OSG_TOOLS_EXPORT Plane : public Dragger
{
public:

  // Useful typedefs
  typedef Dragger BaseClass;
  typedef osg::ref_ptr<Plane> Ptr;

  // Constructors.
  Plane();
  Plane ( const Plane &, const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY );

  // Plumbing.
  META_Node ( OsgTools, Plane );

  // Execute the command.
  virtual void          execute ( Command c, const osg::Vec3d &pt0, const osg::Vec3d &pt1 );

  // File I/O.
  static bool           read  (       osg::Object &, osgDB::Input  & );
  static bool           write ( const osg::Object &, osgDB::Output & );

  void                      geometryX ( State s, osg::Node *n );

  void                      geometryY ( State s, osg::Node *n );

  void                      geometryZ ( State s, osg::Node *n );

protected:

  // Local callback class.
  struct LocalCallback : public OsgTools::Draggers::Callback
  {
    typedef OsgTools::Draggers::Callback BaseClass;
    LocalCallback ( Plane &d, Dragger::Command c );
    virtual void operator() ( Dragger &d );
  protected:
    friend class Translate3;
    LocalCallback ( const LocalCallback & );
    virtual ~LocalCallback();
  private:
    Plane &_d;
    Dragger::Command _c;
  };

  // Use reference counting.
  virtual ~Plane();

  void              _adjust ( Dragger &d );

  static void       _setX ( double x, osg::MatrixTransform &mt );
  static void       _setY ( double y, osg::MatrixTransform &mt );
  static void       _setZ ( double z, osg::MatrixTransform &mt );


private:

  osg::ref_ptr<osg::MatrixTransform> _rx;
  osg::ref_ptr<osg::MatrixTransform> _ry;
  osg::ref_ptr<osg::MatrixTransform> _rz;
  osg::ref_ptr < Rotate1 > _x;
  osg::ref_ptr < Rotate1 > _y;
  osg::ref_ptr < Translate1 > _z;
};


}; // namespace Draggers
}; // namespace OsgTools


#endif // _OSG_TOOLS_DRAG_PLANE_CLASS_H_
