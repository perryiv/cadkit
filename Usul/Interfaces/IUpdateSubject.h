
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface to add caller to list of things to be updated.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IUPDATE_SUBJECT_H__
#define __USUL_INTERFACES_IUPDATE_SUBJECT_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IUpdateSubject : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IUpdateSubject );

  /// Id for this interface.
  enum { IID = 2658302544u };

  virtual void addUpdateListener    ( Usul::Interfaces::IUnknown *listener ) = 0;
  virtual void removeUpdateListener ( Usul::Interfaces::IUnknown *listener ) = 0;

}; // struct IUpdateSubject


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_IUPDATE_SUBJECT_H__ */
