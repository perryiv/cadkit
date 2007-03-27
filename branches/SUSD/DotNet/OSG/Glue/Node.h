
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

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
        Node( osg::Node *node );
        Node( System::IntPtr ptr );
        ~Node();
        !Node();

        void            clear();

        osg::Node*      node();
        System::IntPtr  nodePtr();


      private:
        void            _setNode( osg::Node* );
        void            _setNodePtr ( System::IntPtr );

        osg::Node *_node;
	    };
    }
  }
}
