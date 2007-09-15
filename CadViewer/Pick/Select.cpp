
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for selecting the intersected portion of the scene.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "Select.h"

#include "CadViewer/ErrorChecker.h"
#include "CadViewer/Constants.h"
#include "CadViewer/ScenePredicates.h"

#include "CadViewer/Interfaces/ISelection.h"
#include "CadViewer/Interfaces/IMaterialStack.h"

#include "Usul/Print/Node.h"
#include "Usul/Predicates/IsOfType.h"
#include "Usul/Math/Vector4.h"

#include "osg/Material"
#include "osg/Geode"
#include "osg/LOD"
#include "osg/MatrixTransform"

#include "boost/mpl/assert.hpp"
#include "boost/type_traits/is_same.hpp"

#include <algorithm>

using namespace CV;
using namespace CV::Pick;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Select::Select ( 
  Unknown *unknown, 
  Direction *dir,
  const std::string& name ) : 
  BaseClass ( unknown, dir, name ),
  _node ( 0x0 ),
  _material ( 0x0 )
{
  // You assume this when you use the algorithms.
  BOOST_MPL_ASSERT (( boost::is_same < osg::Node *, osg::NodePath::value_type > ));
  BOOST_MPL_ASSERT (( boost::is_same < osg::Node *, BaseClass::Path::value_type > ));
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Select::Select ( const Select &s ) : 
  BaseClass ( s ),
  _node ( s._node ),
  _material ( s._material )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Select::~Select()
{
#if 0
  // Pop the material if there is a selected node.
  this->_popMaterial ( _node.get() );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unselect the current node, if there is one.
//
///////////////////////////////////////////////////////////////////////////////

void Select::unselect()
{
  // Undecorate.
  this->_popMaterial ( _node.get() );

  // Unselect.
  this->_unselect ( _node.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Select the intersected portion of the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Select::select()
{
  typedef BaseClass::Path Path;
  typedef Path::reverse_iterator RevItr;
  typedef Usul::Predicates::IsOfType<osg::MatrixTransform> IsMatrixTransform;

#if 1
  // If we have a selected node then reset its material.
  this->_popMaterial ( _node.get() );
#endif

  // Now reset the current node to null.
  _node = 0x0;

  // Intersect the navigation-scene. We have to use this node because it has 
  // the transformations in it.
  this->_intersect ( this->_navigationScene() );

  // See if we hit anything.
  Path &path = this->_path();
  if ( path.empty() )
    return;

  // If the model-scene is not in the path then we hit the grid or nothing.
  if ( path.end() == std::find ( path.begin(), path.end(), this->_modelsScene() ) )
    return;

  // Get the "lowest" node that is a MatrixTransform (the part).
  RevItr i = std::find_if ( path.rbegin(), path.rend(), IsMatrixTransform() );
  _node = ( path.rend() == i ) ? _node.get() : *i;

  // Find the model-scene's root in the path.
  RevItr n = std::find ( i, path.rend(), this->_modelsScene() );
  if ( path.rend() != n )
    ++n;

  // Get the selection interface.
  CV::Interfaces::ISelection::ValidQueryPtr iSelection ( this->caller () );

  // Find the first unselected parent with more than one child, 
  // or null if we are already at the top.
  // TODO: add the check for one child.
  CV::OSG::Predicates::IsSelected isSelected ( iSelection );
  i = std::find_if ( i, n, std::not1 ( isSelected ) );
  _node = ( n == i ) ? 0x0 : *i;

  // Select the node.
  this->_select ( _node.get() );

  // Decorate.
  this->_pushMaterial ( _node.get() );

#if 1
  Usul::Print::OSG::node ( "   Selected node: ", _node.get(), std::cout );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Change the material of the intersected portion of the scene. It will 
//  always be a leaf node. We grab the MatrixTransform that is above it 
//  (in order to highlight the entire "part").
//
///////////////////////////////////////////////////////////////////////////////

void Select::operator()()
{
  typedef BaseClass::Path Path;
  typedef Path::reverse_iterator RevItr;
  typedef Usul::Predicates::IsOfType<osg::MatrixTransform> IsMatrixTransform;

  // If we have a selected node then reset its material.
  this->_popMaterial ( _node.get() );

  // Now reset the current node to null.
  _node = 0x0;

  // Draw the ray.
  this->_updateRayScene();

  // Intersect the scene.
  this->_intersect ( this->_navigationScene() );

  // See if we hit anything.
  Path &path = this->_path();
  if ( path.empty() )
    return;

  // If the model-scene is not in the path then we hit the grid or nothing.
  if ( path.end() == std::find ( path.begin(), path.end(), this->_modelsScene() ) )
    return;

  // Get the "lowest" node that is a MatrixTransform (the part).
  RevItr i = std::find_if ( path.rbegin(), path.rend(), IsMatrixTransform() );
  _node = ( path.rend() == i ) ? _node.get() : *i;

#if 0
  Print::OSG::node ( "Intersected node: ", _node.get(), std::cout );
#endif

  // Push the material. This will ignore a null node.
  this->_pushMaterial ( _node.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Select the node.
//
///////////////////////////////////////////////////////////////////////////////

void Select::_select ( osg::Node *node )
{
  if ( node )
  {
    USUL_ASSERT ( node->getName() != std::string ( "_gridBranch" ) );
    CV::Interfaces::ISelection::ValidQueryPtr iSelection ( this->caller () );
    iSelection->select ( node );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unselect the node.
//
///////////////////////////////////////////////////////////////////////////////

void Select::_unselect ( osg::Node *node )
{
  if ( node )
  {
    CV::Interfaces::ISelection::ValidQueryPtr iSelection ( this->caller () );
    iSelection->unselect ( node );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Push a material of the given node.
//
///////////////////////////////////////////////////////////////////////////////

void Select::_pushMaterial ( osg::Node* node )
{
  if ( node )
  {
    osg::ref_ptr<osg::Material> mat ( this->_cloneMaterial() );
    this->_pushMaterial ( mat.get(), _node.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Push the material of the given node.
//
///////////////////////////////////////////////////////////////////////////////

void Select::_pushMaterial ( const osg::Material* mat, osg::Node* node )
{
  ErrorChecker ( 1070922363, 0x0 != mat );
  ErrorChecker ( 1070922364, 0x0 != node );

  CV::Interfaces::IMaterialStack::ValidQueryPtr materialStack ( this->caller () );
  materialStack->pushMaterial ( mat, node );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pop the material of the given node.
//
///////////////////////////////////////////////////////////////////////////////

void Select::_popMaterial ( osg::Node* node )
{
  if ( node )
  {
    CV::Interfaces::IMaterialStack::ValidQueryPtr materialStack ( this->caller () );
    materialStack->popMaterial ( node );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone the material.
//
///////////////////////////////////////////////////////////////////////////////

Select::Material *Select::_cloneMaterial() const
{
  if ( !_material.valid() )
    return new Material;

  osg::Referenced *copy = _material->clone ( osg::CopyOp::DEEP_COPY_ALL );
  MaterialPtr mat ( dynamic_cast < Material * > ( copy ) );
  ErrorChecker ( 4195367204u, mat.valid() );
  return mat.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the material.
//
///////////////////////////////////////////////////////////////////////////////

const Select::Material *Select::material() const
{
  return _material.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the material.
//
///////////////////////////////////////////////////////////////////////////////

void Select::material ( Select::Material *m )
{
  _material = m;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset any states.
//
///////////////////////////////////////////////////////////////////////////////

void Select::reset()
{
  BaseClass::reset();
  _node = 0x0;
}
