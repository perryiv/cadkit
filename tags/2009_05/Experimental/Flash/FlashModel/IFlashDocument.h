
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __FLASH_IFLASH_DOCUMENT_H__
#define __FLASH_IFLASH_DOCUMENT_H__

#include "Usul/Interfaces/IUnknown.h"


namespace Flash {


struct IFlashDocument : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IFlashDocument );
  
  /// Id for this interface.
  enum { IID = 3686096927u };
  
  /// Set/get the minimum value to show.
  virtual void                minimumSet ( double ) = 0;
  virtual double              minimumGet() const = 0;
  
  /// Set/get the maximum value to show.
  virtual void                maximumSet ( double ) = 0;
  virtual double              maximumGet() const = 0;
  
}; // struct IFlashDocument


} // End namespace Flash

#endif // __FLASH_IFLASH_DOCUMENT_H__
