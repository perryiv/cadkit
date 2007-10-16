
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
//  Body class.
//
///////////////////////////////////////////////////////////////////////////////

#include "StarSystem/Body.h"
#include "StarSystem/Tile.h"
#include "StarSystem/Visitor.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Trace/Trace.h"

#include "OsgTools/ShapeFactory.h"
#include "OsgTools/Group.h"

#include "ossimPlanet/ossimPlanet.h"
#include "ossimPlanet/ossimPlanetLand.h"
#include "ossimPlanet/ossimPlanetLandModel.h"

#include "osgDB/DatabasePager"

#include "osg/Group"

#include <limits>

using namespace StarSystem;

STAR_SYSTEM_IMPLEMENT_NODE_CLASS ( Body );


///////////////////////////////////////////////////////////////////////////////
//
//  Callback for setting the bounding sphere.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  class BoundingSphereCallback : public osg::Node::ComputeBoundingSphereCallback
  {
  public:

    BoundingSphereCallback ( StarSystem::Body *body ) : _body ( body )
    {
    }

    virtual osg::BoundingSphere computeBound ( const osg::Node & ) const
    {
      osg::BoundingSphere bounds ( osg::Vec3 ( 0.0f, 0.0f, 0.0f ), 1.0f );
      if ( 0x0 != _body )
      {
        const double radius ( _body->maxRadius() );
        const double maxFloat ( std::numeric_limits<float>::max() );
        if ( radius > maxFloat )
          std::cout << "Error 1058343654: bounding sphere is bigger than maximum float" << std::endl;
        const StarSystem::Body::Vec3d center ( _body->center() );
        bounds = osg::BoundingSphere ( osg::Vec3d ( center[0], center[1], center[2] ), static_cast<float> ( radius ) );
      }
      return bounds;
    }

  protected:

    virtual ~BoundingSphereCallback()
    {
    }

  private:

    StarSystem::Body *_body; // Raw pointer to avoid circular reference.
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Callback for culling.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  class CullCallback : public osg::NodeCallback
  {
  public:

    CullCallback ( StarSystem::Body *body ) : _body ( body )
    {
    }

    virtual void operator () ( osg::Node *node, osg::NodeVisitor *nv )
    {
      if ( ( 0x0 != node ) && ( 0x0 != nv ) && ( osg::NodeVisitor::CULL_VISITOR == nv->getVisitorType() ) )
      {
        _body->_buildScene ( dynamic_cast < osgUtil::CullVisitor * > ( nv ) );
        node->traverse ( *nv );
      }
    }

  protected:

    virtual ~CullCallback()
    {
    }

  private:

    StarSystem::Body *_body; // Raw pointer to avoid circular reference.
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Body::Body() : BaseClass(),
  _planet    ( new ossimPlanet() ),
  _landModel ( 0x0 ),
  _transform ( new osg::MatrixTransform ),
  _ellipsoid ( new ossimEllipsoid ( osg::WGS_84_RADIUS_EQUATOR, osg::WGS_84_RADIUS_POLAR ) )
{
  USUL_TRACE_SCOPE;

  // Not using smart pointers.
  _planet->ref();
  _transform->ref();

  // Add the planet to the transform.
  //_transform->addChild ( _planet );

  // Add a tile.
  Tile::ValidRefPtr tile ( new Tile ( osg::Vec2d ( -180, -90 ), osg::Vec2d ( 180, 90 ), this->radii()[Body::RADIUS_EQUATOR], this->radii()[Body::RADIUS_EQUATOR] * 7, _ellipsoid ) );
  _transform->addChild ( tile.get() );

  // Experimenting...
  //_transform->setCullCallback ( new Helper::CullCallback ( this ) );

  // Set the default attributes.
  _planet->getLand()->setLandType ( ossimPlanetLandType_ELLIPSOID );
  _planet->getLand()->setElevationEnabledFlag ( true );
  _planet->getLand()->setHeightExag ( 1.0 );
  _planet->getLand()->setElevationPatchSize ( 16 );
  _planet->getLand()->setMaxLevelDetail ( 32 );
  _planet->setEnableEphemerisFlag ( false );
  _planet->setEnableHudFlag ( false );
  _planet->getLand()->resetGraph();

  // Set the bounding sphere callback.
  _planet->setComputeBoundingSphereCallback ( new Helper::BoundingSphereCallback ( this ) );

  // Shortcut to the land.
  _landModel = dynamic_cast < ossimPlanetEllipsoidLandModel * > ( _planet->getLand()->getModel().get() );
  Usul::Pointers::reference ( _landModel );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Body::~Body()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Body::_destroy ), "3973302267" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy
//
///////////////////////////////////////////////////////////////////////////////

void Body::_destroy()
{
  USUL_TRACE_SCOPE;

  _planet->setComputeBoundingSphereCallback ( 0x0 );

  Usul::Pointers::unreference ( _planet );    _planet    = 0x0;
  Usul::Pointers::unreference ( _landModel ); _landModel = 0x0;
  Usul::Pointers::unreference ( _transform ); _transform = 0x0;

  delete _ellipsoid; _ellipsoid = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Node *Body::scene() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _transform;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *Body::scene()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _transform;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the center.
//
///////////////////////////////////////////////////////////////////////////////

void Body::center ( const Vec3d &c )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _transform->setMatrix ( osg::Matrix::translate ( c[0], c[1], c[2] ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the center.
//
///////////////////////////////////////////////////////////////////////////////

Body::Vec3d Body::center() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  const osg::Vec3d c ( _transform->getMatrix().getTrans() );
  return Vec3d ( c[0], c[1], c[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the maximum radius.
//
///////////////////////////////////////////////////////////////////////////////

double Body::maxRadius() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return ( ( 0x0 == _ellipsoid ) ? 1 : Usul::Math::maximum ( _ellipsoid->a(), _ellipsoid->b() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the radii.
//
///////////////////////////////////////////////////////////////////////////////

void Body::radii ( const Vec2d &r )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  if ( 0x0 != _ellipsoid )
  {
    _ellipsoid->setAB ( r[Body::RADIUS_EQUATOR], r[Body::RADIUS_POLAR] );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the radii.
//
///////////////////////////////////////////////////////////////////////////////

Body::Vec2d Body::radii() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return ( ( 0x0 == _ellipsoid ) ? Vec2d ( 1, 1 ) : Vec2d ( _ellipsoid->a(), _ellipsoid->b() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Body::_buildScene ( osgUtil::CullVisitor *cv )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Handle null visitor.
  if ( 0x0 == cv )
    return;

  // Remove all children in the group.
  //OsgTools::Group::removeAllChildren ( _transform );
}
