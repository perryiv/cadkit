
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV and John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Coordinate axes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_COORDINATE_AXES_H_
#define _OSG_TOOLS_COORDINATE_AXES_H_

#include "OsgTools/Export.h"
#include "OsgTools/Declarations.h"

#include "osg/Vec4"
#include "osg/Quat"


namespace OsgTools {


class OSG_TOOLS_EXPORT Axes
{
public:

  Axes();

  osg::Node*          box_graphic       ( const osg::Vec4& color ) const;
  osg::Group*         cartesian_graphic ( const osg::Vec4& color, const osg::Quat& quat = osg::Quat() ) const;
  osg::Group*         torus_graphic     ( const osg::Vec4& color, const osg::Quat& quat = osg::Quat() ) const;

  void                colorX ( const osg::Vec4 &c ) { _colorX = c; }
  const osg::Vec4 &   colorX() const { return _colorX; }

  void                colorY ( const osg::Vec4 &c ) { _colorY = c; }
  const osg::Vec4 &   colorY() const { return _colorY; }

  void                colorZ ( const osg::Vec4 &c ) { _colorZ = c; }
  const osg::Vec4 &   colorZ() const { return _colorZ; }

  void                colorBox ( const osg::Vec4 &c) { _colorBox = c; }
  const osg::Vec4 &   colorBox() const { return _colorBox; }

  void                length ( float length ) { _length = length; }
  float               length() const { return _length; }

  void                lineWidth ( float width ) { _width = width; }
  const float &       lineWidth() const { return _width; }

  void                state ( unsigned int s ) { _state = s; }
  unsigned int        state() const { return _state; }

  osg::Node *         operator()() const;

  enum DisplayState
  {
    POSITIVE_X  = 0x00000001,
    POSITIVE_Y  = 0x00000002,
    POSITIVE_Z  = 0x00000004,
    NEGATIVE_X  = 0x00000008,
    NEGATIVE_Y  = 0x00000010,
    NEGATIVE_Z  = 0x00000020,
    ROTATE_X    = 0x00000040,
    ROTATE_Y    = 0x00000080,
    ROTATE_Z    = 0x00000100,
    ORIGIN_CUBE = 0x00000200,
  };

private:

  osg::Vec4 _colorX;
  osg::Vec4 _colorY;
  osg::Vec4 _colorZ;
  osg::Vec4 _colorBox;
  float _length;
  float _width;
  unsigned int _state;
};


}; // namespace OsgTools


#endif // _OSG_TOOLS_COORDINATE_AXES_H_
