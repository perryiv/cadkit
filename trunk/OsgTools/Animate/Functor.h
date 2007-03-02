
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSGTOOLS_ANIMATE_FUNCTOR_H__
#define __OSGTOOLS_ANIMATE_FUNCTOR_H__

#include "OsgTools/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

namespace OsgTools
{
  namespace Animate
  {
    class Date;

    class OSG_TOOLS_EXPORT Functor : public Usul::Base::Referenced
    {
    public:
      typedef Usul::Base::Referenced BaseClass;
      
      USUL_DECLARE_REF_POINTERS ( Functor );

      virtual void execute ( const OsgTools::Animate::Date& first, const OsgTools::Animate::Date& last ) = 0;
    };
  }
}

#endif // __OSGTOOLS_ANIMATE_FUNCTOR_H__
