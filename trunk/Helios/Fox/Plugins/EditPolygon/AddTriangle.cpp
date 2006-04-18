
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "AddTriangle.h"

#include "FoxTools/Cursors/Factory.h"
#include "FoxTools/Headers/Cursor.h"
#include "FoxTools/Headers/App.h"

#include "OsgTools/MaterialSetter.h"
#include "OsgTools/State/StateSet.h"
#include "OsgTools/Ray.h"
#include "OsgTools/Triangles/SharedVertex.h"
#include "OsgTools/Triangles/Triangle.h"

#include "Usul/Math/MinMax.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Interfaces/ISceneIntersect.h"
#include "Usul/Interfaces/IGetDocument.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Interfaces/ISendMessage.h"
#include "Usul/Interfaces/IGroup.h"
#include "Usul/Interfaces/ITriangle.h"
#include "Usul/Interfaces/IGetDocument.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Interfaces/IAddTriangle.h"
#include "Usul/Interfaces/IGetBoundingBox.h"
#include "Usul/Interfaces/ITriangleSV.h"
#include "Usul/Interfaces/IGetVertex.h"
#include "Usul/Interfaces/IGetTriangleNormal.h"

#include "osgUtil/IntersectVisitor"

#include "osg/ShapeDrawable"
#include "osg/Geometry"
#include "osg/Geode"
#include "osg/Group"
#include "osg/Vec2"
#include "osg/BoundingBox"
#include "osg/AutoTransform"

