
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "SortBackToFront.h"

#include "Usul/Errors/Assert.h"

#include "osg/Geometry"
#include "osgUtil/CullVisitor"

#include <iostream>
#include <algorithm>

using namespace OsgTools::Callbacks;

namespace Detail
{


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Triangle structure to hold the indices in the vertex list.
  //
  ///////////////////////////////////////////////////////////////////////////////

  struct Triangle
  {
  public:
    Triangle ( unsigned int i0, unsigned int i1, unsigned int i2, const osg::Vec3Array& vertices ) :
      _index0 ( i0 ),
      _index1 ( i1 ),
      _index2 ( i2 ),
      _center ()
    {
      _center = vertices.at( _index0 ) + vertices.at( _index1 ) + vertices.at( _index2 );
      _center /= 3;
    }

    Triangle ( const Triangle &rhs ) :
      _index0 ( rhs._index0 ),
      _index1 ( rhs._index1 ),
      _index2 ( rhs._index2 ),
      _center ( rhs._center )
    {
    }

    Triangle& operator= ( const Triangle &rhs )
    {
      this->_index0 = rhs._index0;
      this->_index1 = rhs._index1;
      this->_index2 = rhs._index2;
      this->_center = rhs._center;

      return *this;
    }

    /*bool operator< ( const Triangle &t ) const
    {
      return _center < t._center;
    }*/

    double length2 ( const osg::Vec3& eye ) const
    {
      return ( eye - _center ).length2();
    }

    unsigned int index0 () const { return _index0; }
    unsigned int index1 () const { return _index1; }
    unsigned int index2 () const { return _index2; }

  private:
    unsigned int _index0;
    unsigned int _index1;
    unsigned int _index2;
    osg::Vec3    _center;
  };


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Quad structure to hold the indices in the vertex list.
  //
  ///////////////////////////////////////////////////////////////////////////////

  struct Quad
  {
  public:
    Quad ( unsigned int i0, unsigned int i1, unsigned int i2, unsigned int i3, const osg::Vec3Array* vertices ) :
      _index0 ( i0 ),
      _index1 ( i1 ),
      _index2 ( i2 ),
      _index3 ( i3 ),
      _center (),
      _vertices ( vertices )
    {
      _center = _vertices->at( _index0 ) + _vertices->at( _index1 ) + _vertices->at( _index2 ) + _vertices->at ( _index3 );
      _center /= 4;
    }

    Quad ( const Quad &rhs ) :
      _index0 ( rhs._index0 ),
      _index1 ( rhs._index1 ),
      _index2 ( rhs._index2 ),
      _index3 ( rhs._index3 ),
      _center ( rhs._center ),
      _vertices ( rhs._vertices )
    {
    }


    /* bool operator< ( const Quad &t ) const
    {
      const osg::Vec3& v1 ( _vertices->at( _index0 ) );
      const osg::Vec3& v2 ( _vertices->at( t._index0 ) );

      return v1[2] < v2[2];
      }*/

    Quad& operator= ( const Quad &rhs )
    {
      this->_index0 = rhs._index0;
      this->_index1 = rhs._index1;
      this->_index2 = rhs._index2;
      this->_index3 = rhs._index3;
      this->_vertices = rhs._vertices;
      this->_center = rhs._center;

      return *this;
    }

    double length2 ( const osg::Vec3& eye ) const
    {
      return (eye - _center ).length2();
    }

    unsigned int index0 () const { return _index0; }
    unsigned int index1 () const { return _index1; }
    unsigned int index2 () const { return _index2; }
    unsigned int index3 () const { return _index3; }

  private:
    unsigned int _index0;
    unsigned int _index1;
    unsigned int _index2;
    unsigned int _index3;
    osg::Vec3 _center;

    osg::ref_ptr < const osg::Vec3Array > _vertices;
  };


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Predicate to sort back to front.
  //
  ///////////////////////////////////////////////////////////////////////////////

  struct BackToFront
  {
    BackToFront ( const osg::Vec3& eye ) :
    _eye ( eye )
    {
    }

    ///////////////////////////////////////////////////////////////////////////////
    //
    //  Return true if a is farther to the eye than b.
    //
    ///////////////////////////////////////////////////////////////////////////////


