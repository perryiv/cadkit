
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#pragma once

using namespace System;

#include "osg/Node"

namespace CadKit 
{
  namespace OSG 
  { 
    namespace Glue 
    {

	    public ref class Node
	    {
      public:
		    Node();
        ~Node();
        !Node();

        osg::Node*      node();
        void            node( osg::Node* );
        System::IntPtr  nodePtr();
        void            nodePtr ( System::IntPtr );

      protected:
        void            _unreference();
      private:
        osg::Node *_node;
	    };
    }
  }
}
