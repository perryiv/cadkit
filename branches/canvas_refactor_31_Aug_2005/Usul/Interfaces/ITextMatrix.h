
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for setting values in a text matrix
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_TEXT_MATRIX_H_
#define _USUL_INTERFACE_TEXT_MATRIX_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>

namespace Usul {
namespace Interfaces {


struct ITextMatrix : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITextMatrix );

  /// Id for this interface.
  enum { IID = 1758536334u };

  virtual void textCellSet      ( float x, float y, unsigned int row, unsigned int col, const std::string& text ) = 0;
  virtual void textMatrixRemove ( float x, float y ) = 0;
  virtual void textMatrixResize ( float x, float y, unsigned int numRows, unsigned int numCols, int rowHeight, int columnWidth ) = 0;
};


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_TEXT_MATRIX_H_
