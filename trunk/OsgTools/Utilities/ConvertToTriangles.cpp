
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Convert other primitives (currently only tri-strips) to triangles.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Utilities/ConvertToTriangles.h"
#include "OsgTools/Visitor.h"

#include "Usul/Adaptors/MemberFunction.h"

#include "osg/Geode"
#include "osg/Geometry"

using namespace OsgTools::Utilities;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ConvertToTriangles::ConvertToTriangles()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ConvertToTriangles::~ConvertToTriangles()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert.
//
///////////////////////////////////////////////////////////////////////////////

void ConvertToTriangles::operator () ( osg::Node *node )
{
  if ( node )
  {
    // Declare the visitor.
    osg::ref_ptr<osg::NodeVisitor> visitor ( 
      OsgTools::MakeVisitor<osg::Geode>::make ( 
      Usul::Adaptors::memberFunction ( this, &ConvertToTriangles::_convert ) ) );
    
    // Visit the scene.
    node->accept ( *visitor );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert draw arrays.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  osg::PrimitiveSet* convertDrawArrays ( const osg::DrawArrays& da )
  {
    if ( osg::PrimitiveSet::TRIANGLE_STRIP == da.getMode() )
    {
      // Make a primitive set for triangles.
      osg::ref_ptr<osg::DrawElementsUInt> drawElements ( new osg::DrawElementsUInt ( GL_TRIANGLES ) );
      
      drawElements->reserve ( ( da.getCount() - 2 ) * 3 );
      
      for ( int i = da.getFirst(); i < da.getCount() - 2; ++i )
      {
        unsigned int i0, i1, i2;
        if( i % 2 )
        {
          i0 = i;
          i1 = i + 2;
          i2 = i + 1;
        }
        else
        {
          i0 = i;
          i1 = i + 1;
          i2 = i + 2;
        }
        
        drawElements->push_back ( i0 );
        drawElements->push_back ( i1 );
        drawElements->push_back ( i2 );
      }
      
      return drawElements.release();
    }
    
    // Unable to convert.
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert draw elements.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template<class DrawElements>
  osg::PrimitiveSet* convertDrawElements ( const DrawElements& de )
  {
    if ( osg::PrimitiveSet::TRIANGLE_STRIP == de.getMode() )
    {
      // Make a primitive set for triangles.
      osg::ref_ptr<osg::DrawElementsUInt> drawElements ( new osg::DrawElementsUInt ( GL_TRIANGLES ) );
      
      drawElements->reserve ( ( de.size() - 2 ) * 3 );
      
      for ( unsigned int i = 0; i < de.size() - 2; ++i )
      {
        unsigned int i0, i1, i2;
        if( i % 2 )
        {
          i0 = de.at ( i );
          i1 = de.at ( i + 2 );
          i2 = de.at ( i + 1 );
        }
        else
        {
          i0 = de.at ( i );
          i1 = de.at ( i + 1 );
          i2 = de.at ( i + 2 );
        }
        
        drawElements->push_back ( i0 );
        drawElements->push_back ( i1 );
        drawElements->push_back ( i2 );
      }
      
      return drawElements.release();
    }
    
    // Unable to convert.
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert tri-strip to triangle.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  void convertGeometry ( osg::Geometry& geometry )
  {
    typedef osg::Geometry::PrimitiveSetList PrimitiveSets;
    
    osg::ref_ptr< osg::Array > normals ( geometry.getNormalArray() );
    osg::ref_ptr< osg::Array > colors  ( geometry.getColorArray() );
    
    // Only handle per vertex normal binding.
    if ( true == normals.valid() && osg::Geometry::BIND_PER_VERTEX != geometry.getNormalBinding() )
      return;
    
    // Only handle per vertex color binding.
    if ( true == colors.valid() && osg::Geometry::BIND_PER_VERTEX != geometry.getColorBinding() )
      return;
    
    //Get the primitive set
    PrimitiveSets& prims ( geometry.getPrimitiveSetList() );

    // Go through each primitive set
    for ( unsigned int prim = 0; prim < prims.size(); ++prim )
    {
      osg::ref_ptr<osg::PrimitiveSet> oldPrimitiveSet ( prims.at ( prim ) );
      
      if ( oldPrimitiveSet.valid() )
      {   
        osg::ref_ptr<osg::PrimitiveSet> newPrimitiveSet ( 0x0 );
        
        switch ( oldPrimitiveSet->getType() )
        {
        case osg::PrimitiveSet::DrawArraysPrimitiveType:
        {
          osg::DrawArrays *da ( static_cast < osg::DrawArrays* > ( oldPrimitiveSet.get() ) );
          newPrimitiveSet = Detail::convertDrawArrays ( *da );
        }
        break;
        case osg::PrimitiveSet::DrawArrayLengthsPrimitiveType:
          // TODO
          break;
        case osg::PrimitiveSet::DrawElementsUBytePrimitiveType:
        {
          osg::DrawElementsUByte *de = static_cast < osg::DrawElementsUByte* > ( oldPrimitiveSet.get() );
          newPrimitiveSet = Detail::convertDrawElements( *de );
        }
        break;            
        case osg::PrimitiveSet::DrawElementsUShortPrimitiveType:
        {
          osg::DrawElementsUShort *de = static_cast < osg::DrawElementsUShort* > ( oldPrimitiveSet.get() );
          newPrimitiveSet = Detail::convertDrawElements( *de );
        }
        break;
        case osg::PrimitiveSet::DrawElementsUIntPrimitiveType:
        {
          osg::DrawElementsUInt *de = static_cast < osg::DrawElementsUInt* > ( oldPrimitiveSet.get() );
          newPrimitiveSet = Detail::convertDrawElements( *de );
        }
        break;
        }

        if ( newPrimitiveSet.valid() )
          prims.at ( prim ) = newPrimitiveSet.get();
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert all the geode's drawables to triangles, if possible.
//
///////////////////////////////////////////////////////////////////////////////

void ConvertToTriangles::_convert ( osg::Geode *geode )
{
  if ( geode )
  {
    for ( unsigned int i = 0; i < geode->getNumDrawables(); ++i )
    {
      osg::ref_ptr<osg::Geometry> geometry ( dynamic_cast<osg::Geometry*> ( geode->getDrawable ( i ) ) );
      if ( geometry.valid() )
      {
        Detail::convertGeometry ( *geometry );
      }
    }
  }
}
