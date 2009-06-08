///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to make a box.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_BOX_H_
#define _OSG_TOOLS_BOX_H_

#include "OsgTools/Configure/OSG.h"

#include "OsgTools/ColorPolicyFunctor.h"
#include "OsgTools/ColorSetter.h"
#include "OsgTools/MaterialSetter.h"

#include "osg/Vec4"
#include "osg/BoundingBox"
#include "osg/Geode"
#include "osg/Geometry"

namespace OsgTools
{

  template < class ColorPolicy > class Box : public ColorPolicyFunctor < ColorPolicy >
  {
  public:
    typedef ColorPolicyFunctor<ColorPolicy> CPF;
    Box() : CPF(),
      _width  ( 1.0f ), 
      _height ( 1.0f ),
      _depth  ( 1.0f )
    {
    }

    Box ( float w, float h, float d ) : CPF(),
      _width  ( w ),
      _height ( h ),
      _depth  ( d )
    {
    }

    Box ( float size, float detailRatio ) : CPF(),
      _width  ( size ),
      _height ( size ),
      _depth  ( size )
    {
    }

    Box ( const osg::BoundingBox &b ) : CPF(),
      _width  ( 1.0f ), 
      _height ( 1.0f ),
      _depth  ( 1.0f )
    {
      if ( b.valid() )
      {
        _width  = b.xMax() - b.xMin();
        _height = b.yMax() - b.yMin();
        _depth  = b.zMax() - b.zMin();
      }
    }

    Box ( const Box &b ) : CPF ( b ),
      _width  ( b._width  ),
      _height ( b._height ),
      _depth  ( b._depth  )
    {
    }

    virtual ~Box()
    {
    }

    Box& operator = ( const Box& b )
    {
      CPF::operator = ( b );
      _width  = b._width;
      _height = b._height;
      _depth  = b._depth;
      return *this;
    }

    virtual osg::Node* operator()() const
    {
      // half lengths
      float width_2 = _width*0.5;
      float height_2 = _height*0.5;
      float depth_2 = _depth*0.5;

      osg::Vec3Array* vertices = new osg::Vec3Array;
      osg::Vec3Array* normals = new osg::Vec3Array;

      // front face
      vertices->push_back( osg::Vec3(-width_2,-height_2,depth_2) );
      vertices->push_back( osg::Vec3(width_2,-height_2,depth_2) );
      vertices->push_back( osg::Vec3(width_2,height_2,depth_2) );
      vertices->push_back( osg::Vec3(-width_2,height_2,depth_2) );
      normals->push_back( osg::Vec3(0.0,0.0,1.0) );

      // back face
      vertices->push_back( osg::Vec3(width_2,-height_2,-depth_2) );
      vertices->push_back( osg::Vec3(-width_2,-height_2,-depth_2) );
      vertices->push_back( osg::Vec3(-width_2,height_2,-depth_2) );
      vertices->push_back( osg::Vec3(width_2,height_2,-depth_2) );
      normals->push_back( osg::Vec3(0.0,0.0,-1.0) );

      // top face
      vertices->push_back( osg::Vec3(width_2,height_2,depth_2) );
      vertices->push_back( osg::Vec3(width_2,height_2,-depth_2) );
      vertices->push_back( osg::Vec3(-width_2,height_2,-depth_2) );
      vertices->push_back( osg::Vec3(-width_2,height_2,depth_2) );
      normals->push_back( osg::Vec3(0.0,1.0,0.0) );

      // bottom face
      vertices->push_back( osg::Vec3(width_2,-height_2,depth_2) );
      vertices->push_back( osg::Vec3(-width_2,-height_2,depth_2) );
      vertices->push_back( osg::Vec3(-width_2,-height_2,-depth_2) );
      vertices->push_back( osg::Vec3(width_2,-height_2,-depth_2) );
      normals->push_back( osg::Vec3(0.0,-1.0,0.0) );

      // left face
      vertices->push_back( osg::Vec3(-width_2,height_2,depth_2) );
      vertices->push_back( osg::Vec3(-width_2,height_2,-depth_2) );
      vertices->push_back( osg::Vec3(-width_2,-height_2,-depth_2) );
      vertices->push_back( osg::Vec3(-width_2,-height_2,depth_2) );
      normals->push_back( osg::Vec3(-1.0,0.0,0.0) );

      // right face
      vertices->push_back( osg::Vec3(width_2,height_2,-depth_2) );
      vertices->push_back( osg::Vec3(width_2,height_2,depth_2) );
      vertices->push_back( osg::Vec3(width_2,-height_2,depth_2) );
      vertices->push_back( osg::Vec3(width_2,-height_2,-depth_2) );
      normals->push_back( osg::Vec3(1.0,0.0,0.0) );


      osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
      geometry->setVertexArray( vertices );
      geometry->setNormalArray( normals );
      geometry->setNormalBinding( osg::Geometry::BIND_PER_PRIMITIVE );
      geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,
						    0,vertices->size()) );

      // Needed to make gcc 4.1.1 happy
      this->color_policy()( geometry.get() );

      osg::ref_ptr<osg::Geode> geode = new osg::Geode;
      geode->setName("OsgTools_Box_geode");
      geode->addDrawable( geometry.get() );
      return( geode.release() );
    }

    void set(float w,float h,float d) { _width = w; _height = h; _depth = d; }

    void  setDepth ( float d ) { _depth = d; }
    float getDepth() const { return _depth; }

    void  setHeight ( float h ) { _height = h; }
    float getHeight() const { return _height; }

    void  setWidth ( float w ) { _width = w; }
    float getWidth() const { return _width; }

  private:

    float _width,_height,_depth;
  };

  typedef Box<ColorSetter> ColorBox;
  typedef Box<MaterialSetter> MaterialBox;
}; // namespace OsgTools

#endif // _OSG_TOOLS_BOX_H_
