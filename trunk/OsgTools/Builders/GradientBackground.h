
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Makes a gradient background.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_GRADIENT_BACKGROUND_H__
#define __OSG_TOOLS_GRADIENT_BACKGROUND_H__

#include "OsgTools/Export.h"
#include "OsgTools/Declarations.h"

#include "osg/Projection"
#include "osg/MatrixTransform"
#include "osg/Geometry"
#include "osg/ref_ptr"


namespace OsgTools {
namespace Builders {


class OSG_TOOLS_EXPORT GradientBackground
{
public:

  // Background corners.
  struct Corners
  {
    enum
    {
      BOTTOM_LEFT  = 0x00000001,
      BOTTOM_RIGHT = 0x00000002,
      TOP_LEFT     = 0x00000004,
      TOP_RIGHT    = 0x00000008,
      LEFT         = BOTTOM_LEFT  | TOP_LEFT,
      RIGHT        = BOTTOM_RIGHT | TOP_RIGHT,
      BOTTOM       = BOTTOM_LEFT  | BOTTOM_RIGHT,
      TOP          = TOP_LEFT     | TOP_RIGHT,
      ALL          = BOTTOM       | TOP
    };
  };

  // Constructor/destructor.
  GradientBackground();
  ~GradientBackground();

  // Clear the internal scene.
  void                      clear();

  // Set the colors.
  void                      color ( const osg::Vec4 &, unsigned int corners );

  // Get the color. Asking for more than one will average them.
  osg::Vec4                 color ( unsigned int corners ) const;

  // Initialize the internal scene.
  void                      init();

  // Return the root of the scene.
  osg::Node *               root();

  // Update the vertices.
  void                      update ( unsigned int width, unsigned int height );

private:

  osg::ref_ptr<osg::Projection> _root;
  osg::ref_ptr<osg::Geometry> _geom;
  osg::ref_ptr<osg::Vec3Array> _vertices;
  osg::ref_ptr<osg::Vec4Array> _colors;
};


}
}


#endif // __OSG_TOOLS_GRADIENT_BACKGROUND_H__
