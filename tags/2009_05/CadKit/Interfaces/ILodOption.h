
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  ILodOption: Interface for controlling which LODs to process.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_LEVEL_OF_DETAIL_OPTION_H_
#define _CADKIT_INTERFACE_LEVEL_OF_DETAIL_OPTION_H_

#include "IUnknown.h"
#include "Enum.h"


namespace CadKit
{
class ILodOption : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1043122214 };

  // Set option for processing the LODs.
  virtual void            setLodProcessOption ( const LodProcessOption &option ) = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_LEVEL_OF_DETAIL_OPTION_H_
