
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __GRADIENT_COLOR_FUNCTOR_H__
#define __GRADIENT_COLOR_FUNCTOR_H__

#include "Minerva/Core/Export.h"

#include "BaseColorFunctor.h"

#include "Serialize/XML/Macros.h"

namespace Minerva {
namespace Core {
namespace Functors {

struct MINERVA_EXPORT GradientColorFunctor : public Minerva::Core::Functors::BaseColorFunctor
{
  /// Typedefs.
  typedef Minerva::Core::Functors::BaseColorFunctor BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( GradientColorFunctor );
  
  /// Constructor.
  GradientColorFunctor();
  virtual ~GradientColorFunctor();

  virtual BaseColorFunctor*    clone() const;

  /// Get/Set the min color.
  void               minColor( const osg::Vec4& );
  const osg::Vec4&   minColor () const;

  /// Get/Set the max color.
  void               maxColor( const osg::Vec4& );
  const osg::Vec4&   maxColor () const;

  /// Get/Set min value
  void               minValue ( double minValue );
  double             minValue () const;

  /// Get/Set max value
  void               maxValue ( double maxValue );
  double             maxValue () const;

  /// Get the color.
  virtual osg::Vec4  operator() ( double value ) const;

  virtual OsgTools::Legend::Icon * icon ( Usul::Interfaces::IUnknown *caller = 0x0 );

private:

  osg::Vec4 _minColor;
  osg::Vec4 _maxColor;
  double _minValue;
  double _maxValue;

  SERIALIZE_XML_DEFINE_MEMBERS ( GradientColorFunctor );
};


  }
}
}


#endif
