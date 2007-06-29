
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

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

namespace Minerva
{
  namespace Core
  {
    namespace DataObjects { class DataObject; }
    namespace Layers      { class Layer;      }

    class MINERVA_EXPORT Visitor : public Usul::Base::Referenced
    {
    public:
      // Typedefs.
      typedef Usul::Base::Referenced BaseClass;

      USUL_DECLARE_REF_POINTERS ( Visitor );

      Visitor() : BaseClass()
      {
      }

      virtual void visit ( Minerva::Core::DataObjects::DataObject &dataObject ) = 0;
      virtual void visit ( Minerva::Core::Layers::Layer& layer ) = 0;

    protected:
      virtual ~Visitor()
      {
      }
    };
  }
}

#endif // __MINERVA_CORE_VISITOR_H__
