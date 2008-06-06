
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  1D translate dragger.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_DRAG_TRANSLATE_1_DRAGGER_CLASS_H_
#define _OSG_TOOLS_DRAG_TRANSLATE_1_DRAGGER_CLASS_H_

#include "OsgTools/Draggers/Translate.h"


namespace OsgTools {
namespace Draggers {


class OSG_TOOLS_EXPORT Translate1 : public Translate
{
public :

  // Useful typedefs
  typedef Translate BaseClass;
  typedef osg::ref_ptr<Translate1> Ptr;

  enum Plane
  {
    X,
    Y,
    Z
  };

  // Constructors.
  Translate1();
  Translate1( Plane p );
  Translate1 ( const Translate1 &, const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY );

  // Plumbing.
  META_Node ( OsgTools, Translate1 );

  // Execute the command.
  virtual void      execute ( Command c, const osg::Vec3d &pt0, const osg::Vec3d &pt1 );

  void              plane( Plane p ) { _plane = p; }

  // File I/O.
  static bool       read  (       osg::Object &, osgDB::Input  & );
  static bool       write ( const osg::Object &, osgDB::Output & );

protected :  

  // Use reference counting.
  virtual ~Translate1();

  bool              _closest ( const osg::Vec3d &pt0, const osg::Vec3d &pt1, osg::Vec3d &point );
  void              _common  ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 );
  void              _finish  ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 );
  void              _move    ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 );
  void              _start   ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 );

private:

  Plane _plane;
  osg::Vec3d _diff;
};


}; // namespace Draggers
}; // namespace OsgTools


#endif // _OSG_TOOLS_DRAG_TRANSLATE_1_DRAGGER_CLASS_H_
