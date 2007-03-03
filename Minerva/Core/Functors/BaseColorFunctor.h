
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __BASE_COLOR_FUNCTOR_H__
#define __BASE_COLOR_FUNCTOR_H__

#include "Minerva/Core/Export.h"

#include "osg/Vec4"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include "boost/serialization/is_abstract.hpp"

namespace OsgTools { namespace Legend { class Icon; } }

namespace Minerva {
namespace Core {
  namespace Functors {

struct MINERVA_EXPORT BaseColorFunctor : public Usul::Base::Referenced
{
  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( BaseColorFunctor );

  virtual osg::Vec4 operator() ( double value ) const = 0;

  virtual OsgTools::Legend::Icon * icon () = 0;
};

  }
}
}

BOOST_IS_ABSTRACT(Minerva::Core::Functors::BaseColorFunctor)

#endif
