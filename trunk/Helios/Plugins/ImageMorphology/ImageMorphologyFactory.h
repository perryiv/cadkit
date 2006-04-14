
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Factory class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMAGE_MORPHOLOGY_FACTORY_CLASS_H_
#define _IMAGE_MORPHOLOGY_FACTORY_CLASS_H_

#include "CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IClassFactory.h"
#include "Usul/Interfaces/IClassesFactory.h"


class ImageMorphologyFactory : public Usul::Base::Referenced,
                               public Usul::Interfaces::IClassFactory,
                               public Usul::Interfaces::IClassesFactory
{
public:

  /// Typedefs.
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ImageMorphologyFactory );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  ImageMorphologyFactory();

protected:

  /// Use reference counting.
  virtual ~ImageMorphologyFactory();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IClassFactory
  //
  /////////////////////////////////////////////////////////////////////////////

  /// Create a component of the given interface type (defined by iid).
  /// Return an unknown pointer to this new component.
  virtual Unknown *       createInstance ( unsigned long iid );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IClassesFactory
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual Unknowns       createInstances ( unsigned long iid );
};




#endif // _IMAGE_MORPHOLOGY_FACTORY_CLASS_H_
