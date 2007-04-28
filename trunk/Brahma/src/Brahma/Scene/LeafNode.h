
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
//  Node that contains the data. 
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __BRAHMA_SCENE_LEAFNODE_H__
#define __BRAHMA_SCENE_LEAFNODE_H__

#include "Brahma/Scene/Node.h"
#include "Brahma/Scene/NodeVisitor.h"

#include "Export.h"

namespace Brahma
{
  namespace Scene
  {
		class SCENE_EXPORT LeafNode : public Brahma::Scene::Node
    {
      public:

				///////////////////////////////////////////////////////////////////////
				//	
				//  Easy to remember names. 
				//
				///////////////////////////////////////////////////////////////////////

				typedef Brahma::Scene::Node BaseClass;

        USUL_DECLARE_REF_POINTERS( LeafNode );                

        LeafNode();

        const std::string&											data() const;
        void																		data( const std::string& source );

				virtual void														accept( Brahma::Scene::NodeVisitor &nv );

      protected:
      
        virtual ~LeafNode();

        std::string															_data;  

        SERIALIZE_XML_DEFINE_MEMBERS ( LeafNode );     
    };
  }
}

#endif // __BRAHMA_SCENE_LEAFNODE_H__
