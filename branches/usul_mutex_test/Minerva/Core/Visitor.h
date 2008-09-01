
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_VISITOR_H__
#define __MINERVA_CORE_VISITOR_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Macros.h"

#include "Usul/Base/Object.h"
#include "Usul/Pointers/Pointers.h"

namespace Minerva
{
  namespace Core
  {
    namespace DataObjects 
    { 
      class DataObject;
    }

    namespace Layers
    {
      class Container;
    }
    
    namespace TileEngine
    {
      class Node;
      class Group;
      class Body;
      class System;
    }

    class MINERVA_EXPORT Visitor : public Usul::Base::Object
    {
    public:

      // Typedefs.
      typedef Usul::Base::Object                      BaseClass;
      typedef Minerva::Core::DataObjects::DataObject  DataObject;
      typedef Minerva::Core::Layers::Container        Container;

      MINERVA_DEFINE_VISITOR_CLASS ( Visitor );

      virtual void visit ( DataObject &dataObject );

      virtual void visit ( Container& container );
      
      virtual void visit ( Minerva::Core::TileEngine::System & );
      virtual void visit ( Minerva::Core::TileEngine::Group & );
      virtual void visit ( Minerva::Core::TileEngine::Body & );
      virtual void visit ( Minerva::Core::TileEngine::Node & );

    protected:
      
      // Constructor
      Visitor();
      
      // Use reference counting.
      virtual ~Visitor();
      
    private:
      
      // No copying or assignment.
      Visitor ( const Visitor & );
      Visitor &operator = ( const Visitor & );
      
      void                      _destroy();
    };
  }
}

#endif // __MINERVA_CORE_VISITOR_H__