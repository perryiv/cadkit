
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

%{
#include "Usul/Pointers/SmartPointer.h"
%}


namespace Usul {
  namespace Pointers {
    
    
    template
    <
    class T,
    class Config_
    >
    struct SmartPointer
    {
    public:
      SmartPointer();
      ~SmartPointer();
      
      T* operator->();
    };
  }
}
