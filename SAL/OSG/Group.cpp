
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Group class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "Group.h"
#include "Factory.h"

#include "SAL/Common/Exceptions.h"

#include "OsgTools/Group.h"

#include "osg/Group"

using namespace SAL;
using namespace SAL::OSG;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Group, Group::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Group::Group ( osg::Group *g ) : BaseClass ( g ),
  _group ( g )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Group::~Group()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Group::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case SAL::Interfaces::IGroup::IID:
    return static_cast<SAL::Interfaces::IGroup*>(this);
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the node to list of children.
//
///////////////////////////////////////////////////////////////////////////////

void Group::addChild ( INode *in )
{
  // Get the node.
  SAL::Interfaces::IOSG::ValidQueryPtr iosg ( in );
  Node::ValidOsgRefPtr rp ( iosg->osgReferenced() );
  Node::ValidOsgNodePtr node ( dynamic_cast<osg::Node *> ( rp.get() ) );

  // Add the node to the group.
  _group->addChild ( node.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the i'th child.
//
///////////////////////////////////////////////////////////////////////////////

SAL::Interfaces::INode *Group::childNode ( unsigned long i )
{
  // Get the number of children.
  const unsigned long numKids ( this->numChildren() );

  // Make sure the index is in range.
  if ( i >= numKids )
  {
    Usul::Exceptions::Thrower<SAL::Exceptions::IndexOutOfRange>
      ( "Error 1469903385, child index is out of range.",
        "\n\tGiven index:  ", i,
        "\n\tNum children: ", numKids );
  }

  // Get the osg-node.
  typedef USUL_VALID_REF_POINTER ( osg::Node ) ValidOsgNodePtr;
  ValidOsgNodePtr osgNode ( _group->getChild ( i ) );

  // Try to create the node.
  SAL::Interfaces::INode::QueryPtr node ( Factory::create ( osgNode ) );
  if ( !node.valid() )
  {
    Usul::Exceptions::Thrower<SAL::Exceptions::UnknownNodeType>
      ( "Error: 1989210475, failed to determine node type.", 
        "\n\tosg::Node = ", osgNode,
        "\n\tName      = ", osgNode->getName() ,
        "\n\tType      = ", osgNode->className() );
  }

  // Return the node.
  return node.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the i'th child.
//
///////////////////////////////////////////////////////////////////////////////

const SAL::Interfaces::INode *Group::childNode ( unsigned long i ) const
{
  // Getting a child does not alter the scene, so this should be ok.
  Group *me = const_cast < Group * > ( this );
  return me->childNode ( i );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of children.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long Group::numChildren() const
{
  return _group->getNumChildren();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all children.
//
///////////////////////////////////////////////////////////////////////////////

void Group::removeAllChildren()
{
  OsgTools::Group::removeAllChildren ( _group );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all occurances of the node from the list of children.
//
///////////////////////////////////////////////////////////////////////////////

void Group::removeAllOccurances ( INode *in )
{
  // Get the node.
  SAL::Interfaces::IOSG::ValidQueryPtr iosg ( in );
  Node::ValidOsgRefPtr rp ( iosg->osgReferenced() );
  Node::ValidOsgNodePtr node ( dynamic_cast<osg::Node *> ( rp.get() ) );

  // Remove occurances.
  OsgTools::Group::removeAllOccurances ( node.get(), _group );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the first occurance of the node from the list of children.
//
///////////////////////////////////////////////////////////////////////////////

void Group::removeFirstOccurance ( INode *in )
{
  // Get the node.
  SAL::Interfaces::IOSG::ValidQueryPtr iosg ( in );
  Node::ValidOsgRefPtr rp ( iosg->osgReferenced() );
  Node::ValidOsgNodePtr node ( dynamic_cast<osg::Node *> ( rp.get() ) );

  // Remove the first occurance.
  _group->removeChild ( node.get() );
}
