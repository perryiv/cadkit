
#ifndef __BRAHMA_SCENE_FILENODE_H__
#define __BRAHMA_SCENE_FILENODE_H__

#include "Brahma/Core/Object.h"

#include "Export.h"

namespace Brahma
{
  namespace Scene
  {
    class SCENE_EXPORT Database : public Core::Object
    {
      public:

        typedef Core::Object BaseClass;

        USUL_DECLARE_REF_POINTERS( Database );                
      
         Database() : BaseClass(),
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

      protected:
      
      

        virtual ~Database()
        {
        }

        std::string         _data;  

        SERIALIZE_XML_DEFINE_MEMBERS ( Database );     
    };
  }
}

#endif // __BRAHMA_SCENE_FILENODE_H__