
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_STL_PRINT_VISITOR_H__
#define __OSG_TOOLS_STL_PRINT_VISITOR_H__

#include "osg/NodeVisitor"

#include "osg/Geode"
#include "osg/LOD"
#include "osg/Geometry"

#include <iostream>

namespace OsgTools {
namespace IO {


///////////////////////////////////////////////////////////////////////////////
//
//  Visitor to write out an STL file.  Make sure scene is flattened before
//  applying visitor.
//
///////////////////////////////////////////////////////////////////////////////

template < class Writer >
class STLPrintVisitor : public osg::NodeVisitor
{
public:
  STLPrintVisitor ( std::ostream& out ) : osg::NodeVisitor( osg::NodeVisitor::TRAVERSE_ALL_CHILDREN ), _writer( ), _out ( out )
  {
    // Always hit every node.
    const unsigned int all ( 0xffffffff );
    this->setTraversalMask    ( all );
    this->setNodeMaskOverride ( all );
  }

  virtual void apply(osg::Geode& geode);
  virtual void apply(osg::LOD& lod);

protected:
  virtual ~STLPrintVisitor() { }

  void _writeDrawable( osg::Drawable& );

  void _handleDrawArrays ( const osg::DrawArrays& drawArrays, 
                           const osg::Vec3Array& vertices, 
                           const osg::Vec3Array& normals, 
                           osg::Vec3Array& verticesToWrite, 
                           osg::Vec3Array& normalsToWrite, 
                           bool bindPerVertex );

