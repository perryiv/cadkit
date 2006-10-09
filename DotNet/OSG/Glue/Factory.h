
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#pragma once

#include "osg/Node"
#include "osg/ref_ptr"

#include <map>

namespace CadKit
{
  namespace OSG
  {
    namespace Glue
    {
      class Factory
      {
      public:
        static Factory& instance();

        void            create ( unsigned int key );
        osg::Node *     node   ( unsigned int key );
        void            node   ( unsigned int key, osg::Node* );
        void            remove ( unsigned int key );
      private:
        typedef std::map < unsigned int, osg::ref_ptr < osg::Node > > NodeMap;
        Factory();

        static Factory *_instance;

        NodeMap _map;
      };
    }
  }
}
