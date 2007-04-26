
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

				typedef Brahma::Scene::Node BaseClass;

        USUL_DECLARE_REF_POINTERS( LeafNode );                
      
         LeafNode() : BaseClass(),
          _data ( "" )
        {
          this->_addMember( "_data", _data );
        }

        const std::string&  data() const
        {
          return _data;
        }

        void data( const std::string& source )
        {
          _data = source;
        }

				virtual void accept( Brahma::Scene::NodeVisitor &nv )
				{
					nv.visit( *this );
				}

      protected:
      
        virtual ~LeafNode()
        {
        }

        std::string         _data;  

        SERIALIZE_XML_DEFINE_MEMBERS ( LeafNode );     
    };
  }
}

#endif // __BRAHMA_SCENE_LEAFNODE_H__
