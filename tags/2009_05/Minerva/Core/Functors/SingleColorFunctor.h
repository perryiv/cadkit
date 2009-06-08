
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
#include "Minerva/Core/Functors/BaseColorFunctor.h"

#include "Serialize/XML/Macros.h"

#include "osg/Vec4d"

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

  virtual BaseColorFunctor* clone() const;

  /// Get/Set the color.
  void                color( const osg::Vec4d& );
  const osg::Vec4d&   color () const;

  /// Get the color.
  virtual osg::Vec4  operator() ( double value ) const;

  virtual OsgTools::Widgets::Item * icon ( Usul::Interfaces::IUnknown *caller = 0x0 );

private:

  osg::Vec4d _color;

  SERIALIZE_XML_DEFINE_MEMBERS ( SingleColorFunctor );
};

  }
}
}


#endif
