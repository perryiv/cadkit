
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_SORT_BACK_TO_FRONT_CALLBACK_H__
#define __OSG_TOOLS_SORT_BACK_TO_FRONT_CALLBACK_H__

#include "OsgTools/Export.h"

#include "osg/NodeCallback"
#include "osg/NodeVisitor"
#include "osg/Geode"

namespace OsgTools {


///////////////////////////////////////////////////////////////////////////////
//
//  Callback to sort back to front.
//
///////////////////////////////////////////////////////////////////////////////

class OSG_TOOLS_EXPORT SortBackToFrontCallback : public osg::NodeCallback
{
public:
  typedef osg::NodeCallback BaseClass;

  SortBackToFrontCallback();
  virtual ~SortBackToFrontCallback();

  virtual void operator() ( osg::Node* node, osg::NodeVisitor* nv );
    
};


///////////////////////////////////////////////////////////////////////////////
//
//  Visitor to set the callback.
//
///////////////////////////////////////////////////////////////////////////////

class OSG_TOOLS_EXPORT SetSortToFrontCallback : public osg::NodeVisitor
{
public:
  typedef osg::NodeVisitor BaseClass;

  SetSortToFrontCallback ( TraversalMode mode = BaseClass::TRAVERSE_ALL_CHILDREN ) : BaseClass( mode ), _cb ( new SortBackToFrontCallback ) 
  { 
    // Always hit every node.
    const unsigned int all ( 0xffffffff );
    this->setTraversalMask    ( all );
    this->setNodeMaskOverride ( all );
  }
  

  virtual void apply( osg::Node& node)  
  { 
    node.setCullCallback( _cb.get() );  
    traverse( node ); 
  }

private:
  virtual ~SetSortToFrontCallback () { }

  osg::ref_ptr< SortBackToFrontCallback > _cb;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Visitor to unset the callback.
//
///////////////////////////////////////////////////////////////////////////////

class OSG_TOOLS_EXPORT UnSetSortToFrontCallback : public osg::NodeVisitor
{
public:
  typedef osg::NodeVisitor BaseClass;

  UnSetSortToFrontCallback ( TraversalMode mode = BaseClass::TRAVERSE_ALL_CHILDREN ) : BaseClass( mode )
  { 
    // Always hit every node.
    const unsigned int all ( 0xffffffff );
    this->setTraversalMask    ( all );
    this->setNodeMaskOverride ( all );
  }

  virtual void apply( osg::Node& node)  { node.setCullCallback( 0x0 );  traverse( node ); }

private:
  virtual ~UnSetSortToFrontCallback () { }

};

}


#endif // __OSG_TOOLS_SORT_BACK_TO_FRONT_CALLBACK_H__

