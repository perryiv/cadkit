
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IRASTER_ALPHAS_H__
#define __USUL_INTERFACES_IRASTER_ALPHAS_H__

#include "Usul/Interfaces/IUnknown.h"

#include "Usul/Math/Vector3.h"
#include "Usul/Predicates/LessVector.h"

#include <functional>
#include <map>

namespace Usul {
namespace Interfaces {
    
    
struct IRasterAlphas : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IRasterAlphas );
      
  /// Id for this interface.
  enum { IID = 2453397727u };
  
  typedef Usul::Math::Vec3uc Color;
  typedef std::equal_to < unsigned char > EqualPredicate;
  typedef Usul::Predicates::LessVector < EqualPredicate, 3 > LessColor;
  typedef std::map < Usul::Math::Vec3uc, unsigned char, LessColor > Alphas;
  
  // Add an alpha value.
  virtual void                  alpha ( unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha ) = 0;
  
  // Get the alpha values.
  virtual Alphas                alphas() const = 0;
  
}; // struct IRasterAlphas
    
    
} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_IRASTER_ALPHAS_H__ */

