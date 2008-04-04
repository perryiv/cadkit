
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Utilities/TranslateGeometry.h"

#include "osg/Geode"
#include "osg/Geometry"

using namespace OsgTools::Utilities;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

TranslateGeometry::TranslateGeometry ( const osg::Vec3d& amount ) :  BaseClass ( osg::NodeVisitor::TRAVERSE_ALL_CHILDREN ),
  _amount ( amount ),
  _vertices()
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

TranslateGeometry::~TranslateGeometry()
{
  // Clear the vertices.
  _vertices.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit a geode.
//
///////////////////////////////////////////////////////////////////////////////

void TranslateGeometry::apply( osg::Geode& geode )
{
  // Get the number of drawables.
  const unsigned int numDrawables ( geode.getNumDrawables() );

  // Loop through the drawables.
  for ( unsigned int i = 0; i < numDrawables; ++i )
  {
    // Get the drawable.
    osg::Drawable *drawable = geode.getDrawable ( i );
    
    if ( osg::Geometry *geometry = dynamic_cast < osg::Geometry* > ( drawable ) )
    {
      osg::ref_ptr< osg::Vec3Array > vertices ( dynamic_cast < osg::Vec3Array * > ( geometry->getVertexArray() ) );

      // If the vertices are valid and we haven't seen them yet...
      if( vertices.valid() && ( _vertices.end() == _vertices.find ( vertices ) ) )
      {
        for ( osg::Vec3Array::iterator iter = vertices->begin(); iter != vertices->end(); ++iter )
        {
          osg::Vec3d vertex ( osg::Vec3d ( *iter ) - _amount );
          (*iter).set ( static_cast<osg::Vec3f::value_type> ( vertex[0] ),
                        static_cast<osg::Vec3f::value_type> ( vertex[1] ),
                        static_cast<osg::Vec3f::value_type> ( vertex[2] ) );
        }

        // Add to the set so we don't move the vertices again.
        _vertices.insert ( vertices );
      }
    }

		drawable->dirtyBound();
  }
}
