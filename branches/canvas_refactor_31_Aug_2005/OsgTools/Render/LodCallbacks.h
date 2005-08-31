
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Callback classes and functions for LODs.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OPEN_SCENE_GRAPH_TOOLS_LOD_CALLBACKS_H_
#define _OPEN_SCENE_GRAPH_TOOLS_LOD_CALLBACKS_H_

#include "OsgTools/Visitor.h"

#include "osg/LOD"


namespace OsgTools {
namespace Render {


///////////////////////////////////////////////////////////////////////////////
//
//  Functor to set the callback.
//
///////////////////////////////////////////////////////////////////////////////

template < class CB, class LodList > struct SetLodCullCallback
{
  SetLodCullCallback ( CB *cb, LodList &ll ) : _cb ( cb ), _ll ( ll ){}
  template < class NT > void operator() ( NT *node )
  {
    if ( node )
    {
      // Set the callback.
      node->setCullCallback ( _cb.get() );

      // If the list is invalid then append this node.
      if ( false == _ll.first )
        _ll.second.push_back ( node );
    }
  }
protected:
  osg::ref_ptr<CB> _cb;
  LodList &_ll;
};


///////////////////////////////////////////////////////////////////////////////
//
//  The low-lod cull-callback.
//
///////////////////////////////////////////////////////////////////////////////

struct LowLodCallback : public osg::NodeCallback
{
  typedef osg::ref_ptr<LowLodCallback> Ptr;

  LowLodCallback(){}
  virtual ~LowLodCallback(){}
  virtual void operator() ( osg::Node *node, osg::NodeVisitor *visitor )
  {
    if ( node && visitor )
    {
      // See if it is a LOD with children.
      osg::LOD *lod ( dynamic_cast<osg::LOD *> ( node ) );
      if ( lod && lod->getNumChildren() )
      {
        // Choose the last child.
        osg::Node *child = lod->getChild ( lod->getNumChildren() - 1 );
        if ( child )
        {
          // By sending the cull-visitor to this child we select it to draw.
          child->accept ( *visitor );
        }
      }
    }
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  The high-lod cull-callback.
//
///////////////////////////////////////////////////////////////////////////////

struct HighLodCallback : public osg::NodeCallback
{
  typedef osg::ref_ptr<HighLodCallback> Ptr;

  HighLodCallback(){}
  virtual ~HighLodCallback(){}
  virtual void operator() ( osg::Node *node, osg::NodeVisitor *visitor )
  {
    if ( node && visitor )
    {
      // See if it is a LOD with children.
      osg::LOD *lod ( dynamic_cast<osg::LOD *> ( node ) );
      if ( lod && lod->getNumChildren() )
      {
        // Choose the first child.
        osg::Node *child = lod->getChild ( 0 );
        if ( child )
        {
          // By sending the cull-visitor to this child we select it to draw.
          child->accept ( *visitor );
        }
      }
    }
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Set the callback.
//
///////////////////////////////////////////////////////////////////////////////

template < class CB, class LodList > void setLodCullCallback ( CB *cb, osg::Node *node, LodList &ll )
{
  // If there is a node...
  if ( node )
  {
    // If the list of lods is valid...
    if ( true == ll.first )
    {
      // Loop through the list.
      typedef typename LodList::second_type Lods;
      typedef typename Lods::iterator LodsItr;

      Lods &lods = ll.second;
      for ( LodsItr i = lods.begin(); i != lods.end(); ++i )
        (*i)->setCullCallback ( cb );
    }

    // Otherwise...
    else
    {
      // Declare the functor to set the callback.
      typedef SetLodCullCallback<CB,LodList> Setter;
      Setter setter ( cb, ll );

      // Declare the visitor that uses the functor.
      typedef OsgTools::Visitor<osg::LOD,Setter> Visitor;
      typedef typename Visitor::Ptr VisitorPtr;
      VisitorPtr visitor ( new Visitor ( setter ) );

      // Set all callbacks for all the lods.
      node->accept ( *visitor );

      // The list of lods is now valid.
      ll.first = true;
    }
  }
}


} // namespace Render
} // namespace OsgTools


#endif // _OPEN_SCENE_GRAPH_TOOLS_LOD_CALLBACKS_H_
