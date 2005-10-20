
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
  void _writeDrawable( osg::Drawable& );

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
//  Write out a drawable
//  TODO, convert polygon to triangles.
//  TODO, handle different normal bindings
//
///////////////////////////////////////////////////////////////////////////////

template < class Writer >
void STLPrintVisitor<Writer>::_writeDrawable( osg::Drawable& drawable)
{
  osg::ref_ptr< osg::Vec3Array > verticesToWrite ( new osg::Vec3Array );
  osg::ref_ptr< osg::Vec3Array > normalsToWrite  ( new osg::Vec3Array );

  // See if the drawable is a geometry.
  if( osg::Geometry *geometry = drawable.asGeometry() )
  {
    osg::ref_ptr< osg::Vec3Array > vertices ( dynamic_cast< osg::Vec3Array*> ( geometry->getVertexArray() ) );
    osg::ref_ptr< osg::Vec3Array > normals  ( geometry->getNormalArray() );

    // Are we binding per vertex or per primitive
    bool bindPerVertex ( geometry->getNormalBinding() == osg::Geometry::BIND_PER_VERTEX );

    const unsigned int numPrimitiveSets ( geometry->getNumPrimitiveSets() );

    //Loop through all the primitive sets
    for ( unsigned int j = 0; j < numPrimitiveSets; ++j )
    {
      //See if the primitive set is a draw arrays
      if( const osg::DrawArrays *drawArrays = dynamic_cast < const osg::DrawArrays * > ( geometry->getPrimitiveSet ( j ) ) )
      {
        const unsigned int first ( drawArrays->getFirst() );
        const unsigned int count ( drawArrays->getCount() );

        switch ( drawArrays->getMode() )
        {
        case osg::PrimitiveSet::POLYGON:
          //TODO 
          break;
        case osg::PrimitiveSet::QUAD_STRIP:
          {
            verticesToWrite->reserve( count * 2 );
            normalsToWrite->reserve ( count * 2 / 3 );
            for(unsigned int k = first; k < count - 3; k ++)
            {
              //Split this quad into two
              normalsToWrite->push_back( normals->at(k) );
              verticesToWrite->push_back ( vertices->at(k) );
              verticesToWrite->push_back ( vertices->at(k + 1) );
              verticesToWrite->push_back ( vertices->at(k + 2) );
              
              normalsToWrite->push_back( normals->at(k) );
              verticesToWrite->push_back ( vertices->at(k + 1) );
              verticesToWrite->push_back ( vertices->at(k + 2) );
              verticesToWrite->push_back ( vertices->at(k + 3) );
            }
          }
          break;
        case osg::PrimitiveSet::QUADS:
          {
            const unsigned int numQuads ( count / 4 );
            verticesToWrite->reserve( numQuads * 2     );
            normalsToWrite->reserve ( numQuads * 2 / 3 );

            for(unsigned int k = first; k < numQuads; k ++)
            {
              //Split this quad into two
              normalsToWrite->push_back( normals->at(k) );
              verticesToWrite->push_back ( vertices->at(k * 4) );
              verticesToWrite->push_back ( vertices->at(k * 4 + 1) );
              verticesToWrite->push_back ( vertices->at(k * 4 + 2) );
              
              normalsToWrite->push_back( normals->at(k) );
              verticesToWrite->push_back ( vertices->at(k * 4 + 1) );
              verticesToWrite->push_back ( vertices->at(k * 4 + 2) );
              verticesToWrite->push_back ( vertices->at(k * 4 + 3) );
            }
          }
          break;
        case osg::PrimitiveSet::TRIANGLE_FAN:
          {
            const unsigned int numTris ( count - 1 );
            verticesToWrite->reserve( numTris      );
            normalsToWrite->reserve ( numTris  / 3 );

            for(unsigned int k = first + 1; k < numTris; k ++)
            {
              normalsToWrite->push_back( normals->at(k) );
              verticesToWrite->push_back ( vertices->at( first ) );
              verticesToWrite->push_back ( vertices->at( k ) );
              verticesToWrite->push_back ( vertices->at( k + 1 ) );
            }
          }
          break;
        case osg::PrimitiveSet::TRIANGLE_STRIP:
          {
            const unsigned int numTris ( count - 2 );
            verticesToWrite->reserve( numTris      );
            normalsToWrite->reserve ( numTris  / 3 );

            for(unsigned int k = first; k < count - 2; k ++)
            {
              normalsToWrite->push_back  ( normals->at ( k ) );
              verticesToWrite->push_back ( vertices->at( k ) );

              if ( k % 2 )
              {
                verticesToWrite->push_back ( vertices->at( k + 2 ) );
                verticesToWrite->push_back ( vertices->at( k + 1 ) );
              }
              else
              {
                verticesToWrite->push_back ( vertices->at( k + 1 ) );
                verticesToWrite->push_back ( vertices->at( k + 2 ) );
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
            normalsToWrite->reserve( normals->size() / 3);
            //For now write every third normal.  This should probably be a policy
            for( unsigned int i = 0; i < normals->size(); i += 3 )
              normalsToWrite->push_back( normals->at ( i ) );
          }
          break;
        }
      }
    }
  }

  _writer( _out, *verticesToWrite, *normalsToWrite );
}

}
}

#endif // __OSG_TOOLS_STL_PRINT_VISITOR_H__
