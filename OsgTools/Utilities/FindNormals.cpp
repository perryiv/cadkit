
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Utilities/FindNormals.h"

#include "OsgTools/Ray.h"

#include "osg/Geode"
#include "osg/Geometry"
#include "osg/LOD"
#include "osg/AutoTransform"

using namespace OsgTools::Utilities;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

FindNormals::FindNormals ( ) : 
  BaseClass ( osg::NodeVisitor::TRAVERSE_ALL_CHILDREN ),
  _root( new osg::Group ),
  _size ( 1.0 ),
  _vertices( new osg::Vec3Array )
{
  // Always hit every node.
  const unsigned int all ( 0xffffffff );
  this->setTraversalMask    ( all );
  this->setNodeMaskOverride ( all );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

FindNormals::~FindNormals()
{
  _root = 0x0;
  _vertices = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the node to draw the normals.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* FindNormals::normals ()
{
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );

  geometry->setVertexArray( _vertices.get() );
  
  osg::Vec4 color ( 0.0, 1.0, 0.0, 1.0 );
  osg::ref_ptr< osg::Vec4Array > colors ( new osg::Vec4Array );
  colors->push_back ( color );
  
  geometry->setColorArray ( colors.get() );

  geometry->setColorBinding ( osg::Geometry::BIND_OVERALL );

  geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINES, 0, _vertices->size() ) );

  geode->addDrawable( geometry.get() );

  _root->addChild( geode.get() );

  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit a geode.
//
///////////////////////////////////////////////////////////////////////////////

void FindNormals::apply(osg::Geode& geode)
{
  //Get the number of drawables
  const unsigned int numDrawables ( geode.getNumDrawables() );

  //loop through the drawables
  for ( unsigned int i = 0; i < numDrawables; ++i )
  {
    // Get the drawable.
    osg::Drawable *drawable = geode.getDrawable ( i );
    
    if ( osg::Geometry *geometry = dynamic_cast < osg::Geometry* > ( drawable ) )
    {
      osg::ref_ptr< osg::Vec3Array > vertices ( dynamic_cast < osg::Vec3Array * > ( geometry->getVertexArray() ) );
      osg::ref_ptr< osg::Vec3Array > normals  ( dynamic_cast < osg::Vec3Array* > ( geometry->getNormalArray() ) );

      if( vertices.valid() && normals.valid() )
      {
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
            this->_handleDrawArrays( *drawArrays, *vertices, *normals, bindPerVertex );
          }

          else if( const osg::DrawElementsUByte *drawElementsUByte = dynamic_cast < const osg::DrawElementsUByte * > ( primitiveSet ) )
          {
            this->_handleDrawElements< osg::DrawElementsUByte > ( *drawElementsUByte, *vertices, *normals, bindPerVertex );
          }

          else if( const osg::DrawElementsUShort *drawElementsUShort = dynamic_cast < const osg::DrawElementsUShort * > ( primitiveSet ) )
          {
            this->_handleDrawElements< osg::DrawElementsUShort > ( *drawElementsUShort, *vertices, *normals, bindPerVertex );
          }

          else if( const osg::DrawElementsUInt *drawElementsUInt = dynamic_cast < const osg::DrawElementsUInt * > ( primitiveSet ) )
          {
            this->_handleDrawElements< osg::DrawElementsUInt > ( *drawElementsUInt, *vertices, *normals, bindPerVertex );
          }
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit a lod.
//
///////////////////////////////////////////////////////////////////////////////

void FindNormals::apply(osg::LOD& lod)
{
  // What to do about lods?
  // Should we visit each one and build lods in our mini-scene?
  // Or do we just visit the highest LOD.

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
//  Handle draw arrays.
//
///////////////////////////////////////////////////////////////////////////////

void FindNormals::_handleDrawArrays ( const osg::DrawArrays& drawArrays,
                           const osg::Vec3Array& vertices,
                           const osg::Vec3Array& normals,
                           bool bindPerVertex )
{
  const unsigned int first ( drawArrays.getFirst() );
  const unsigned int count ( drawArrays.getCount() );

  switch ( drawArrays.getMode() )
  {
  case osg::PrimitiveSet::TRIANGLES:
    {
      for( unsigned int k = first; k < count - 3; k ++ )
      {
        const osg::Vec3& v0 ( vertices.at( k ) );
        const osg::Vec3& v1 ( vertices.at( k + 1 ) );
        const osg::Vec3& v2 ( vertices.at( k + 2 ) );

        if( bindPerVertex )
        {
          this->_addNormal( v0, normals.at( k ) );
          this->_addNormal( v1, normals.at( k + 1 ) );
          this->_addNormal( v2, normals.at( k + 2 ) );
        }
        else
        {
          osg::Vec3 v ( v0 + v1 + v2 );
          this->_addNormal( v0, normals.at( k / 3 ) );
        }
      }
    }
    break;
  default:
    break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a normal.
//
///////////////////////////////////////////////////////////////////////////////

void FindNormals::_addNormal ( const osg::Vec3& position, const osg::Vec3& normal )
{
  _vertices->push_back ( position );
  _vertices->push_back ( position + ( normal * _size ) );
}
