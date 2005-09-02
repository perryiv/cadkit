
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting the document
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_DOCUMENT_H_
#define _USUL_INTERFACE_DOCUMENT_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>


namespace Usul {
namespace Interfaces {


struct IDocument : public Usul::Interfaces::IUnknown
{
  /// Format-format enumerations.
  enum Format
  {
    FILE_FORMAT_ASCII,
    FILE_FORMAT_BINARY,
    FILE_FORMAT_UNKNOWN
  };

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDocument );

  /// Id for this interface.
  enum { IID = 3762815634u };

  virtual void                          activeView ( IUnknown * ) = 0;
  virtual IUnknown *                    activeView() const = 0;

  virtual void                          addWindow   ( IUnknown *window ) = 0;
  virtual void                          addView     ( IUnknown *view   ) = 0;

  // The following window is closing
  virtual void                          closing     ( IUnknown *window ) = 0;

  /// Create default GUI
  virtual void                          createDefaultGUI ( IUnknown *caller = 0x0 ) = 0;

  virtual Format                        fileFormat() const = 0;
  virtual void                          fileFormat ( const Format & ) = 0;

  virtual void                          fileName ( const std::string &f ) = 0;
  virtual std::string                   fileName() const = 0;

  virtual bool                          fileValid() const = 0;
  virtual void                          fileValid ( bool ) = 0;

  virtual bool                          modified() const = 0;
  virtual void                          modified ( bool ) = 0;

  /// Refresh the view
  virtual void                          refreshView ( IUnknown * ) = 0;

  virtual void                          removeWindow   ( IUnknown *window ) = 0;
  virtual void                          removeView     ( IUnknown *view   ) = 0;

  virtual std::string                   typeName() const = 0;
};


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_DOCUMENT_H_