#include <algorithm>


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( AddTriangle , AddTriangle::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Group names used below.
//
///////////////////////////////////////////////////////////////////////////////

namespace Labels
{
  const std::string CLOSEST_VERTEX ( "closest vertex" );
  const std::string ADD_TRIANGLE   ( "add triangle"   );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AddTriangle::AddTriangle() : BaseClass(),
  _cursor ( FoxTools::Cursors::Factory::create ( FoxTools::Cursors::Factory::CURSOR_GUN_SIGHT ) ),
  _vertices()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

AddTriangle::~AddTriangle()
{
  delete _cursor;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *AddTriangle::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IFoxEvent::IID:
    return static_cast< Usul::Interfaces::IFoxEvent* >(this);
  case Usul::Interfaces::ISetCursor::IID:
    return static_cast< Usul::Interfaces::ISetCursor*>(this);
  case Usul::Interfaces::ICleanUp::IID:
    return static_cast< Usul::Interfaces::ICleanUp*>(this);
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process mouse motion.
//
///////////////////////////////////////////////////////////////////////////////

void AddTriangle::_processMouseMotion ( const osgUtil::Hit &hit, Usul::Interfaces::IUnknown *caller )
{
  // Get needed interfaces.
  Usul::Interfaces::IGroup::ValidQueryPtr group ( caller );

  // Get the group.
  osg::ref_ptr < osg::Group > scene ( group->getGroup ( Labels::CLOSEST_VERTEX ) );

  // Get the closest vertex to the mouse cursor.
  VertexPair vertex ( this->_closestVertex ( hit, caller ) );

  // Make the sphere.
  osg::ref_ptr < osg::Node > sphere ( this->_makeSphere ( vertex.first, 10, osg::Vec4f ( 1.0, 1.0, 0.0, 1.0 ) ) );

  // Add the geode to the scene.
  scene->addChild ( sphere.get() );

  // Add decorations for the neighbors.
  scene->addChild ( this->_buildNeighborTriangles ( vertex.second.get(), caller ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process left-button release.
//
///////////////////////////////////////////////////////////////////////////////

void AddTriangle::_processLeftRelease ( const osgUtil::Hit &hit, Usul::Interfaces::IUnknown *caller )
{
  // Get needed interfaces.
  Usul::Interfaces::IGroup::ValidQueryPtr group ( caller );

  // Never more than three.
  if ( 3 == _vertices.size() )
    _vertices.pop_front();

  // Get the closest vertex to the mouse cursor.
  VertexPair vertex ( this->_closestVertex ( hit, caller ) );

  // Add the vertex to the list if it isn't in there already.
  if ( _vertices.end() == std::find ( _vertices.begin(), _vertices.end(), vertex ) )
    _vertices.push_back ( vertex );

  // Otherwise, permit the user to undo a mistaken click.
  else
    _vertices.remove ( vertex );

  // Remove the whole group.
  group->removeGroup ( Labels::ADD_TRIANGLE );

  // Get the scene.
  osg::ref_ptr < osg::Group > scene ( group->getGroup ( Labels::ADD_TRIANGLE ) );

  // Add a sphere for each vertex.
  for ( Vertices::iterator i = _vertices.begin(); i != _vertices.end(); ++i )
    scene->addChild ( this->_makeSphere ( i->first, 10, osg::Vec4f ( 0.0, 0.0, 1.0, 1.0 ) ) );

  // Process the vertices.
  this->_processVertices ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process the three vertices that have been clicked.
//
///////////////////////////////////////////////////////////////////////////////

void AddTriangle::_processVertices ( Usul::Interfaces::IUnknown *caller )
{
  // Get the interface
  Usul::Interfaces::IGetDocument::ValidQueryPtr getDoc ( caller );
  Usul::Interfaces::IGroup::ValidQueryPtr group ( caller );
  Usul::Interfaces::IDocument::ValidQueryPtr document ( getDoc->getDocument() );
  Usul::Interfaces::IAddTriangleWithSharedVertex::ValidQueryPtr triangle ( document );

  // If we do not have three then return.
  if ( _vertices.size() < 3 )
    return;

  // Get the three vertices of the new triangle.
  Vertices::const_iterator vertices ( _vertices.begin() );
  VertexPair v0 ( *vertices ); ++vertices;
  VertexPair v1 ( *vertices ); ++vertices;
  VertexPair v2 ( *vertices );

  // Should be true.
  USUL_ASSERT ( v0.second != v1.second && v0.second != v2.second && v1.second != v2.second );

  // Calculate the normal.
  const osg::Vec3 t1 ( v1.first - v0.first );
  const osg::Vec3 t2 ( v2.first - v0.first );
  osg::Vec3 norm ( t1 ^ t2 );
  norm.normalize();

  // Add the triangle
  OsgTools::Triangles::Triangle::RefPtr t ( triangle->addTriangle ( v0.second.get(), v1.second.get(), v2.second.get(), norm, true ) );

  // This is not an "original" triangle.
  if ( t.valid() )
    t->original ( false );

  // Clear the vertices.
  _vertices.clear();

  // Remove the decorations from the scene.
  group->removeGroup ( Labels::ADD_TRIANGLE );
  group->removeGroup ( Labels::CLOSEST_VERTEX );

  // The document is now modified.
  document->modified ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  If click intersects the scene, remove primitive
//
///////////////////////////////////////////////////////////////////////////////

bool AddTriangle::execute ( Usul::Interfaces::IUnknown *caller, bool left, bool middle, bool right, bool motion, float x, float y, float z )
{
  // Get needed interfaces.
  Usul::Interfaces::ISceneIntersect::ValidQueryPtr intersector ( caller );
  Usul::Interfaces::IGetDocument::ValidQueryPtr getDoc ( caller );
  Usul::Interfaces::IDocument::ValidQueryPtr document ( getDoc->getDocument() );
  Usul::Interfaces::ISendMessage::ValidQueryPtr send ( document );
  Usul::Interfaces::IGroup::ValidQueryPtr group ( caller );

  // Remove the sphere from the scene.
  group->removeGroup ( Labels::CLOSEST_VERTEX );

  // See if the mouse click intersects the scene.
  osgUtil::Hit hit;
  if ( false == intersector->intersect ( x, y, hit ) )
    return false;

  // Is the mouse moving?
  if ( motion )
    this->_processMouseMotion ( hit, caller );

  // Was the left-mouse button was released?
  if ( left )
    this->_processLeftRelease ( hit, caller );

  // Let everybody know.
  send->sendMessage ( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );

  // Always return false.    <-- Why? TODO
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return triangles that hover over the neighbors.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *AddTriangle::_buildNeighborTriangles ( const SharedVertex *sv, Usul::Interfaces::IUnknown *caller ) const
{
  // Get needed interfaces.
  Usul::Interfaces::IGetDocument::ValidQueryPtr getDoc ( caller );
  Usul::Interfaces::IDocument::ValidQueryPtr document ( getDoc->getDocument() );

  // Group to hold all triangles.
  osg::ref_ptr < osg::Group > group ( new osg::Group );

  // Get needed interface.
  Usul::Interfaces::IGetVertex::QueryPtr gv ( document );
  Usul::Interfaces::IGetTriangleNormal::QueryPtr gn ( document );
  if ( false == gv.valid() || false == gn.valid() )
    return group.release();

  // Loop through the triangles belonging to the given shared-vertex.
  for ( SharedVertex::ConstTriangleItr i = sv->begin(); i != sv->end(); ++i )
  {
    // Get the triangle.
    const OsgTools::Triangles::Triangle *t ( *i );

    // Get the vertices of this triangle.
    const osg::Vec3f v0 ( gv->getVertex ( t->vertex0()->index() ) );
    const osg::Vec3f v1 ( gv->getVertex ( t->vertex1()->index() ) );
    const osg::Vec3f v2 ( gv->getVertex ( t->vertex2()->index() ) );

    // Get the normal vector.
    const osg::Vec3f n ( gn->getTriangleNormal ( t->index() ) );

    // Make the decorations.
    group->addChild ( this->_makeDecoration ( v0, v1, v2,  n ) );
    group->addChild ( this->_makeDecoration ( v0, v1, v2, -n ) );
  }

  // Return the group.
  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the cursor
//
///////////////////////////////////////////////////////////////////////////////

FX::FXCursor* AddTriangle::getCursor()
{
  return _cursor;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the vertex closest to the mouse.
//
///////////////////////////////////////////////////////////////////////////////

AddTriangle::VertexPair AddTriangle::_closestVertex ( const osgUtil::Hit& hit, Usul::Interfaces::IUnknown *caller )
{
  Usul::Interfaces::IGetDocument::ValidQueryPtr getDoc ( caller );
  Usul::Interfaces::IDocument::ValidQueryPtr document ( getDoc->getDocument() );
  Usul::Interfaces::ITriangle::ValidQueryPtr triangle ( document );
  Usul::Interfaces::ITriangleSV::ValidQueryPtr triangleSV ( document );

  // Get the intersection point
  osg::Vec3 intersectPoint ( hit.getWorldIntersectPoint() );

  // Get the index
  unsigned int index ( hit._primitiveIndex );
  const osg::Drawable *drawable ( hit._drawable.get() );
  const osg::Geode *geode ( hit._geode.get() );

  // Get the vertices of the triangle.
  const osg::Vec3f& p0 ( triangle->vertex0 ( geode, drawable, index ) );
  const osg::Vec3f& p1 ( triangle->vertex1 ( geode, drawable, index ) );
  const osg::Vec3f& p2 ( triangle->vertex2 ( geode, drawable, index ) );

  // Get the difference between the triangle's vertices and the intersect point.
  const osg::Vec3f v0 ( p0 - intersectPoint );
  const osg::Vec3f v1 ( p1 - intersectPoint );
  const osg::Vec3f v2 ( p2 - intersectPoint );

  // Find the distance between the intersect point and the vertices.
  const double d0 ( v0.length2() );
  const double d1 ( v1.length2() );
  const double d2 ( v2.length2() );

  // Return the vertex that is closest to the intersect point.
  if ( d0 < d1 )
  {
    if ( d0 < d2 )
    {
      SharedVertex::ValidRefPtr sv ( triangleSV->sharedVertex0 ( geode, drawable, index ) );
      return VertexPair ( p0, sv.get() );
    }
  }
  else
  {
    if( d1 < d2 )
    {
      SharedVertex::ValidRefPtr sv ( triangleSV->sharedVertex1 ( geode, drawable, index ) );
      return VertexPair ( p1, sv.get() );
    }
  }

  SharedVertex::ValidRefPtr sv ( triangleSV->sharedVertex2 ( geode, drawable, index ) );
  return VertexPair ( p2, sv.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a sphere with given center and radius.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* AddTriangle::_makeSphere ( const osg::Vec3f& center, double radius, const osg::Vec4f &color ) const
{
  // Make the sphere
  osg::ref_ptr<osg::Sphere> sphere ( new osg::Sphere );

  // Set the needed parameters for the sphere
  sphere->setCenter ( osg::Vec3 ( 0.0, 0.0, 0.0 ) );
  sphere->setRadius ( 15 );

  // Make the drawable.
  osg::ref_ptr<osg::ShapeDrawable> shape ( new osg::ShapeDrawable ( sphere.get() ) );

  // Make the geode
  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );

  // Add the sphere to the geode.
  geode->addDrawable ( shape.get() );

  // Set the material.
  OsgTools::MaterialSetter ms;
  ms.diffuse ( color );
  ms ( geode.get() );

  // Make the auto transform.
  osg::ref_ptr< osg::AutoTransform > transform ( new osg::AutoTransform ( ) );
  transform->setPosition ( center );
  transform->setAutoScaleToScreen ( true );

  // Set the normalize state to true, so when the sphere size changes it still looks correct.
  OsgTools::State::StateSet::setNormalize ( transform.get(), true );

  // Add the sphere and return the transform.
  transform->addChild ( geode.get() );
  return transform.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the decorations for this triangle.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *AddTriangle::_makeDecoration ( const osg::Vec3f &v0, const osg::Vec3f &v1, const osg::Vec3f &v2, const osg::Vec3f &n ) const
{
  // Make group and initialize ray.
  osg::ref_ptr < osg::Group > group ( new osg::Group );
  OsgTools::Ray ray;
  ray.thickness ( 3 );

  // For convenience.
  osg::Vec3f a ( v0[0], v0[1], v0[2] );
  osg::Vec3f b ( v1[0], v1[1], v1[2] );
  osg::Vec3f c ( v2[0], v2[1], v2[2] );
  const osg::Vec3f ab ( b - a );
  const osg::Vec3f ac ( c - a );
  const osg::Vec3f bc ( c - b );

  // Offset according to maximum leg length.
  const float maxLeg ( Usul::Math::maximum ( ab.length(), ac.length(), bc.length() ) );
  const float offset ( maxLeg * 0.01f );

  // Offset the vertices.
  a += ( n * offset );
  b += ( n * offset );
  c += ( n * offset );

  // Connect the dots.
  ray.start ( a ); ray.end ( b ); group->addChild ( ray() );
  ray.start ( a ); ray.end ( c ); group->addChild ( ray() );
  ray.start ( b ); ray.end ( c ); group->addChild ( ray() );

  // Return group.
  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean up anything from the scene.
//
///////////////////////////////////////////////////////////////////////////////

void AddTriangle::cleanUp ( Usul::Interfaces::IUnknown* caller )
{
  // Get needed interfaces.
  Usul::Interfaces::IGetDocument::ValidQueryPtr getDoc ( caller );
  Usul::Interfaces::IDocument::ValidQueryPtr document ( getDoc->getDocument() );
  Usul::Interfaces::ISendMessage::ValidQueryPtr send ( document );
  Usul::Interfaces::IGroup::ValidQueryPtr group ( caller );

  group->removeGroup ( Labels::CLOSEST_VERTEX );
  group->removeGroup ( Labels::ADD_TRIANGLE );

  _vertices.clear();

  send->sendMessage ( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );
}
