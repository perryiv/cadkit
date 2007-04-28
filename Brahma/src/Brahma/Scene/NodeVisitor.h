
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// Visitor for scene traversal. 
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SCENE_NODEVISITOR_H__
#define __SCENE_NODEVISITOR_H__

#include "Brahma/Core/Object.h"

namespace Brahma
{
  namespace Scene
  {
    class LeafNode;
    class Group;
    class Transform;

    class NodeVisitor : public Core::Object
    {
      public:         
        
        USUL_DECLARE_REF_POINTERS( NodeVisitor );				

        virtual void visit( LeafNode&, const bool& createNew = true )		= 0;

        virtual void visit( Group&, const bool& createNew = true )			= 0;        

				virtual void visit( Transform&, const bool& createNew = true )	= 0;        

      protected:

        virtual ~NodeVisitor()
        {
        }
    };
  }
}

#endif // __SCENE_NODEVISITOR_H__
