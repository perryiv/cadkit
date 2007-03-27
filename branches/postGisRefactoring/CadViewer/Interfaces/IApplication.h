
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Application interface.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_INTERFACE_APPLICATION_H_
#define _CV_INTERFACE_APPLICATION_H_

#include "Usul/Interfaces/IUnknown.h"


namespace CV {
namespace Interfaces {


struct IApplication : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IApplication );

  /// Id for this interface.
  enum { IID = 1075273987 };
};


}; // namespace Interfaces
}; // namespace CV


#endif // _CV_INTERFACE_APPLICATION_H_
