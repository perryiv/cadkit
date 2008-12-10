
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_LEGEND_SOLID_ICON_H__
#define __OSG_TOOLS_LEGEND_SOLID_ICON_H__

#include "OsgTools/Export.h"
#include "OsgTools/Widgets/Item.h"
#include "OsgTools/Widgets/LineFunctor.h"
#include "OsgTools/Widgets/QuadFunctor.h"
#include "OsgTools/Widgets/SphereFunctor.h"
#include "OsgTools/ColorSetter.h"
#include "OsgTools/MaterialSetter.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include "osg/Geode"

namespace osg { class Node; class Geometry; }

namespace OsgTools {
namespace Widgets {

template < class ColorPolicy, class GeometryFunctor >
class SolidIcon : public OsgTools::Widgets::Item
{
public:
  typedef OsgTools::Widgets::Item BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( SolidIcon );

  SolidIcon() :
    _policy( ),
    _functor( )
    {
    }

  /// Get/Set the geometry functor.
  void                    geometryFunctor( const GeometryFunctor& gf ) { _functor = gf; }
  GeometryFunctor&        geometryFunctor() { return _functor; }
  const GeometryFunctor&  geometryFunctor() const { return _functor; }

  /// Get/Set the color policy.
  void                    colorPolicy ( const ColorPolicy& policy ) { _policy = policy; }
  ColorPolicy&            colorPolicy()       { return _policy; }
  const ColorPolicy&      colorPolicy() const { return _policy; }

  virtual osg::Node*      buildScene ( unsigned int depth = 0 );

protected:
  virtual ~SolidIcon() { }

private:
  ColorPolicy _policy;
  GeometryFunctor _functor;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

template < class ColorPolicy, class GeometryFunctor >
osg::Node* SolidIcon < ColorPolicy, GeometryFunctor >::buildScene ( unsigned int depth )
{
  const SizeType width  ( this->size()[0] );
  const SizeType height ( this->size()[1] );
  
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  geode->setName( "SolidIcon" );
  osg::ref_ptr < osg::Geometry > geometry ( this->geometryFunctor() ( width, height ) );

  geode->addDrawable( geometry.get() );

  // Set the correct render order.
  Item::_setState ( geode->getOrCreateStateSet(), depth );

  _policy ( geometry.get() );

  return geode.release();
}


/// Useful typedefs.
typedef SolidIcon< OsgTools::ColorSetter, LineFunctor >       SolidColorLineIcon;
typedef SolidIcon< OsgTools::ColorSetter, QuadFunctor >       SolidColorQuadIcon; 
typedef SolidIcon< OsgTools::MaterialSetter, QuadFunctor >    SolidMaterialQuadIcon; 
typedef SolidIcon< OsgTools::MaterialSetter, SphereFunctor >  SolidMaterialSphereIcon; 

}
}


#endif //__OSG_TOOLS_LEGEND_SOLID_ICON_H__
