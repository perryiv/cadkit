
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IDATASYNC_H__
#define __USUL_INTERFACES_IDATASYNC_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IDataSync : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDataSync );

  /// Id for this interface.
  enum { IID = 1478280416u };

  // set a data flag
  virtual void                  setDataFlag( const std::string &machine, bool value ) = 0;

  // query data state
  virtual bool                  queryDataState() = 0;

  // reset data
  virtual void                  resetData( const std::string &machine ) = 0;

}; //struct IDataSync



}
}

#endif // __USUL_INTERFACES_IDATASYNC_H__
