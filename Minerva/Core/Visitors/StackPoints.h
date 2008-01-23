
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_VISITORS_STACK_POINTS_H__
#define __MINERVA_VISITORS_STACK_POINTS_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Visitor.h"

#include <map>

namespace Minerva {
  namespace Core {
    namespace Visitors {
      
      class MINERVA_EXPORT StackPoints : public Minerva::Core::Visitor
      {
      public:
        typedef Minerva::Core::Visitor BaseClass;
        typedef Minerva::Core::Animate::Date Date;
        
        USUL_DECLARE_REF_POINTERS ( StackPoints );
        
        StackPoints ();
        
        virtual void visit ( Minerva::Core::DataObjects::Point &object );
        virtual void visit ( Minerva::Core::Layers::PointLayer &layer );
      protected:
        /// Use reference counting.
        virtual ~StackPoints ();
        
      private:
        typedef std::map< osg::Vec3, unsigned int > Counts;
        Counts _counts;
      };
      
      
    }
  }
}

#endif // __MINERVA_VISITORS_STACK_POINTS_H__
