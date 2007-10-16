
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
#include "Usul/Math/MinMax.h"
#include "Usul/Trace/Trace.h"

#include "ossim/base/ossimEllipsoid.h"

#include "osgUtil/CullVisitor"

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
             unsigned int numRows, unsigned int numColumns,
             double elevation, double distance, 
             ossimEllipsoid *ellipsoid ) : BaseClass(),
  _ellipsoid ( ellipsoid ),
  _min ( mn ),
  _max ( mx ),
  _elevation ( elevation ),
  _distance ( distance ),
  _mesh ( new OsgTools::Mesh ( numRows, numColumns ) )
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
  _ellipsoid ( tile._ellipsoid ),
  _min ( tile._min ),
  _max ( tile._max ),
  _elevation ( tile._elevation ),
  _distance ( tile._distance ),
  _mesh ( new OsgTools::Mesh ( tile._mesh->rows(), tile._mesh->columns() ) )
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

  // Handle bad state.
  if ( ( 0x0 == _ellipsoid ) || ( 0x0 == _mesh ) )
    return;

  // Add internal geometry.
  OsgTools::Mesh &mesh ( *_mesh );
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
  _ellipsoid = 0x0; // Don't delete!
  delete _mesh; _mesh = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse the children.
//
///////////////////////////////////////////////////////////////////////////////

void Tile::traverse ( osg::NodeVisitor &nv )
{
  USUL_TRACE_SCOPE;

  // Handle bad state.
  if ( ( 0x0 == _mesh ) || ( _mesh->rows() < 2 ) || ( _mesh->columns() < 2 ) )
    return;

  // If this is the cull-visitor.
  if ( osg::NodeVisitor::CULL_VISITOR == nv.getVisitorType() )
  {
    const osgUtil::CullVisitor &cv ( dynamic_cast < osgUtil::CullVisitor & > ( nv ) );

    // Four corners and center of the tile.
    const osg::Vec3f &eye ( cv.getViewPointLocal() );
    const osg::Vec3f &p00 ( _mesh->point ( 0, 0 ) );
    const osg::Vec3f &p0N ( _mesh->point ( 0, _mesh->columns() - 1 ) );
    const osg::Vec3f &pN0 ( _mesh->point ( _mesh->rows() - 1, 0 ) );
    const osg::Vec3f &pNN ( _mesh->point ( _mesh->rows() - 1, _mesh->columns() - 1 ) );
    const osg::Vec3f &pBC ( this->getBound().center() );

    // Squared distances from the eye to t he points.
    const float dist00 ( ( eye - p00 ).length2() );
    const float dist0N ( ( eye - p0N ).length2() );
    const float distN0 ( ( eye - pN0 ).length2() );
    const float distNN ( ( eye - pNN ).length2() );
    const float distBC ( ( eye - pBC ).length2() );

    // Check with smallest distance.
    const float dist ( Usul::Math::minimum ( dist00, dist0N, distN0, distNN, distBC ) );
    const bool low ( ( dist > ( _distance * _distance ) ) );
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
        group->addChild ( new Tile ( osg::Vec2d ( _min[0], _min[1] ), osg::Vec2d (  mid[0],  mid[1] ), _mesh->rows(), _mesh->columns(), _elevation, half, _ellipsoid ) ); // lower left  tile
        group->addChild ( new Tile ( osg::Vec2d (  mid[0], _min[1] ), osg::Vec2d ( _max[0],  mid[1] ), _mesh->rows(), _mesh->columns(), _elevation, half, _ellipsoid ) ); // lower right tile
        group->addChild ( new Tile ( osg::Vec2d ( _min[0],  mid[1] ), osg::Vec2d (  mid[0], _max[1] ), _mesh->rows(), _mesh->columns(), _elevation, half, _ellipsoid ) ); // upper left  tile
        group->addChild ( new Tile ( osg::Vec2d (  mid[0],  mid[1] ), osg::Vec2d ( _max[0], _max[1] ), _mesh->rows(), _mesh->columns(), _elevation, half, _ellipsoid ) ); // upper right tile
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
