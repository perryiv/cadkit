
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

#include "Usul/Cast/Cast.h"

#include "SAL/Common/Exceptions.h"

#include "PfTools/Group.h"

#include "Performer/pf/pfGroup.h"

#include <limits>

using namespace SAL;
using namespace SAL::Performer;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Group, Group::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Group::Group ( ::pfGroup *g ) : BaseClass ( g ),
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
  _group->addChild ( Node::getNode ( in ) );
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
  if ( ( i >= numKids ) && ( i < ( USUL_UNSAFE_CAST ( unsigned int, std::numeric_limits<int>::max() ) ) ) )
  {
    Usul::Exceptions::Thrower<SAL::Exceptions::IndexOutOfRange>
      ( "Error 1418914482, child index is out of range.",
        "\n\tGiven index:  ", i,
        "\n\tNum children: ", numKids );
  }

  // Get the pfNode.
  typedef USUL_VALID_REF_POINTER ( ::pfNode ) ValidPfNodePtr;
  ValidPfNodePtr perfNode ( _group->getChild ( USUL_UNSAFE_CAST ( int, i ) ) );

  // Try to create the node.
  SAL::Interfaces::INode::QueryPtr node ( Factory::create ( perfNode ) );
  if ( !node.valid() )
  {
    const char *buf = perfNode->getName();
    std::string name ( ( buf ) ? buf : "" );
    Usul::Exceptions::Thrower<SAL::Exceptions::UnknownNodeType>
      ( "Error: 3633596891, failed to determine node type.", 
        "\n\tnode = ", perfNode,
        "\n\tName = ", name,
        "\n\tType = ", perfNode->getClassType()->getName() );
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
  PfTools::Group::removeAllChildren ( _group );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all occurances of the node from the list of children.
//
///////////////////////////////////////////////////////////////////////////////

void Group::removeAllOccurances ( INode *in )
{
  PfTools::Group::removeAllOccurances ( Node::getNode ( in ), _group );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the first occurance of the node from the list of children.
//
///////////////////////////////////////////////////////////////////////////////

void Group::removeFirstOccurance ( INode *in )
{
  _group->removeChild ( Node::getNode ( in ) );
}
