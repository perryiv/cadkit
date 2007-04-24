
#ifndef __SCENE_BASENODEVISITOR_H__
#define __SCENE_BASENODEVISITOR_H__

#include "Brahma/Core/Object.h"

namespace Brahma
{
  namespace Scene
  {
    class Node;
    class Group;
    class Transform;

    class BaseNodeVisitor : public Core::Object
    {
      public:         
        
        USUL_DECLARE_REF_POINTERS( BaseNodeVisitor );

        virtual void apply( Node& )         = 0;

        virtual void apply( Grroup& )       = 0;

        virtual void apply( Transform& )    = 0;

      protected:

        virtual ~BaseNodeVisitor()
        {
        }
    };
  }
}

#endif // __SCENE_BASENODEVISITOR_H__