    template < class Primitive > bool operator () ( const Primitive &a, const Primitive &b ) const
    {
      double da ( a.length2 ( _eye ) );
      double db ( b.length2 ( _eye ) );

      return ( da > db );
    }

  private:
    const osg::Vec3& _eye;
  };



  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Predicate to sort back to front.
  //
  ///////////////////////////////////////////////////////////////////////////////

  struct BackToFrontGeometry
  {
    BackToFrontGeometry ( const osg::Vec3& eye ) :
    _eye ( eye )
    {
    }

    ///////////////////////////////////////////////////////////////////////////////
    //
    //  Return true if a is farther to the eye than b.
    //
    ///////////////////////////////////////////////////////////////////////////////


    template < class GeometryPtr > bool operator () ( const GeometryPtr &a, const GeometryPtr &b ) const
    {
      osg::BoundingBox bb1 ( a->getBound() );
      osg::BoundingBox bb2 ( b->getBound() );

      double da ( (_eye - bb1.center()).length2 ( ) );
      double db ( (_eye - bb2.center()).length2 ( ) );

      return ( da > db );
    }

  private:
    const osg::Vec3& _eye;
  };


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Predicate to sort back to front.
  //
  ///////////////////////////////////////////////////////////////////////////////

  struct BackToFrontNode
  {
    BackToFrontNode ( const osg::Vec3& eye ) :
    _eye ( eye )
    {
    }

    ///////////////////////////////////////////////////////////////////////////////
    //
    //  Return true if a is farther to the eye than b.
    //
    ///////////////////////////////////////////////////////////////////////////////

    template < class NodePtr > bool operator () ( const NodePtr &a, const NodePtr &b ) const
    {
      osg::BoundingSphere bs1 ( a->getBound() );
      osg::BoundingSphere bs2 ( b->getBound() );

      double da ( (_eye - bs1.center()).length2 ( ) );
      double db ( (_eye - bs2.center()).length2 ( ) );

      return ( da > db );
    }

  private:
    const osg::Vec3& _eye;
  };


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Sort a draw arrays vector from back to front.
  //
  ///////////////////////////////////////////////////////////////////////////////

