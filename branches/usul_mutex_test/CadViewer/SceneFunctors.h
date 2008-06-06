
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functors that operate on osg::Object objects.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_SCENE_FUNCTORS_H_
#define _CV_SCENE_FUNCTORS_H_

#include "CadViewer/Interfaces/ISelection.h"
#include "CadViewer/Interfaces/IVisibility.h"
#include "CadViewer/Interfaces/IMaterialStack.h"

#include "osg/Group"
#include "osg/ref_ptr"

#include "Usul/Pointers/Pointers.h"

#include <list>


namespace CV {
namespace OSG {
namespace Functors {


///////////////////////////////////////////////////////////////////////////////
//
//  Functor class for appending the node to the list.
//
///////////////////////////////////////////////////////////////////////////////

template < class NodeType > struct Append
{
  typedef USUL_VALID_REF_POINTER(NodeType) NodePtr;
  typedef std::list<NodePtr> NodeList;
  typedef typename NodeList::const_iterator const_iterator;
  typedef typename NodeList::iterator iterator;
  typedef typename NodeList::size_type size_type;

  Append() : _list(){}
  Append ( const Append &a ) : _list ( a._list ){}

  void operator () ( osg::Object *object )
  {
    if ( object )
    {
      NodeType *node = dynamic_cast < NodeType * > ( object );
      if ( node )
      {
        _list.push_back ( node );
      }
    }
  }

  const_iterator  begin() const { return _list.begin(); }
  iterator        begin()       { return _list.begin(); }

  const_iterator  end()   const { return _list.end(); }
  iterator        end()         { return _list.end(); }

  size_type       size()  const { return _list.size(); }

private:

  NodeList _list;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Functor class for selecting.
//
///////////////////////////////////////////////////////////////////////////////

struct Select
{
  Select ( CV::Interfaces::ISelection *s ) : _s ( s ){}
  Select ( const Select &s ) : _s ( s._s ){}
  void operator () ( osg::Object *object )
  {
    if ( object )
      _s->select ( object );
  }
private:
  CV::Interfaces::ISelection::ValidRefPtr _s;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Functor class for unselecting.
//
///////////////////////////////////////////////////////////////////////////////

struct Unselect
{
  Unselect ( CV::Interfaces::ISelection *s ) : _s ( s ){}
  Unselect ( const Unselect &s ) : _s ( s._s ){}
  void operator () ( osg::Object *object )
  {
    if ( object )
      _s->unselect ( object );
  }
private:
  CV::Interfaces::ISelection::ValidRefPtr _s;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Functor class for hiding.
//
///////////////////////////////////////////////////////////////////////////////

struct Hide
{
  Hide ( CV::Interfaces::IVisibility *v ) : _v ( v ){}
  Hide ( const Hide &v ) : _v ( v._v ){}
  void operator () ( osg::Node *node )
  {
    if ( node )
      _v->hide ( node );
  }
private:
  CV::Interfaces::IVisibility::ValidRefPtr _v;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Functor class for showing (making visible).
//
///////////////////////////////////////////////////////////////////////////////

struct Show
{
  Show ( CV::Interfaces::IVisibility *v ) : _v ( v ){}
  Show ( const Show &v ) : _v ( v._v ){}
  void operator () ( osg::Node *node )
  {
    if ( node )
      _v->show ( node );
  }
private:
  CV::Interfaces::IVisibility::ValidRefPtr _v;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Functor to pop the material.
//
///////////////////////////////////////////////////////////////////////////////

struct PopMaterial
{
  PopMaterial ( CV::Interfaces::IMaterialStack *s ) : _s ( s ){}
  PopMaterial ( const PopMaterial &s ) : _s ( s._s ){}
  void operator () ( osg::Node *node )
  {
    if ( node )
      _s->popMaterial ( node );
  }
private:
  CV::Interfaces::IMaterialStack::ValidRefPtr _s;
};


///////////////////////////////////////////////////////////////////////////////
//
//  For getting the number of children.
//
///////////////////////////////////////////////////////////////////////////////

struct NumChildren
{
  NumChildren(){}
  unsigned int operator () ( const osg::Object *object ) const
  {
    const osg::Group *group = dynamic_cast < const osg::Group * > ( object );
    return ( group ) ? group->getNumChildren() : 0;
  }
};


}; // namespace Functors
}; // namespace OSG
}; // namespace CV


#endif // _CV_SCENE_FUNCTORS_H_
