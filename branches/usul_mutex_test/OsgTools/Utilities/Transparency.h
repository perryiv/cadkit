
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Visitor to set the transparency of materials in the scene..
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_UTILITIES_TRANSPARENCY_H__
#define __OSG_TOOLS_UTILITIES_TRANSPARENCY_H__

#include "osg/NodeVisitor"
#include "osg/Material"
#include "osg/Node"
#include "osg/BlendFunc"
#include "osg/Depth"

namespace OsgTools {
namespace Utilities {

class TransparencyVisitor : public osg::NodeVisitor
{
public:
  typedef osg::NodeVisitor BaseClass;

  TransparencyVisitor( float alpha ) : BaseClass ( TRAVERSE_ALL_CHILDREN ), _alpha ( alpha )
  {
  }

  virtual void apply( osg::Node& node )
  {
    osg::ref_ptr < osg::StateSet > ss ( node.getOrCreateStateSet() );
    this->_setStateSet ( ss.get() );

    this->traverse(node);
  }

  virtual void apply( osg::Geode& node )
  {
    osg::ref_ptr < osg::StateSet > ss ( node.getOrCreateStateSet() );
    this->_setStateSet ( ss.get() );

    for( unsigned int i = 0; i < node.getNumDrawables(); ++i )
    {
      osg::ref_ptr < osg::StateSet > ss ( node.getDrawable(i)->getOrCreateStateSet() );
      this->_setStateSet ( ss.get() );
    }
  }

private:
  void _setStateSet ( osg::StateSet *ss )
  {
    osg::ref_ptr< osg::Material > mat ( reinterpret_cast < osg::Material* > ( ss->getAttribute( osg::StateAttribute::MATERIAL ) ) );
    
    if( mat.valid() )
    {
      mat->setTransparency( osg::Material::FRONT_AND_BACK, 1.0f - _alpha );

      if( 1.0f == _alpha )
      {
        ss->setMode ( GL_BLEND,      osg::StateAttribute::OFF );
        ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::ON  );
        ss->setRenderingHint( osg::StateSet::DEFAULT_BIN );

        osg::ref_ptr < osg::Depth > depth ( new osg::Depth );
        depth->setWriteMask ( true );
        //ss->setAttribute ( depth.get(), osg::StateAttribute::ON );
      }
      else
      {
        ss->setMode ( GL_BLEND,      osg::StateAttribute::ON  );
        ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::OFF );
        ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
        osg::ref_ptr < osg::BlendFunc > func ( new osg::BlendFunc );
        func->setSource ( GL_SRC_COLOR );
        func->setDestination ( GL_ONE_MINUS_CONSTANT_ALPHA );
        //ss->setAttribute ( func.get(), osg::StateAttribute::ON );

        osg::ref_ptr < osg::Depth > depth ( new osg::Depth );
        depth->setWriteMask ( false );
        //ss->setAttribute ( depth.get(), osg::StateAttribute::ON );
      }
    }
  }

  TransparencyVisitor(); // Do not use.

  float _alpha;
};


void setTransparency ( osg::Node * node, float alpha )
{
  osg::ref_ptr< TransparencyVisitor > visitor ( new TransparencyVisitor ( alpha ) );

  node->accept ( *visitor );
}

}
}

#endif // __OSG_TOOLS_UTILITIES_TRANSPARENCY_H__

