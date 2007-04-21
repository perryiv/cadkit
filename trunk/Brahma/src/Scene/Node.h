
#ifndef __BRAHMA_SCENE_NODE_H__
#define __BRAHMA_SCENE_NODE_H__

#include "Core/Object.h"

#include "Scene/Database.h"

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
          _id   ( "" ), 
          _database( 0x00 )
        {
          this->_addMember( "_id", _id );  
          this->_addMember( "_database", _database );
          //this->_addMember( "_data", _data );
        }

        const std::string&  id() const;        
        void                id( const std::string& id );

        Database*           database();
        void                database( Database* db );        

        
        //const std::string&  data() const;
        //void                data( const std::string& source );        

      protected:     
      
        virtual ~Node()
        {
        }

        std::string           _id;          
        //std::string         _data;  

        Database::RefPtr      _database;

        SERIALIZE_XML_DEFINE_MEMBERS ( Node );     
    };
  }
}

#endif // #define __BRAHMA_SCENE_NODE_H__
