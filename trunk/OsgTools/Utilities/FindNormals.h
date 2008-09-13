
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_UTILITIES_FIND_NORMALS_H__
#define __OSG_TOOLS_UTILITIES_FIND_NORMALS_H__

#include "OsgTools/Export.h"
#include "OsgTools/Configure/OSG.h"

#include "osg/NodeVisitor"
#include "osg/Group"
#include "osg/ref_ptr"
#include "osg/Array"
#include "osg/PrimitiveSet"
#include "osg/BoundingBox"


namespace OsgTools {
namespace Utilities {


class OSG_TOOLS_EXPORT FindNormals : public osg::NodeVisitor
{
public:
  typedef osg::NodeVisitor BaseClass;

  FindNormals ( );

  osg::Node* normals (); 

  virtual void apply(osg::Geode& geode);
  virtual void apply(osg::LOD& lod);

  void         size ( double size ) { _size = size; }

protected:
  void _handleDrawArrays ( const osg::DrawArrays& drawArrays,
                           const osg::Vec3Array& vertices,
                           const osg::Vec3Array& normals,
                           bool bindPerVertex );

  template < class DrawElements >
  void _handleDrawElements ( const DrawElements& indices,
                             const osg::Vec3Array& vertices, 
                             const osg::Vec3Array& normals, 
                             bool bindPerVertex )
  {

    switch( indices.getMode() )
    {
    case osg::PrimitiveSet::TRIANGLES:
    {
      for( typename DrawElements::const_iterator iter = indices.begin(); iter != indices.end(); iter += 3 )
      {
        const osg::Vec3& v0 ( vertices.at( *iter ) );
        const osg::Vec3& v1 ( vertices.at( *(iter + 1 ) ) );
        const osg::Vec3& v2 ( vertices.at( *(iter + 2 ) ) );

        if( bindPerVertex )
        {
          this->_addNormal( v0, normals.at( *iter ) );
          this->_addNormal( v1, normals.at( *(iter + 1 ) ) );
          this->_addNormal( v2, normals.at( *(iter + 2 ) ) );
        }
        else
        {
          osg::Vec3 v ( v0 + v1 + v2 );
          this->_addNormal( v0, normals.at( *iter / 3 ) );
        }
      }
    }
    break;
    case osg::PrimitiveSet::TRIANGLE_FAN:
    {
      typename DrawElements::const_iterator iter = indices.begin();
      const osg::Vec3& vfirst = vertices.at(*iter);
      ++iter;

      if ( bindPerVertex )
        this->_addNormal( vfirst, normals.at( *iter ) );
      
      for( typename DrawElements::size_type i = 2; i < indices.size(); ++i,++iter )
      {
        if( bindPerVertex )
        {
          this->_addNormal( vertices.at( *(iter) ), normals.at ( *iter ) );
        }
        // Not sure how to handle bind per primitive.
      }
      
    }
    break;
    case osg::PrimitiveSet::TRIANGLE_STRIP:
    {
      if( bindPerVertex )
      {
        typename DrawElements::const_iterator iter = indices.begin();
        for( typename DrawElements::size_type i = 2; i < indices.size(); ++i, ++iter)
        {
          this->_addNormal( vertices.at( *(iter) ), normals.at ( *iter ) );
        }
      }
      break;
    }
#if 0
    case osg::PrimitiveSet::QUADS:
    {
      typename DrawElements::const_iterator iter = indices.begin();

      for( typename DrawElements::size_type i = 3; i < indices.size(); i+=4, iter+=4 )
      {
        normalsToWrite.push_back  ( normals.at ( i ) );
        verticesToWrite.push_back( vertices.at( *iter ) );
        verticesToWrite.push_back( vertices.at( *(iter + 1) ) );
        verticesToWrite.push_back( vertices.at( *(iter + 2) ) );

        normalsToWrite.push_back  ( normals.at ( i ) );
        verticesToWrite.push_back( vertices.at( *iter ) );
        verticesToWrite.push_back( vertices.at( *(iter + 2) ) );
        verticesToWrite.push_back( vertices.at( *(iter + 3) ) );
      }
      break;
    }
    case osg::PrimitiveSet::QUAD_STRIP:
    {
      typename DrawElements::const_iterator iter = indices.begin();
      for( typename DrawElements::size_type i = 3; i < indices.size(); i+=2,iter+=2 )
      {
        normalsToWrite.push_back  ( normals.at ( i ) );
        verticesToWrite.push_back( vertices.at( *iter ) );
        verticesToWrite.push_back( vertices.at( *(iter + 1) ) );
        verticesToWrite.push_back( vertices.at( *(iter + 2) ) );

        normalsToWrite.push_back  ( normals.at ( i ) );
        verticesToWrite.push_back( vertices.at( *(iter + 1)  ) );
        verticesToWrite.push_back( vertices.at( *(iter + 3) ) );
        verticesToWrite.push_back( vertices.at( *(iter + 2) ) );
      }
      break;
    }
    case osg::PrimitiveSet::POLYGON: // treat polygons as GL_TRIANGLE_FAN
    
    #endif
    default:
      break;
    }
  }

  void _addNormal ( const osg::Vec3& position, const osg::Vec3& normal );

private:
  virtual ~FindNormals();

  osg::ref_ptr < osg::Group > _root;

  double _size;

  osg::ref_ptr < osg::Vec3Array > _vertices;
};


}
}

#endif // __OSG_TOOLS_UTILITIES_FIND_NORMALS_H__
