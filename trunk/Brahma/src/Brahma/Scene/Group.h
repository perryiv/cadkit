
#ifndef __BRAHMA_SCENE_GROUP_H__
#define __BRAHMA_SCENE_GROUP_H__

#include "Brahma/Core/Object.h"

#include "Brahma/Scene/Node.h"

#include "Export.h" 

#include <vector>
#include <string>

namespace Brahma
{ 
  namespace Scene
  {
    class SCENE_EXPORT Group: public Brahma::Scene::Node
    {
      public:

        typedef Brahma::Scene::Node                 BaseClass;
				typedef Brahma::Scene::Node::RefPtr					NodePtr;

				typedef std::vector< NodePtr >							Children;


        USUL_DECLARE_REF_POINTERS( Group );

        Group() : BaseClass(), 
          _children()
        {         
          this->_addMember( "_children", _children );
        }

        const Children&                             children() const;
        void                                        children( const Children& children );        

				virtual void																accept( NodeVisitor& nv );       

      protected:

        virtual ~Group()
        {
        }

        Children                                    _children;

        SERIALIZE_XML_DEFINE_MEMBERS ( Group );
    };
  }
}

#endif // #define __BRAHMA_SCENE_GROUP_H__