  void sortDrawArrays ( osg::DrawArrays & da, const osg::Vec3& eye, osg::Vec3Array& vertices, osg::Vec3Array& normals, bool bindPerVertex, bool copyNormals )
  {
    switch ( da.getMode() )
    {
    case osg::PrimitiveSet::TRIANGLES:
      {
        typedef std::vector< Triangle > Triangles;
        Triangles triangles;
        
        triangles.reserve ( da.getNumPrimitives() );

        for ( int i = da.getFirst(); i < da.getCount(); i += 3 )
        {
          Triangle t ( i, i + 1, i + 2, vertices );

          triangles.push_back( t );
        }

        // Sort
        std::sort ( triangles.begin(), triangles.end(), BackToFront( eye ) );

        // Make a copy
        osg::ref_ptr < osg::Vec3Array > vcopy ( new osg::Vec3Array ( vertices.size() ) );
        osg::ref_ptr < osg::Vec3Array > ncopy ( new osg::Vec3Array ( normals.size() ) );

        std::copy ( vertices.begin(), vertices.end(), vcopy->begin() );
        std::copy ( normals.begin(),  normals.end(),  ncopy->begin() );

        // Populate the vertices
        unsigned int current ( 0 );
        for( Triangles::iterator i = triangles.begin(); i != triangles.end(); ++i, current += 3 )
        {
          vertices.at ( current )     = vcopy->at ( i->index0() );
          vertices.at ( current + 1 ) = vcopy->at ( i->index1() );
          vertices.at ( current + 2 ) = vcopy->at ( i->index2() );

          if ( copyNormals )
          {
            if ( bindPerVertex )
            {
              normals.at ( current )     = ncopy->at ( i->index0() );
              normals.at ( current + 1 ) = ncopy->at ( i->index1() );
              normals.at ( current + 2 ) = ncopy->at ( i->index2() );
            }
            else
            {
              normals.at ( current / 3 )     = ncopy->at ( i->index0() / 3 );
            }
          }

        }
      }
      break;
    case osg::PrimitiveSet::QUADS:
      {
        typedef std::vector< Quad > Quads;
        Quads quads;
        
        quads.reserve ( da.getNumPrimitives() );

        for ( int i = da.getFirst(); i < da.getCount(); i += 4 )
        {
          Quad t ( i, i + 1, i + 2, i + 3, &vertices );

          quads.push_back( t );
        }

        // Sort
        std::sort ( quads.begin(), quads.end(), BackToFront( eye ) );

        // Make a copy
        osg::ref_ptr < osg::Vec3Array > vcopy ( new osg::Vec3Array ( vertices.size() ) );
        osg::ref_ptr < osg::Vec3Array > ncopy ( new osg::Vec3Array ( normals.size() ) );

        std::copy ( vertices.begin(), vertices.end(), vcopy->begin() );
        std::copy ( normals.begin(),  normals.end(),  ncopy->begin() );

        // Populate the vertices
        unsigned int current ( 0 );
        for( Quads::iterator i = quads.begin(); i != quads.end(); ++i, current += 4 )
        {
          vertices.at ( current )     = vcopy->at ( i->index0() );
          vertices.at ( current + 1 ) = vcopy->at ( i->index1() );
          vertices.at ( current + 2 ) = vcopy->at ( i->index2() );
          vertices.at ( current + 3 ) = vcopy->at ( i->index3() );

          if ( bindPerVertex )
          {
            normals.at ( current )     = ncopy->at ( i->index0() );
            normals.at ( current + 1 ) = ncopy->at ( i->index1() );
            normals.at ( current + 2 ) = ncopy->at ( i->index2() );
            normals.at ( current + 3 ) = ncopy->at ( i->index3() );
          }
          else
          {
            normals.at ( current / 4 )     = ncopy->at ( i->index0() / 4 );
          }
        }
      }
      break;
    }
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Sort a draw elements vector from back to front.
  //
  ///////////////////////////////////////////////////////////////////////////////

  template < class DrawElements >
  void sortDrawElements ( DrawElements& de, const osg::Vec3& eye, const osg::Vec3Array& vertices )
  {
    switch ( de.getMode() )
    {
    case osg::PrimitiveSet::TRIANGLES:
      {
        typedef std::vector< Triangle > Triangles;
        Triangles triangles;
        
        triangles.reserve ( de.getNumPrimitives() );

        // Build the triangle vector to sort
        for ( typename DrawElements::iterator i = de.begin(); i != de.end(); i += 3 )
        {
          Triangle t ( *i, *(i + 1), *(i + 2 ), vertices );
          triangles.push_back( t );
        }

        // Sort
        std::sort ( triangles.begin(), triangles.end(), BackToFront( eye ) );

        // Populate the draw elements
        unsigned int current ( 0 );
        for( Triangles::iterator i = triangles.begin(); i != triangles.end(); ++i, current += 3 )
        {
          de.at ( current )     = i->index0();
          de.at ( current + 1 ) = i->index1();
          de.at ( current + 2 ) = i->index2();
        }
      }
      break;
    case osg::PrimitiveSet::QUADS:
      {
        typedef std::vector< Quad > Quads;
        Quads quads;
        
        quads.reserve ( de.getNumPrimitives() );

        // Build the triangle vector to sort
        for ( typename DrawElements::iterator i = de.begin(); i != de.end(); i += 4 )
        {
          Quad t ( *i, *(i + 1), *(i + 2), *(i + 3), &vertices );

          quads.push_back( t );
        }

        // Sort
        std::sort ( quads.begin(), quads.end(), BackToFront( eye ) );

        // Populate the draw elements
        unsigned int current ( 0 );
        for( Quads::iterator i = quads.begin(); i != quads.end(); ++i, current += 4 )
        {
          de.at ( current )     = i->index0();
          de.at ( current + 1 ) = i->index1();
          de.at ( current + 2 ) = i->index2();
          de.at ( current + 3 ) = i->index3();
        }
      }
      break;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SortBackToFront::SortBackToFront() : BaseClass ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SortBackToFront::~SortBackToFront()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Do the sorting of the vertices.
//
///////////////////////////////////////////////////////////////////////////////

void SortBackToFront::operator () ( osg::Node* node, osg::NodeVisitor *nv )
{
  // Return now if we don't have a valid node visitor or if it's not a cull visitor.
  if ( 0x0 == nv || osg::NodeVisitor::CULL_VISITOR != nv->getVisitorType() )
    return;
  
  // We know it's a cull visitor.
  osgUtil::CullVisitor *cv ( static_cast< osgUtil::CullVisitor* > ( nv ) );
  
  // Get the eye position.
  const osg::Vec3& eye ( cv->getEyePoint() );
  
  // is the node a group?
  if ( osg::Group *group = dynamic_cast<osg::Group*> ( node ) )
  {
    const unsigned int number ( group->getNumChildren() );
    
    // Make a copy of the children.
    osg::NodeList children ( number );
    for ( unsigned int i = 0; i < number; ++i )
      children.at ( i ) = group->getChild ( i );
    
    // Sort back to front.
    std::sort ( children.begin(), children.end(), Detail::BackToFrontNode ( eye ) );
    
    // Set the children back to the group.
    for ( unsigned int i = 0; i < number; ++i )
       group->setChild ( i, children.at ( i ).get() );
  }

  // Is the node a geode?
  else if ( osg::Geode *geode = dynamic_cast < osg::Geode* > ( node ) )
  {
    // Get all the drwables
    osg::Geode::DrawableList& drawables ( const_cast < osg::Geode::DrawableList& > ( geode->getDrawableList() ) );
    
    // Sort the geometies.
    std::sort ( drawables.begin(), drawables.end(), Detail::BackToFrontGeometry( eye ) );

    // For each drawable
    for ( unsigned int i = 0; i < drawables.size(); ++i )
    {
      // Is the drawable a geometry?
      if( osg::Geometry *geometry = drawables.at ( i )->asGeometry() )
      {
        // Don't bother sorting if the geometry is culled.
        if( cv )
        {
          const osg::BoundingBox &bb = geometry->getBound();
          if ( cv->isCulled( bb ) )
            continue;
        }
        typedef osg::Geometry::PrimitiveSetList PrimitiveSetList;

        //Get the primitive set
        PrimitiveSetList& prims ( geometry->getPrimitiveSetList() );

        // Get the vertices
        osg::ref_ptr< osg::Vec3Array > vertices ( dynamic_cast < osg::Vec3Array* > ( geometry->getVertexArray () ) );

        // Should be true.  
        // Currently don't use anything other than osg::Vec3Arrays.  
        // Putting this here so if that changes we know what needs fixes.
        USUL_ASSERT ( vertices != 0x0 );

        osg::ref_ptr< osg::Vec3Array > normals  ( dynamic_cast < osg::Vec3Array * > ( geometry->getNormalArray() ) );
        const bool copyNoramls ( normals.valid() );

        // Create an empty normal array if normals is null.
        if ( false == copyNoramls )
          normals = new osg::Vec3Array;

        // Go through each primitive set
        for ( PrimitiveSetList::iterator i = prims.begin(); i != prims.end(); ++i )
        {
          switch ( (*i)->getType() )
          {
          case osg::PrimitiveSet::DrawArraysPrimitiveType:
            {
              osg::DrawArrays *da = static_cast < osg::DrawArrays* > ( i->get() );
              Detail::sortDrawArrays ( *da, eye, *vertices, *normals, geometry->getNormalBinding() == osg::Geometry::BIND_PER_VERTEX, copyNoramls );
            }
            break;
          case osg::PrimitiveSet::DrawArrayLengthsPrimitiveType:
            // TODO
            break;
          case osg::PrimitiveSet::DrawElementsUBytePrimitiveType:
            {
              osg::DrawElementsUByte *de = static_cast < osg::DrawElementsUByte* > ( i->get() );
              Detail::sortDrawElements( *de, eye, *vertices );
            }
            break;
            
          case osg::PrimitiveSet::DrawElementsUShortPrimitiveType:
            {
              osg::DrawElementsUShort *de = static_cast < osg::DrawElementsUShort* > ( i->get() );
              Detail::sortDrawElements( *de, eye, *vertices );
            }
            break;
          case osg::PrimitiveSet::DrawElementsUIntPrimitiveType:
            {
              osg::DrawElementsUInt *de = static_cast < osg::DrawElementsUInt* > ( i->get() );
              Detail::sortDrawElements( *de, eye, *vertices );
            }
            break;
          }
        }

        // Dirty the display list.
        geometry->dirtyDisplayList();
      }
    }
  }

  traverse(node,nv);
}

