///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to make a Sphere.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _osg_tools_builder_sphere_
#define _osg_tools_builder_sphere_

#include "OsgTools/Export.h"
#include "OsgTools/Declarations.h"

// forward declarations
namespace osg
{
  class TessellationHints;
  class Material;
  class ShapeDrawable;
  class PolygonMode;
}

namespace OsgTools
{

  class OSG_TOOLS_EXPORT Sphere
  {
  public:
    // constructors
    Sphere();
    Sphere(const Sphere&);
    Sphere& operator = (const Sphere&);
    virtual ~Sphere();

    Sphere(const osg::Vec3&, float);
    Sphere(const osg::BoundingSphere&);

  protected:
    void _init();

  public:
    // methods
    osg::Geode* operator() ();  // accessing method

  private:
    // member data
    osg::ref_ptr<osg::TessellationHints> _hints;
    osg::ref_ptr<osg::Sphere> _sphere;
    osg::ref_ptr<osg::Material> _material;
    osg::ref_ptr<osg::StateSet> _stateset;
    osg::ref_ptr<osg::ShapeDrawable> _shapedrawable;
    osg::ref_ptr<osg::PolygonMode> _polygonmode;
    osg::Vec4 _color;
  };

};
    

#endif
