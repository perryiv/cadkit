
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

#include "osg/Vec4f"

#include <string>

namespace osgText { class Text; }

namespace Usul {
namespace Interfaces {


struct ITextMatrix : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITextMatrix );

  /// Id for this interface.
  enum { IID = 1758536334u };

  /// Get text at the (x,y) on the screen.
  virtual osgText::Text*        getText    ( unsigned int x, unsigned int y ) = 0;

  /// Set text value.
  virtual void                  setText    ( unsigned int x, unsigned int y, const std::string& text, const osg::Vec4f& color, const osg::Vec4f& backDropColor = osg::Vec4f ( 0, 0, 0, 0 ) ) = 0;

  /// Remove text
  virtual void                  removeText ( unsigned int x, unsigned int y ) = 0;

}; // class ITextMatrix


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_TEXT_MATRIX_H_
