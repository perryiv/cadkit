
#ifndef __BRAHMA_CORE_OBJECT_H__
#define __BRAHMA_CORE_OBJECT_H__

#include "Serialize/XML/Serialize.h"
#include "Serialize/XML/Deserialize.h"
#include "Serialize/XML/RegisterCreator.h"
#include "Serialize/XML/SimpleDataMember.h"
#include "Serialize/XML/DataMemberMap.h"
#include "Serialize/XML/Macros.h"
#include "Serialize/XML/SmartPointerMember.h"

namespace Brahma
{
  namespace Core
  {
    class Object : public Usul::Base::Referenced
    {
      public:
        
        typedef Usul::Base::Referenced BaseClass;

        USUL_DECLARE_REF_POINTERS( Object );

        Object() : BaseClass(), 
          SERIALIZE_XML_INITIALIZER_LIST
        {
        }
      
      protected:

        virtual ~Object()
        {
        }

        SERIALIZE_XML_DEFINE_MAP;
        SERIALIZE_XML_DEFINE_MEMBERS( Object );
    };
  }
}
#endif // __BRAHMA_CORE_OBJECT_H__