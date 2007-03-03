
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SINGLE_COLOR_FUNCTOR_H__
#define __SINGLE_COLOR_FUNCTOR_H__

#include "Minerva/Core/Export.h"

#include "BaseColorFunctor.h"

#include "OsgTools/Utilities/Vec4Serialize.h"

#include "boost/serialization/nvp.hpp"
namespace boost { namespace serialization { class access; } }

namespace Minerva {
namespace Core {
  namespace Functors {

struct MINERVA_EXPORT SingleColorFunctor : public Minerva::Core::Functors::BaseColorFunctor
{
  /// Typedefs.
  typedef Minerva::Core::Functors::BaseColorFunctor BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( SingleColorFunctor );
  
  /// Constructor.
  SingleColorFunctor();
  virtual ~SingleColorFunctor();

  /// Get/Set the color.
  void               color( const osg::Vec4& );
  const osg::Vec4&   color () const;

  /// Get the color.
  virtual osg::Vec4  operator() ( double value ) const;

  virtual OsgTools::Legend::Icon * icon ();

private:
  friend class boost::serialization::access;
  template < class Archive > void serialize( Archive &ar, const unsigned int version );

  osg::Vec4 _color;
};


template < class Archive >
void SingleColorFunctor::serialize( Archive &ar, const unsigned int version )
{
  boost::serialization::void_cast_register<SingleColorFunctor, BaseClass>( this, this );

  ar & boost::serialization::make_nvp( "Color", _color );
}

  }
}
}


#endif
