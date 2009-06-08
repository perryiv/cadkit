
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_STL_PLUGIN_PRINT_VISITOR_H__
#define __OSG_STL_PLUGIN_PRINT_VISITOR_H__

#include "osg/NodeVisitor"

#include "osg/Geode"
#include "osg/LOD"

template < class Writer >
class PrintVisitor : public osg::NodeVisitor
{
public:
  PrintVisitor ( Writer writer ) : osg::NodeVisitor( osg::NodeVisitor::TRAVERSE_ALL_CHILDREN ), _writer ( writer ) 
  {
    // Always hit every node.
    const unsigned int all ( 0xffffffff );
    this->setTraversalMask    ( all );
    this->setNodeMaskOverride ( all );
  }

  virtual void apply(osg::Geode& geode);
  virtual void apply(osg::LOD& lod);

protected:
  Writer _writer;
}; //PrintVisitor


///////////////////////////////////////////////////////////////////////////////
//
//  Write out a geode's geometry to the file
//  TODO, convert polygon to triangles.
//  TODO, handle different normal bindings
//  TODO, handle parent matrix transforms
//
///////////////////////////////////////////////////////////////////////////////
template < class Writer >
void PrintVisitor<Writer>::apply(osg::Geode& geode)
{
  //loop through the drawables
  unsigned int numDrawables ( geode.getNumDrawables() );
  for ( unsigned int i = 0; i < numDrawables; ++i )
  {
    // Get the drawable.
    const osg::Drawable *drawable = geode.getDrawable ( i );

    // See if the drawable is a geometry.
    const osg::Geometry *geometry = drawable->asGeometry();
    if(geometry)
    {
      const osg::Vec3Array *vertices = dynamic_cast< const osg::Vec3Array*> ( geometry->getVertexArray() );
      const osg::Vec3Array *normals = geometry->getNormalArray();

      unsigned int numPrimitiveSets ( geometry->getNumPrimitiveSets() );
      for ( unsigned int j = 0; j < numPrimitiveSets; ++j )
      {
        // Get the primitive-set.
        const osg::PrimitiveSet *primitiveSet = geometry->getPrimitiveSet ( j );
        unsigned int mode ( primitiveSet->getMode() );

        if( const osg::DrawArrays *drawArrays = dynamic_cast < const osg::DrawArrays * > ( primitiveSet ) )
        {
          unsigned int first ( drawArrays->getFirst() );
          unsigned int count ( drawArrays->getCount() );

          switch ( mode )
          {
          case osg::PrimitiveSet::POLYGON:
            //TODO
            break;
          case osg::PrimitiveSet::QUAD_STRIP:
            for(unsigned int k = first; k < count - 3; k ++)
            {
              _writer(normals->at(k), vertices->at(k), vertices->at(k+1), vertices->at(k+2));
              _writer(normals->at(k), vertices->at(k+1), vertices->at(k+2), vertices->at(k+3));
            }
            break;
          case osg::PrimitiveSet::QUADS:
            for(unsigned int k = first; k < count / 4; k ++)
            {
              _writer(normals->at(k), vertices->at(k*4), vertices->at(k*4+1), vertices->at(k*4+2));
              _writer(normals->at(k), vertices->at(k*4+1), vertices->at(k*4+2), vertices->at(k*4+3));
            }
            break;
          case osg::PrimitiveSet::TRIANGLE_FAN:
            for(unsigned int k = first + 1; k < count - 1; k ++)
            {
              _writer(normals->at(k), vertices->at( first ), vertices->at(k), vertices->at(k+1));
            }
            break;
          case osg::PrimitiveSet::TRIANGLE_STRIP:
            for(unsigned int k = first; k < count - 2; k ++)
            {
              _writer(normals->at(k), vertices->at(k), vertices->at(k+1), vertices->at(k+2));
            }
            break;
          case osg::PrimitiveSet::TRIANGLES:
            for(unsigned int k = first; k < count / 3; k ++)
            {
              _writer(normals->at(k), vertices->at(k*3), vertices->at(k*3+1), vertices->at(k*3+2));
            }
            break;
          }
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit a lod node.  Only traverse the highest lod.
//
///////////////////////////////////////////////////////////////////////////////
template < class Writer > 
void PrintVisitor<Writer>::apply(osg::LOD& lod)
{
  //if there are children
  if( lod.getNumChildren() )
  {
    //get first child
    osg::Node *child = lod.getChild ( 0 );
    child->accept( *this );
  }
}


#endif //__OSG_STL_PLUGIN_PRINT_VISITOR_H__
