
#ifndef __BRAHMA_SCENE_BRAHMA_H__
#define __BRAHMA_SCENE_BRAHMA_H__

#include "Core/Object.h"

#include "Export.h" 

namespace Brahma
{ 
  namespace Scene
  {
    class SCENE_EXPORT Node: public Core::Object
    {
      public:

        typedef Core::Object BaseClass;

        USUL_DECLARE_REF_POINTERS( Node );

        Node() : BaseClass(),           
          _id( "" )
        {
          this->_addMember( "_id", _id );  
        }

        void id( const std::string& id )
        {
          _id = id;
        }

        const std::string& id() const
        {
          return _id;
        }        

      protected:

        virtual ~Node()
        {
        }

        std::string       _id;      
        
        SERIALIZE_XML_DEFINE_MEMBERS ( Node );
    };
  }
}

#endif // #define __BRAHMA_SCENE_BRAHMA_H__