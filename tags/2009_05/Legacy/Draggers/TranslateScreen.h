
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Screen translate dragger.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_DRAG_TRANSLATE_SCREEN_DRAGGER_CLASS_H_
#define _OSG_TOOLS_DRAG_TRANSLATE_SCREEN_DRAGGER_CLASS_H_

#include "OsgTools/Draggers/Translate.h"

#include "osg/Vec2d"


namespace OsgTools {
namespace Draggers {


class OSG_TOOLS_EXPORT TranslateScreen : public Translate
{
public :

  // Useful typedefs
  typedef Translate BaseClass;
  typedef osg::ref_ptr<TranslateScreen> Ptr;

  // Constructors.
  TranslateScreen();
  TranslateScreen ( const TranslateScreen &, const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY );

  // Plumbing.
  META_Node ( OsgTools, TranslateScreen );

  // Execute the command.
  virtual void      execute ( Command c, const osg::Vec3d &pt0, const osg::Vec3d &pt1 );

  // File I/O.
  static bool       read  (       osg::Object &, osgDB::Input  & );
  static bool       write ( const osg::Object &, osgDB::Output & );

protected :

  // Use reference counting.
  virtual ~TranslateScreen();

  bool              _closest ( const osg::Vec3d &pt0, const osg::Vec3d &pt1, osg::Vec3d &point );
  void              _common  ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 );
  void              _finish  ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 );
  void              _move    ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 );
  void              _start   ( const osg::Vec3d &pt0, const osg::Vec3d &pt1 );

private:

  osg::Vec3d _diff;
};


}; // namespace Draggers
}; // namespace OsgTools


#endif // _OSG_TOOLS_DRAG_TRANSLATE_SCREEN_DRAGGER_CLASS_H_
