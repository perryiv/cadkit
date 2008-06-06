
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Node.h"

namespace osg { class Group; }

namespace CadKit 
{
  namespace OSG 
  { 
    namespace Glue 
    {
      public ref class Group : public Node
	    {
      public:
        typedef Node BaseClass;

        Group();
        Group( osg::Group *group );
        Group( System::IntPtr ptr );
        ~Group();
        !Group();

        osg::Group*     group();
	    };
    }
  }
}
