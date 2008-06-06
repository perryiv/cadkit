
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  3D translate dragger.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_DRAG_TRANSLATE_3_DRAGGER_CLASS_H_
#define _OSG_TOOLS_DRAG_TRANSLATE_3_DRAGGER_CLASS_H_

#include "OsgTools/Draggers/Translate.h"


namespace OsgTools {
namespace Draggers {


class OSG_TOOLS_EXPORT Translate3 : public Translate
{
public:

  // Useful typedefs
  typedef Translate BaseClass;
  typedef osg::ref_ptr<Translate3> Ptr;

  // Constructors.
  Translate3();
  Translate3 ( const Translate3 &, const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY );

  // Plumbing.
  META_Node ( OsgTools, Translate3 );

  // Execute the command.
  virtual void          execute ( Command c, const osg::Vec3d &pt0, const osg::Vec3d &pt1 );

  // File I/O.
  static bool           read  (       osg::Object &, osgDB::Input  & );
  static bool           write ( const osg::Object &, osgDB::Output & );

  // Set/get the translation.
  virtual void          translation ( const osg::Vec3d &t );
  virtual osg::Vec3d    translation() const;

protected:

  // Local callback class.
  struct LocalCallback : public OsgTools::Draggers::Callback
  {
    typedef OsgTools::Draggers::Callback BaseClass;
    LocalCallback ( Translate3 &d, Dragger::Command c );
    virtual void operator() ( Dragger &d );
  protected:
    friend class Translate3;
    LocalCallback ( const LocalCallback & );
    virtual ~LocalCallback();
  private:
    Translate3 &_d;
    Dragger::Command _c;
  };

  // Use reference counting.
  virtual ~Translate3();

  void              _adjust ( Dragger &d );

  void              _initRotations();

  static void       _setX ( double x, osg::MatrixTransform &mt );
  static void       _setY ( double y, osg::MatrixTransform &mt );
  static void       _setZ ( double z, osg::MatrixTransform &mt );

private:

  osg::ref_ptr<osg::MatrixTransform> _rxy;
  osg::ref_ptr<osg::MatrixTransform> _ryz;
  osg::ref_ptr<osg::MatrixTransform> _rzx;
  osg::ref_ptr<Translate> _xy;
  osg::ref_ptr<Translate> _yz;
  osg::ref_ptr<Translate> _zx;
};


}; // namespace Draggers
}; // namespace OsgTools


#endif // _OSG_TOOLS_DRAG_TRANSLATE_3_DRAGGER_CLASS_H_
