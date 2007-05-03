
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Callback to draw hidden lines
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_CALLBACKS_HIDDEN_LINES_H__
#define __OSG_TOOLS_CALLBACKS_HIDDEN_LINES_H__

#include "OsgTools/Export.h"

#include "osg/Drawable"
#include "osg/NodeVisitor"
#include "osg/Geode"
#include "osg/Node"
#include "osg/StateSet"
#include "osg/PolygonOffset"
#include "osg/Version"

namespace OsgTools {
namespace Callbacks {

class OSG_TOOLS_EXPORT HiddenLines : public osg::Drawable::DrawCallback
{
public:
  // We may want to have differnt modes for hidden lines.

#if OSG_VERSION_MAJOR >= 1 && OSG_VERSION_MINOR >= 9
  virtual void drawImplementation( osg::RenderInfo& renderInfo, const osg::Drawable* drawable ) const
#else
  virtual void drawImplementation( osg::State& state, const osg::Drawable* drawable ) const
#endif
  {
    // See if the drawable has a state set.
    const osg::StateSet *ss ( drawable->getStateSet() );

    if ( 0x0 == ss )
      ss = _findNodeWithStateSet ( drawable );

    // Draw
#if OSG_VERSION_MAJOR >= 1 && OSG_VERSION_MINOR >= 9
    renderInfo.getState()->apply ( ss );
    drawable->drawImplementation( renderInfo );
#else
    state.apply ( ss );
    drawable->drawImplementation( state );
#endif

    // Make a copy
    osg::ref_ptr < osg::StateSet > hidden ( static_cast < osg::StateSet* > ( ss->clone ( osg::CopyOp::DEEP_COPY_ALL ) ) );

    // Set the polygon-mode.
    osg::ref_ptr<osg::PolygonMode> mode ( new osg::PolygonMode );
    mode->setMode ( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE );
    hidden->setAttributeAndModes ( mode.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );

    // Set the polygon-offset. See osgscribe example.
    osg::ref_ptr<osg::PolygonOffset> offset ( new osg::PolygonOffset );
    offset->setFactor ( -1.0f );
    offset->setUnits  ( -1.0f );
    hidden->setAttributeAndModes ( offset.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );

    // Material same as background and no lighting.
    osg::ref_ptr<osg::Material> material ( new osg::Material );
    osg::Vec4 color ( 0.0, 0.0, 0.0, 1.0 );
    material->setAmbient ( osg::Material::FRONT_AND_BACK, color );
    material->setDiffuse ( osg::Material::FRONT_AND_BACK, color );
    hidden->setAttributeAndModes ( material.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );
    hidden->setMode ( GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );

#if OSG_VERSION_MAJOR >= 1 && OSG_VERSION_MINOR >= 9
    renderInfo.getState()->apply ( hidden.get() );
#else
    state.apply ( hidden.get() );
#endif

    // Draw
#if OSG_VERSION_MAJOR >= 1 && OSG_VERSION_MINOR >= 9
    drawable->drawImplementation( renderInfo );
#else
    drawable->drawImplementation( state );
#endif
  }

private:
  const osg::StateSet * _findNodeWithStateSet ( const osg::Drawable* drawable ) const
  {
    const osg::StateSet *ss ( 0x0 );

    const osg::Node *node ( drawable->getParent ( 0 ) );

    while ( 0x0 != node && 0x0 == ( ss = node->getStateSet() ) )
    {
      node = node->getParent ( 0);
    }

    return ss;
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Visitor to set the callback.
//
///////////////////////////////////////////////////////////////////////////////

class OSG_TOOLS_EXPORT SetHiddenCallback : public osg::NodeVisitor
{
public:
  typedef osg::NodeVisitor BaseClass;

  SetHiddenCallback ( TraversalMode mode = BaseClass::TRAVERSE_ALL_CHILDREN ) : BaseClass( mode )
  { 
    // Always hit every node.
    const unsigned int all ( 0xffffffff );
    this->setTraversalMask    ( all );
    this->setNodeMaskOverride ( all );
  }
  

  virtual void apply( osg::Geode& node )  
  { 
    for ( unsigned int i = 0; i < node.getNumDrawables(); ++i )
    {
      node.getDrawable( i )->setDrawCallback( new HiddenLines );
      
    }
    traverse( node ); 
  }

protected:
  virtual ~SetHiddenCallback () { }

};


///////////////////////////////////////////////////////////////////////////////
//
//  Visitor to unset the callback.
//
///////////////////////////////////////////////////////////////////////////////

class OSG_TOOLS_EXPORT UnSetHiddenCallback : public osg::NodeVisitor
{
public:
  typedef osg::NodeVisitor BaseClass;

  UnSetHiddenCallback ( TraversalMode mode = BaseClass::TRAVERSE_ALL_CHILDREN ) : BaseClass( mode )
  { 
    // Always hit every node.
    const unsigned int all ( 0xffffffff );
    this->setTraversalMask    ( all );
    this->setNodeMaskOverride ( all );
  }

  virtual void apply( osg::Geode& node )  
  {
    for ( unsigned int i = 0; i < node.getNumDrawables(); ++i )
    {
      node.getDrawable( i )->setDrawCallback( 0x0 );
      
    }
    traverse( node ); 
  }

protected:
  virtual ~UnSetHiddenCallback () { }

};


}
}


#endif // __OSG_TOOLS_CALLBACKS_HIDDEN_LINES_H__

