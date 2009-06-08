
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_DATA_TIME_PRIMITIVE_H__
#define __MINERVA_CORE_DATA_TIME_PRIMITIVE_H__

#include "Minerva/Core/Data/Object.h"

#include <string>

namespace Minerva { namespace Core { namespace Animate { class Date; } } }

namespace Minerva {
namespace Core {
namespace Data {


class MINERVA_EXPORT TimePrimitive : public Minerva::Core::Data::Object
{
public:
  typedef Minerva::Core::Data::Object BaseClass;
  typedef Minerva::Core::Animate::Date Date;
  
  USUL_DECLARE_REF_POINTERS ( TimePrimitive );

protected:
  
  TimePrimitive();
  virtual ~TimePrimitive();
};


}
}
}


#endif // __MINERVA_CORE_DATA_TIME_PRIMITIVE_H__
