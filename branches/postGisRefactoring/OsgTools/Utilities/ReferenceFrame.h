
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Visitor to set the reference frame of matrix transforms.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_UTILITIES_REFERENCE_FRAME_H__
#define __OSG_TOOLS_UTILITIES_REFERENCE_FRAME_H__


#include "OsgTools/Visitor.h"

#include "osg/Transform"

namespace OsgTools {
namespace Utilities {

  // Useful typedef.
  typedef osg::Transform::ReferenceFrame ReferenceFrame;

  template < class TList >
struct SetReferenceFrame
{
  SetReferenceFrame ( ReferenceFrame from, ReferenceFrame to, TList &list ) :
    _from ( from ),
    _to ( to ),
    _list ( list )
    {
    }

  void operator () ( osg::Transform * node )
  {
    if( node->getReferenceFrame() == _from )
    {
      node->setReferenceFrame( _to );
      _list.push_back( node );
    }
  }

private:
  ReferenceFrame _from;
  ReferenceFrame _to;
  TList &_list;
};

template < class TList >
void setReferenceFrame ( osg::Node * node, ReferenceFrame from, ReferenceFrame to, TList &list )
{
  typedef SetReferenceFrame< TList > Setter;
  Setter setter ( from, to, list );

  // Declare the visitor that uses the functor.
  typedef OsgTools::Visitor<osg::Transform, Setter> Visitor;
  typedef typename Visitor::Ptr VisitorPtr;
  VisitorPtr visitor ( new Visitor ( setter ) );

  node->accept ( *visitor );
}

}
}

#endif // __OSG_TOOLS_UTILITIES_REFERENCE_FRAME_H__

