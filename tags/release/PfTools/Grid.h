
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to make a grid.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _PF_TOOLS_GRID_H_
#define _PF_TOOLS_GRID_H_

#include "PfTools/Export.h"

#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector4.h"

class pfNode;


namespace PfTools {


class PF_TOOLS_EXPORT Grid
{
public:

  typedef Usul::Math::Vec2ui Vec2ui;
  typedef Usul::Math::Vec2f Vec2f;
  typedef Usul::Math::Vec3f Vec3f;
  typedef Usul::Math::Vec4f Vec4f;

  Grid();

  void                color ( const Vec4f &c ) { _color = c; }
  const Vec4f &       color() const { return _color; }

  void                numBlocks ( const Vec2ui &num ) { _numBlocks = num; }
  const Vec2ui &      numBlocks() const { return _numBlocks; }

  void                size ( const Vec2f &s ) { _size = s; }
  const Vec2f &       size() const { return _size; }

  void                center ( const Vec3f &c ) { _center = c; }
  const Vec3f &       center() const { return _center ; }

  void                lineWidth ( float width ) { _lineWidth = width; }
  const float &       lineWidth() const { return _lineWidth; }

  pfNode *            operator()() const;

private:

  Vec2ui _numBlocks;
  Vec4f _color;  // rgba
  Vec2f _size;   // xz
  Vec3f _center; // xyz
  float _lineWidth;
};


}; // namespace PfTools


#endif // _PF_TOOLS_GRID_H_
