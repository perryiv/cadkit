
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

#include "OsgTools/Utilities/Vec4Serialize.h"

#include "BaseColorFunctor.h"

//#include "boost/serialization/nvp.hpp"
//#include "boost/serialization/export.hpp"
namespace boost { namespace serialization { class access; } }

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

  virtual OsgTools::Legend::Icon * icon ();

private:
  friend class boost::serialization::access;
  template < class Archive > void serialize( Archive &ar, const unsigned int version );

  osg::Vec4 _minColor;
  osg::Vec4 _maxColor;
  double _minValue;
  double _maxValue;
};

template < class Archive >
void GradientColorFunctor::serialize( Archive &ar, const unsigned int version )
{
  boost::serialization::void_cast_register<GradientColorFunctor, BaseClass>( this, this );

  ar & boost::serialization::make_nvp( "MinColor", _minColor );
  ar & boost::serialization::make_nvp( "MaxColor", _maxColor );
  ar & boost::serialization::make_nvp( "MinValue", _minValue );
  ar & boost::serialization::make_nvp( "MaxValue", _maxValue );
}

  }
}
}


#endif
