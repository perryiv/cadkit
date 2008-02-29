
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
#include "Minerva/Core/Layers/Vector.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Cast/Cast.h"

namespace Minerva
{
  namespace Core
  {
    namespace DataObjects 
    { 
      class DataObject;
      class Line;
      class Point;
      class Polygon;
      class Model;
    }

    namespace Layers 
    {
      class VectorGroup;
    }

    class MINERVA_EXPORT Visitor : public Usul::Base::Referenced
    {
    public:

      // Typedefs.
      typedef Usul::Base::Referenced                  BaseClass;
      typedef Minerva::Core::DataObjects::DataObject  DataObject;
      typedef Minerva::Core::Layers::Vector           Vector;

      USUL_DECLARE_REF_POINTERS ( Visitor );

      Visitor() : BaseClass()
      {
      }

      virtual void visit ( Minerva::Core::DataObjects::DataObject &dataObject )
      {
      }

      virtual void visit ( Minerva::Core::DataObjects::Line &line )
      {
        this->visit ( USUL_UNSAFE_CAST ( DataObject&, line ) );
      }

      virtual void visit ( Minerva::Core::DataObjects::Point &point )
      {
        this->visit ( USUL_UNSAFE_CAST ( DataObject&, point ) );
      }

      virtual void visit ( Minerva::Core::DataObjects::Polygon &polygon )
      {
        this->visit ( USUL_UNSAFE_CAST ( DataObject&, polygon ) );
      }
      
      virtual void visit ( Minerva::Core::DataObjects::Model &model )
      {
        this->visit ( USUL_UNSAFE_CAST ( DataObject&, model ) );
      }

      virtual void visit ( Minerva::Core::Layers::Vector& vector )
      {
        vector.traverse ( *this );
      }
      
      virtual void visit ( Minerva::Core::Layers::VectorGroup& group )
      {
        this->visit ( USUL_UNSAFE_CAST ( Vector&, group ) );
      }

    protected:
      virtual ~Visitor()
      {
      }
    };
  }
}

#endif // __MINERVA_CORE_VISITOR_H__
