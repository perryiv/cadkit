
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Test 1.
//
///////////////////////////////////////////////////////////////////////////////

#include "GSG/Core/Transform.h"
#include "GSG/Core/Lod.h"
#include "GSG/Core/Factory.h"
#include "GSG/Core/Shape.h"
#include "GSG/Core/Algorithms.h"
#include "GSG/Core/Renderer.h"
#include "GSG/Core/Update.h"
#include "GSG/Core/FrustumCull.h"
#include "GSG/Core/Sort.h"

#include "GSG/OpenGL/GlDraw.h"

#include "boost/mpl/assert_is_same.hpp"

#include <sstream>
#include <iostream>


using namespace GSG;


struct MyUpdateCallback : public GSG::Visitor::Callback
{
  GSG_DECLARE_CLONE ( MyUpdateCallback );
  GSG_DECLARE_LOCAL_TYPEDEFS ( MyUpdateCallback, GSG::Visitor::Callback );
  virtual void operator () ( Visitor &v, Node &n );
};


void MyUpdateCallback::operator () ( Visitor &v, Node &n )
{
  assert ( dynamic_cast < Update * > ( &v ) );
  std::cout << "Updating node: " << &n 
            << ", name: " << n.first 
            << ", data = " << n.second 
            << std::endl;
}


GSG_IMPLEMENT_CLONE ( MyUpdateCallback );


Group::Ptr buildScene ( Factory *f )
{
  BOOST_MPL_ASSERT_IS_SAME ( std::string, Node::first_type );

  Factory::Ptr factory ( f ? f : new Factory );

  static UnsignedInteger count ( 0 );
  std::ostringstream name;
  name << count++;

  Group::Ptr group ( new Group );
  group->first = "Group ";
  group->first += name.str();

  Color color ( 1.0f, 0.0f, 0.0f, 1.0f );
  Material material;
  material.ambient() = color;
  material.diffuse() = color;

  Shape::Ptr cube ( factory->cube() );
  cube->first = "Shape ";
  cube->first += name.str();
  cube->setAttributes ( new Attributes );
  cube->getAttributes()->getMaterial() = material;

  color.set ( 0.0f, 0.0f, 1.0f, 1.0f );
  material.ambient() = color;
  material.diffuse() = color;

  Shape::Ptr sphere ( factory->sphere() );
  sphere->first = "Shape ";
  sphere->first += name.str();
  sphere->setAttributes ( new Attributes );
  sphere->getAttributes()->getMaterial() = material;

  group->append ( cube );
  group->append ( sphere );

  return group;
}


void test()
{
  // We need a root node.
  Transform::Ptr root ( new Transform );

  // The number of times we loop.
  const unsigned int num ( 100 );
  unsigned int i;

  // Build a scene and add it to the root "num" times. Pass a null factory.
  for ( i = 0; i < num; ++i )
    root->append ( buildScene ( 0x0 ) );

  // Make a factory and set its pools.
  Factory::Ptr factory ( new Factory );
  factory->setVertexPool ( new VertexPool );
  factory->setNormalPool ( new NormalPool );
  factory->setColorPool ( new ColorPool );

  // Build the scene again, this time passing the factory.
  for ( i = 0; i < num; ++i )
    root->append ( buildScene ( factory ) );

  // Look for the node.
  typedef SelectFirst < Node > Select;
  typedef std::equal_to < Select::Type > Comparison;
  typedef Compare < Node, Select, Comparison > FindNode;
  Nodes::iterator a = GSG::recursive_find_first ( root->begin(), root->end(), FindNode ( "Shape 4" ) );
  Nodes::iterator b = GSG::recursive_find_first ( root->begin(), root->end(), FindNode ( "Shape 5" ) );
  Nodes::iterator c = GSG::recursive_find_first ( root->begin(), root->end(), FindNode ( "Group 6" ) );

  // Make sure we found a node.
  assert ( a->valid() );
  assert ( b->valid() );
  assert ( c->valid() );

  // Print the names.
  std::cout << "Found node: " << Select () ( *(*a) ) << std::endl;
  std::cout << "Found node: " << Select () ( *(*b) ) << std::endl;
  std::cout << "Found node: " << Select () ( *(*c) ) << std::endl;

  // Arbitrary viewport.
  Viewport vp ( 0, 0, 100, 100 );

  // Make a renderer.
  RenderBin::Ptr bin ( new RenderBin );
  Renderer::Ptr renderer ( new Renderer );
  Update::Ptr update ( new Update );
  renderer->append ( update );
  renderer->append ( new FrustumCull ( bin ) );
  renderer->append ( new Sort ( bin ) );
  renderer->append ( new GlDraw ( bin, vp, 0x0 ) );

  // Set our post-update callback.
  MyUpdateCallback::Ptr cb ( new MyUpdateCallback );
  update->postVisitCallback ( cb );

  // Have renderer traverse the scene.
  renderer->traverse ( *root );
}


int main ( int argc, char **argv )
{
  try
  {
    test();
  }
  
  catch ( ... )
  {
    std::cout << "Unknown exception caught." << std::endl;
  }

  return 0;
}
