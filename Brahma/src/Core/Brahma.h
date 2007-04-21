
#ifndef __BRAHMA_CORE_BRAHMA_H__
#define __BRAHMA_CORE_BRAHMA_H__

//#define BRAHMA_SHARED 

#include "XmlTree/XercesLife.h"

#include "Serialize/XML/Serialize.h"
#include "Serialize/XML/Deserialize.h"
#include "Serialize/XML/RegisterCreator.h"
#include "Serialize/XML/SimpleDataMember.h"
#include "Serialize/XML/DataMemberMap.h"
#include "Serialize/XML/Macros.h"
#include "Serialize/XML/SmartPointerMember.h"

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Base/Referenced.h"

#include "Scene/Node.h"

#include "Object.h"
#include "Export.h"

#include <vector>

namespace Brahma
{
  namespace Core
  {    
    class CORE_EXPORT Brahma : public Usul::Base::Referenced
    {
      public:

        typedef Usul::Base::Referenced                            BaseClass;  
        typedef Object::RefPtr                                    BaseObject;
        typedef std::vector< BaseObject >                         BaseObjects;

        USUL_DECLARE_REF_POINTERS( Brahma );

        Brahma() : BaseClass()
        {
        }

        const BaseObjects&                    objects() const;

        void                                  load( const std::string& config );

        void                                  serialize( const std::string& config );

        void deserialize();

        BaseObject                            query( const std::string& name );

        void                                  clean();

        template< class T >
        void registerType( const std::string& name )
        {
          Serialize::XML::Factory::instance().add ( new Serialize::XML::TypeCreator< T > ( name ) );
        }


      protected:

        virtual ~Brahma()
        {
          clean();
        }

      private:

        std::string                         _inputConfig;

        BaseObjects                         _baseObjects;
        
    };    
  }
}

#endif // __BRAHMA_CORE_BRAHMA_H__