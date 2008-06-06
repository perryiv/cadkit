
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_ANIMATE_TIME_SPAN_H__
#define __MINERVA_CORE_ANIMATE_TIME_SPAN_H__  

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Animate/Date.h"

#include "Usul/Base/Object.h"
#include "Usul/Pointers/Pointers.h"

#include "Serialize/XML/Macros.h"

namespace Minerva {
namespace Core  {
namespace Animate {

class MINERVA_EXPORT TimeSpan : public Usul::Base::Object 
{
public:
  typedef Usul::Base::Object           BaseClass;
  typedef Minerva::Core::Animate::Date Date;

  USUL_DECLARE_REF_POINTERS ( TimeSpan );

  TimeSpan();

  /// First date of span.
  void             firstDate( const Date& date );
  Date             firstDate() const;

  /// Last date span.
  void             lastDate( const Date& date );
  Date             lastDate() const;

  /// Get the name of the span.
  std::string      name () const;
  
protected:
  virtual ~TimeSpan();

private:
  Date _firstDate;
  Date _lastDate;

  SERIALIZE_XML_DEFINE_MEMBERS ( TimeSpan );
  SERIALIZE_XML_DEFINE_MAP;
};


} // namespace Animate
} // namespace Core
} // namespace Mienrva

#endif // __MINERVA_CORE_ANIMATE_TIME_SPAN_H__
