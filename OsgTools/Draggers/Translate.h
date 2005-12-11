
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base translate dragger.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_DRAG_TRANSLATE_DRAGGER_CLASS_H_
#define _OSG_TOOLS_DRAG_TRANSLATE_DRAGGER_CLASS_H_

#include "OsgTools/Draggers/Dragger.h"


namespace OsgTools {
namespace Draggers {


class OSG_TOOLS_EXPORT Translate : public Dragger
{
public :

  // Useful typedefs
  typedef Dragger BaseClass;
  typedef osg::ref_ptr<Translate> Ptr;

  // Plumbing.
  META_Node ( OsgTools, Translate );

  // Execute the command.
  virtual void          execute ( Command c, const osg::Vec3d &pt0, const osg::Vec3d &pt1 );

  // File I/O.
  static bool           read  (       osg::Object &, osgDB::Input  & );
  static bool           write ( const osg::Object &, osgDB::Output & );

  // Set/get the translation.
  virtual void          translation ( const osg::Vec3d &t );
  virtual osg::Vec3d    translation();

protected :

  // Constructors.
  Translate();
  Translate ( const Translate &, const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY );

  // Use reference counting.
  virtual ~Translate();

  osg::Vec3d _trans;
};


}; // namespace Draggers
}; // namespace OsgTools


#endif // _OSG_TOOLS_DRAG_TRANSLATE_DRAGGER_CLASS_H_
