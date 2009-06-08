
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_KML_TIME_STAMP_H__
#define __MINERVA_LAYERS_KML_TIME_STAMP_H__

#include "Minerva/Core/Data/TimePrimitive.h"
#include "Minerva/Core/Animate/Date.h"

namespace Minerva {
namespace Core {
namespace Data {


class MINERVA_EXPORT TimeStamp : public Minerva::Core::Data::TimePrimitive
{
public:
  typedef Minerva::Core::Data::TimePrimitive BaseClass;
  
  USUL_DECLARE_REF_POINTERS ( TimeStamp );
  
  TimeStamp();
  TimeStamp ( const Date& );
  
  /// Set/get the date.
  void  when ( const Date& );
  Date  when() const;

protected:
  
  virtual ~TimeStamp();
  
private:
  
  Date _when;
};


}
}
}


#endif // __MINERVA_LAYERS_KML_TIME_STAMP_H__
