
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Window factory class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_GUI_SERVER_WINDOW_FACTORY_CLASS_H_
#define _FOX_GUI_SERVER_WINDOW_FACTORY_CLASS_H_

#include "AppFrameWork/Fox/CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"


class FoxObjectFactory : public Usul::Base::Referenced
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( FoxObjectFactory );

  // Default construction.
  FoxObjectFactory();

protected: 

  // Do not copy.
  FoxObjectFactory ( const FoxObjectFactory & );
  FoxObjectFactory &operator = ( const FoxObjectFactory & );

  // Use reference counting.
  virtual ~FoxObjectFactory();
};


#endif // _FOX_GUI_SERVER_WINDOW_FACTORY_CLASS_H_
