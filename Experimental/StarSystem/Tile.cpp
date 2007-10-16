
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A recursive tile.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

#include "StarSystem/Tile.h"

#include "OsgTools/Mesh.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Adaptors/Random.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Trace/Trace.h"

#include "ossim/base/ossimEllipsoid.h"

#include "osg/Geode"
#include "osg/Material"

using namespace StarSystem;

USUL_IMPLEMENT_TYPE_ID ( Tile );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Tile::Tile ( osg::Vec2d &mn, osg::Vec2d &mx, 
             double elevation, double distance, 
             ossimEllipsoid *ellipsoid ) : BaseClass(),
  _min ( mn ),
  _max ( mx ),
  _elevation ( elevation ),
  _distance ( distance ),
  _ellipsoid ( ellipsoid )
{
  USUL_TRACE_SCOPE;
  this->_init();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Tile::Tile ( const Tile &tile, const osg::CopyOp &option ) : BaseClass ( tile, option ),
  _min ( tile._min ),
  _max ( tile._max ),
  _elevation ( tile._elevation ),
  _distance ( tile._distance ),
  _ellipsoid ( tile._ellipsoid )
{
  USUL_TRACE_SCOPE;
  this->_init();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Tile::~Tile()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Tile::_destroy ), "2021499927" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a random material
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  osg::Material* material()
  {
    osg::Vec4 emissive ( 0.0f, 0.0f, 0.0f, 1.0f );
    osg::Vec4 specular ( 0.2f, 0.2f, 0.2f, 1.0f );

    Usul::Adaptors::Random < float > rd ( 0.2f, 1.0f );

	  osg::Vec4 diffuse ( rd(), rd(), rd(), 1.0f );
    osg::Vec4 ambient ( diffuse );

    osg::ref_ptr<osg::Material> mat ( new osg::Material );
	  mat->setAmbient   ( osg::Material::FRONT_AND_BACK, ambient  );
	  mat->setDiffuse   ( osg::Material::FRONT_AND_BACK, diffuse  );
	  mat->setEmission  ( osg::Material::FRONT_AND_BACK, emissive );
	  mat->setSpecular  ( osg::Material::FRONT_AND_BACK, specular );
    mat->setShininess ( osg::Material::FRONT_AND_BACK, 100      );

    return mat.release();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the tile.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::_init()
{
  USUL_TRACE_SCOPE;

  // handle no ellipsoid.
  if ( 0x0 == _ellipsoid )
    return;

  // Add internal geometry.
  OsgTools::Mesh mesh ( 10, 10 );
  for ( unsigned int i = 0; i < mesh.rows(); ++i )
  {
    const double u ( static_cast<double> ( i ) / ( mesh.rows() - 1 ) );
    for ( unsigned int j = 0; j < mesh.columns(); ++j )
    {
      const double v ( static_cast<double> ( j ) / ( mesh.columns() - 1 ) );
      const double lon ( _min[0] + u * ( _max[0] - _min[0] ) );
      const double lat ( _min[1] + v * ( _max[1] - _min[1] ) );

      // Convert lat-lon coordinates to xyz.
      osg::Vec3f &p ( mesh.point ( i, j ) );
      double x ( 0 ), y ( 0 ), z ( 0 );
      _ellipsoid->latLonHeightToXYZ ( lat, lon, _elevation, x, y, z );
      p[0] = static_cast<float> ( x );
      p[1] = static_cast<float> ( y );
      p[2] = static_cast<float> ( z );

      // Assign normal vectors.
      osg::Vec3f &n ( mesh.normal ( i, j ) );
      n = p; // Minus the center, which is (0,0,0).
      n.normalize();
    }
  }
  this->addChild ( mesh() );

  // Assign material.
  this->getOrCreateStateSet()->setAttributeAndModes ( Helper::material(), osg::StateAttribute::PROTECTED );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy
//
///////////////////////////////////////////////////////////////////////////////

void Tile::_destroy()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse the children.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::traverse ( osg::NodeVisitor &nv )
{
  USUL_TRACE_SCOPE;

  // If this is the cull-visitor.
  if ( osg::NodeVisitor::CULL_VISITOR == nv.getVisitorType() )
  {
    const osg::Vec3f &center ( this->getBound().center() );
    const double dist ( nv.getDistanceToViewPoint ( center, true ) );
    const bool low ( ( dist > _distance ) );
    const unsigned int numChildren ( this->getNumChildren() );
    USUL_ASSERT ( numChildren > 0 );

    if ( low )
    {
      // Remove high level of detail.
      this->removeChild ( 1, numChildren - 1 );

      // Traverse low level of detail.
      this->getChild ( 0 )->accept ( nv );
    }

    else
    {
      // Add high level if necessary.
      if ( 1 == numChildren )
      {
        osg::ref_ptr<osg::Group> group ( new osg::Group );
        const double half ( _distance * 0.5f );
        const osg::Vec2d mid ( ( _max + _min ) * 0.5 );
        group->addChild ( new Tile ( osg::Vec2d ( _min[0], _min[1] ), osg::Vec2d (  mid[0],  mid[1] ), _elevation, half, _ellipsoid ) ); // lower left  tile
        group->addChild ( new Tile ( osg::Vec2d (  mid[0], _min[1] ), osg::Vec2d ( _max[0],  mid[1] ), _elevation, half, _ellipsoid ) ); // lower right tile
        group->addChild ( new Tile ( osg::Vec2d ( _min[0],  mid[1] ), osg::Vec2d (  mid[0], _max[1] ), _elevation, half, _ellipsoid ) ); // upper left  tile
        group->addChild ( new Tile ( osg::Vec2d (  mid[0],  mid[1] ), osg::Vec2d ( _max[0], _max[1] ), _elevation, half, _ellipsoid ) ); // upper right tile
        this->addChild ( group.get() );
      }

      // Traverse last child.
      this->getChild ( this->getNumChildren() - 1 )->accept ( nv );
    }
  }

  // Not the cull-visitor.
  else
  {
    BaseClass::traverse ( nv );
  }
}
