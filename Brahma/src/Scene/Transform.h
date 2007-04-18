
#ifndef __BRAHMA_SCENE_TRANSFORM_H__
#define __BRAHMA_SCENE_TRANSFORM_H__

#include "Core/Object.h"

#include "Group.h"

#include "Export.h" 

#include <vector>
#include <string>

namespace Brahma
{ 
  namespace Scene
  {
    class SCENE_EXPORT Transform: public Group
    {
      public:

        typedef Group                               BaseClass;

        USUL_DECLARE_REF_POINTERS( Transform );

        Transform() : BaseClass()
        {         
          this->_addMember( "_type",     _type );
          this->_addMember( "_position", _position );
          this->_addMember( "_rotation", _rotation );
          this->_addMember( "_pivot",    _pivot );
        }        
    
        const std::string&                          type() const;
        void                                        type( const std::string& type );
        

        const Usul::Math::Vec3f&                    position() const;
        void                                        position( const Usul::Math::Vec3f& position );
        
        const Usul::Math::Vec4f&                    rotation() const;  
        void                                        rotation( const Usul::Math::Vec4f& rotation );
        
        const Usul::Math::Vec3f&                    pivot() const;
        void                                        pivot( const Usul::Math::Vec3f& pivot );
        

      protected:

        virtual ~Transform()
        {
        }

        std::string                                 _type;

        Usul::Math::Vec3f                           _position;
        Usul::Math::Vec4f                           _rotation;
        Usul::Math::Vec3f                           _pivot;

        SERIALIZE_XML_DEFINE_MEMBERS ( Transform );
    };
  }
}

#endif // #define __BRAHMA_SCENE_TRANSFORM_H__
