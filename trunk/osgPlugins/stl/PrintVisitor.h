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
//  TODO, convert other primitives to triangles.
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
      const osg::Array *constarray = geometry->getVertexArray();
      osg::Array *array = const_cast < osg::Array*> ( constarray );
      osg::ref_ptr< osg::Vec3Array > vertices = dynamic_cast< osg::Vec3Array*> (array);
      const osg::Vec3Array *constvec3array = geometry->getNormalArray();
      osg::ref_ptr< osg::Vec3Array > normals  = const_cast<osg::Vec3Array *> (constvec3array);

      for(unsigned int j = 0; j < normals->size(); ++j)
      {
        _writer(normals->at(j), vertices->at(j*3), vertices->at(j*3+1), vertices->at(j*3+2));
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
