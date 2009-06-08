
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for modflow document.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_INTERFACES_MODFLOW_MODEL_H_
#define _MODFLOW_INTERFACES_MODFLOW_MODEL_H_

#include "Usul/Interfaces/IUnknown.h"

#include "Usul/Types/Types.h"


namespace Modflow {
namespace Interfaces {


struct IModflowModel : public Usul::Interfaces::IUnknown
{
  typedef Usul::Types::Uint64 TimeStamp;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IModflowModel );

  // Id for this interface.
  enum { IID = 1043603230u };

  // Return the number of time-stamps.
  virtual unsigned int                    numTimeStamps() const = 0;

  // Set/get the i'th time-stamp.
  virtual void                            timeStampSet ( unsigned int i, const TimeStamp &stamp ) = 0;
  virtual bool                            timeStampGet ( unsigned int i, TimeStamp &stamp ) const = 0;

  // Set/get the vertical scale.
  virtual void                            setVerticalScale ( double ) = 0;
  virtual double                          getVerticalScale() const = 0;

  // Set/get the cell margin.
  virtual void                            setCellMargin ( double ) = 0;
  virtual double                          getCellMargin() const = 0;
};


} // end namespace Interfaces
} // end namespace Modflow


#endif // _MODFLOW_INTERFACES_MODFLOW_MODEL_H_
