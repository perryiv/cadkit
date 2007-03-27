
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Declarations: Forward declarations.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_FORWARD_DECLARATIONS_H_
#define _CV_FORWARD_DECLARATIONS_H_

#include <vector>


namespace osg
{
  class Object;
  class Node;
  class Group;
  class MatrixTransform;
  class Vec3f;
  class Vec4f;
  class Vec3d;
  class Vec4d;
  class Material;
  class StateSet;
  class Drawable;
  typedef std::vector<Node*> NodePath;
  class LineSegment;
  class Geode;
  class Projection;
};

namespace osgUtil
{
  class IntersectVisitor;
};

namespace osgText
{
  class Text;
};

namespace vrj
{
  class Kernel;
};

namespace vrjGA
{
  class ButtonGroup;
  class TrackerDevice;
  class JoystickDevice;
};

namespace Usul
{
  namespace CommandLine
  {
    class Parser;
  };
};

namespace OsgTools
{
  class Axes;
  class Grid;
  class Text;
  class Ray;
};


#endif // _CV_FORWARD_DECLARATIONS_H_
