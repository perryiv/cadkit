
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  1D rotate dragger.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_DRAG_ROTATE_1_DRAGGER_CLASS_H_
#define _OSG_TOOLS_DRAG_ROTATE_1_DRAGGER_CLASS_H_

#include "OsgTools/Draggers/Dragger.h"


namespace OsgTools {
namespace Draggers {


class OSG_TOOLS_EXPORT Rotate1 : public Dragger
{
public :

  // Useful typedefs
  typedef Dragger BaseClass;
  typedef osg::ref_ptr<Rotate1> Ptr;

  // Constructors.
  Rotate1();
  Rotate1( const osg::Vec3d& axis );
  Rotate1 ( const Rotate1 &, const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY );

  // Plumbing.
  META_Node ( OsgTools, Rotate1 );

  // Execute the command.
  virtual void      execute ( Command c, const osg::Vec3d &pt0, const osg::Vec3d &pt1 );

  // File I/O.
  static bool       read  (       osg::Object &, osgDB::Input  & );
  static bool       write ( const osg::Object &, osgDB::Output & );

protected :  

  // Use reference counting.
  virtual ~Rotate1();

  bool              _closest ( const osg::Vec3d &pt0, const osg::Vec3d &pt1, osg::Vec3d &point );
  void              _common  ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 );
  void              _finish  ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 );
  void              _move    ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 );
  void              _start   ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 );

private:

  osg::Vec3d _axis;
  osg::Vec3d _diff;
};


}; // namespace Draggers
}; // namespace OsgTools


#endif // _OSG_TOOLS_DRAG_ROTATE_1_DRAGGER_CLASS_H_