  template < class DrawElements >
  void _handleDrawElements ( const DrawElements& indices,
                             const osg::Vec3Array& vertices, 
                             const osg::Vec3Array& normals, 
                             osg::Vec3Array& verticesToWrite, 
                             osg::Vec3Array& normalsToWrite, 
                             bool bindPerVertex )
  {
    switch( indices.getMode() )
    {
    case osg::PrimitiveSet::TRIANGLES:
    {
      if( !bindPerVertex )
        normalsToWrite = normals;
      else
      {
        normalsToWrite.reserve( normals.size() / 3);
      }

      for( typename DrawElements::const_iterator iter = indices.begin(); iter != indices.end(); iter += 3 )
      {
        verticesToWrite.push_back( vertices.at( *iter ) );
        verticesToWrite.push_back( vertices.at( *(iter + 1) ) );
        verticesToWrite.push_back( vertices.at( *(iter + 2) ) );

        if ( bindPerVertex )
        {
          normalsToWrite.push_back( normals.at ( *(iter) ) );
        }
      }
      break;
    }
    case osg::PrimitiveSet::TRIANGLE_STRIP:
    {
      typename DrawElements::const_iterator iter = indices.begin();
      for( typename DrawElements::size_type i = 2; i < indices.size(); ++i, ++iter)
      {
        normalsToWrite.push_back  ( normals.at ( i ) );

        if ( ( i%2 ) )
        {
          verticesToWrite.push_back( vertices.at( *iter ) );
          verticesToWrite.push_back( vertices.at( *(iter + 2) ) );
          verticesToWrite.push_back( vertices.at( *(iter + 1) ) );
        }
        else
        {
          verticesToWrite.push_back( vertices.at( *iter ) );
          verticesToWrite.push_back( vertices.at( *(iter + 1) ) );
          verticesToWrite.push_back( vertices.at( *(iter + 2) ) );
        }
      }
      break;
    }
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
    case osg::PrimitiveSet::TRIANGLE_FAN:
    {
      typename DrawElements::const_iterator iter = indices.begin();
      const osg::Vec3& vfirst = vertices.at(*iter);
      ++iter;
      for( typename DrawElements::size_type i=2; i<indices.size(); ++i,++iter )
      {
        normalsToWrite.push_back  ( normals.at ( i ) );
        verticesToWrite.push_back( vfirst );
        verticesToWrite.push_back( vertices.at( *(iter) ) );
        verticesToWrite.push_back( vertices.at( *(iter + 1) ) );
      }
      break;
    }
    default:
      // can't be converted into to triangles.
      break;
    }
  }

private:
  Writer _writer;
  std::ostream &_out;
}; //PrintVisitor


///////////////////////////////////////////////////////////////////////////////
//
//  Write out a geode's geometry to the file
//
///////////////////////////////////////////////////////////////////////////////

template < class Writer >
void STLPrintVisitor<Writer>::apply(osg::Geode& geode)
{
  //Get the number of drawables
  const unsigned int numDrawables ( geode.getNumDrawables() );

  //loop through the drawables
  for ( unsigned int i = 0; i < numDrawables; ++i )
  {
    // Get the drawable.
    osg::Drawable *drawable = geode.getDrawable ( i );
    this->_writeDrawable( *drawable );
  }
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit a lod node.  Only traverse the highest lod.
//
///////////////////////////////////////////////////////////////////////////////

template < class Writer > 
void STLPrintVisitor<Writer>::apply(osg::LOD& lod)
{
  //if there are children
  if( lod.getNumChildren() )
  {
    //get first child
    osg::Node *child = lod.getChild ( 0 );
    child->accept( *this );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write out a drawable.
//  TODO, handle different normal bindings
//
///////////////////////////////////////////////////////////////////////////////

template < class Writer >
void STLPrintVisitor<Writer>::_writeDrawable( osg::Drawable& drawable )
{
  osg::ref_ptr< osg::Vec3Array > verticesToWrite ( new osg::Vec3Array );
  osg::ref_ptr< osg::Vec3Array > normalsToWrite  ( new osg::Vec3Array );

  // See if the drawable is a geometry.
  if( osg::Geometry *geometry = drawable.asGeometry() )
  {
    osg::ref_ptr< osg::Vec3Array > vertices ( dynamic_cast< osg::Vec3Array* > ( geometry->getVertexArray() ) );
    osg::ref_ptr< osg::Vec3Array > normals  ( dynamic_cast< osg::Vec3Array* > ( geometry->getNormalArray() ) );

    // Are we binding per vertex or per primitive
    bool bindPerVertex ( geometry->getNormalBinding() == osg::Geometry::BIND_PER_VERTEX );

    const unsigned int numPrimitiveSets ( geometry->getNumPrimitiveSets() );

    //Loop through all the primitive sets
    for ( unsigned int j = 0; j < numPrimitiveSets; ++j )
    {
      osg::PrimitiveSet *primitiveSet ( geometry->getPrimitiveSet( j ) );

      //See if the primitive set is a draw arrays
      if( const osg::DrawArrays *drawArrays = dynamic_cast < const osg::DrawArrays * > ( primitiveSet ) )
      {
        this->_handleDrawArrays( *drawArrays, *vertices, *normals, *verticesToWrite, *normalsToWrite, bindPerVertex );
      }

      else if( const osg::DrawElementsUByte *drawElementsUByte = dynamic_cast < const osg::DrawElementsUByte * > ( primitiveSet ) )
      {
        this->_handleDrawElements< osg::DrawElementsUByte > ( *drawElementsUByte, *vertices, *normals, *verticesToWrite, *normalsToWrite, bindPerVertex );
      }

      else if( const osg::DrawElementsUShort *drawElementsUShort = dynamic_cast < const osg::DrawElementsUShort * > ( primitiveSet ) )
      {
        this->_handleDrawElements< osg::DrawElementsUShort > ( *drawElementsUShort, *vertices, *normals, *verticesToWrite, *normalsToWrite, bindPerVertex );
      }

      else if( const osg::DrawElementsUInt *drawElementsUInt = dynamic_cast < const osg::DrawElementsUInt * > ( primitiveSet ) )
      {
        this->_handleDrawElements< osg::DrawElementsUInt > ( *drawElementsUInt, *vertices, *normals, *verticesToWrite, *normalsToWrite, bindPerVertex );
      }

    }
  }

  _writer( _out, *verticesToWrite, *normalsToWrite );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Handle draw arrays.
//
///////////////////////////////////////////////////////////////////////////////

template < class Writer >
void STLPrintVisitor<Writer>::_handleDrawArrays ( const osg::DrawArrays& drawArrays, const osg::Vec3Array &vertices, const osg::Vec3Array& normals, osg::Vec3Array& verticesToWrite, osg::Vec3Array& normalsToWrite, bool bindPerVertex )
{
  const unsigned int first ( drawArrays.getFirst() );
  const unsigned int count ( drawArrays.getCount() );

  switch ( drawArrays.getMode() )
  {
  case osg::PrimitiveSet::QUAD_STRIP:
    {
      verticesToWrite.reserve( count * 2 );
      normalsToWrite.reserve ( count * 2 / 3 );
      for(unsigned int k = first; k < count - 3; k ++)
      {
        //Split this quad into two
        normalsToWrite.push_back  ( normals.at( k ) );
        verticesToWrite.push_back ( vertices.at( k ) );
        verticesToWrite.push_back ( vertices.at( k + 1 ) );
        verticesToWrite.push_back ( vertices.at( k + 2 ) );
        
        normalsToWrite.push_back  ( normals.at( k ) );
        verticesToWrite.push_back ( vertices.at( k + 1 ) );
        verticesToWrite.push_back ( vertices.at( k + 2 ) );
        verticesToWrite.push_back ( vertices.at( k + 3 ) );
      }
    }
    break;
  case osg::PrimitiveSet::QUADS:
    {
      const unsigned int numQuads ( count / 4 );
      verticesToWrite.reserve( numQuads * 2     );
      normalsToWrite.reserve ( numQuads * 2 / 3 );

      for(unsigned int k = first; k < numQuads; k ++)
      {
        //Split this quad into two
        normalsToWrite.push_back( normals.at(k) );
        verticesToWrite.push_back ( vertices.at(k * 4) );
        verticesToWrite.push_back ( vertices.at(k * 4 + 1) );
        verticesToWrite.push_back ( vertices.at(k * 4 + 2) );
        
        normalsToWrite.push_back( normals.at(k) );
        verticesToWrite.push_back ( vertices.at(k * 4 + 1) );
        verticesToWrite.push_back ( vertices.at(k * 4 + 2) );
        verticesToWrite.push_back ( vertices.at(k * 4 + 3) );
      }
    }
    break;
  case osg::PrimitiveSet::POLYGON: // Treat polygon as triangle fan.
  case osg::PrimitiveSet::TRIANGLE_FAN:
    {
      const unsigned int numTris ( count - 1 );
      verticesToWrite.reserve( numTris      );
      normalsToWrite.reserve ( numTris  / 3 );

      for(unsigned int k = first + 1; k < numTris; k ++)
      {
        normalsToWrite.push_back( normals.at(k) );
        verticesToWrite.push_back ( vertices.at( first ) );
        verticesToWrite.push_back ( vertices.at( k ) );
        verticesToWrite.push_back ( vertices.at( k + 1 ) );
      }
    }
    break;
  case osg::PrimitiveSet::TRIANGLE_STRIP:
    {
      const unsigned int numTris ( count - 2 );
      verticesToWrite.reserve( numTris      );
      normalsToWrite.reserve ( numTris  / 3 );

      for(unsigned int k = first; k < count - 2; k ++)
      {
        normalsToWrite.push_back  ( normals.at ( k ) );
        verticesToWrite.push_back ( vertices.at( k ) );

        if ( k % 2 )
        {
          verticesToWrite.push_back ( vertices.at( k + 2 ) );
          verticesToWrite.push_back ( vertices.at( k + 1 ) );
        }
        else
        {
          verticesToWrite.push_back ( vertices.at( k + 1 ) );
          verticesToWrite.push_back ( vertices.at( k + 2 ) );
        }
      }
    }
    break;
  case osg::PrimitiveSet::TRIANGLES:
    verticesToWrite = vertices;
    if( !bindPerVertex )
      normalsToWrite = normals;
    else
    {
      normalsToWrite.reserve( normals.size() / 3);
      //For now write every third normal.
      for( unsigned int i = 0; i < normals.size(); i += 3 )
        normalsToWrite.push_back( normals.at ( i ) );
    }
    break;
  }
}

}
}

#endif // __OSG_TOOLS_STL_PRINT_VISITOR_H__
