
#ifndef __BRAHMA_SCENE_GROUP_H__
#define __BRAHMA_SCENE_GROUP_H__

#include "Brahma/Core/Object.h"

#include "Node.h"

#include "Export.h" 

#include <vector>
#include <string>

namespace Brahma
{ 
  namespace Scene
  {
    class SCENE_EXPORT Group: public Node
    {
      public:

        typedef Node                                BaseClass;
        typedef std::vector< BaseClass::RefPtr >    Children;


        USUL_DECLARE_REF_POINTERS( Group );

        Group() : BaseClass(), 
          _children()
        {         
          this->_addMember( "_children", _children );
        }

        const Children&                             children() const;
        void                                        children( const Children& children );        

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
