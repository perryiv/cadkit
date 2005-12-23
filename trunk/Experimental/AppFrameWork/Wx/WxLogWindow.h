
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The wxWindows log window class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GUI_LOG_WINDOW_CLASS_H_
#define _WX_GUI_LOG_WINDOW_CLASS_H_

#include "AppFrameWork/Wx/CompileGuard.h"
#include "AppFrameWork/Core/LogWindow.h"

class wxTextCtrl;


class WxLogWindow : public AFW::Core::LogWindow
{
public:

  // Typedefs.
  typedef AFW::Core::LogWindow BaseClass;

  // Create the internal window.
  virtual bool                        create ( AFW::Core::Window * );

  // Detach from gui object.
  virtual void                        detach();

  // Get the log window.
  const wxPanel *                     get() const;
  wxPanel *                           get();

  // Set/get/append the text.
  virtual void                        textAppend ( const std::string &t );
  virtual void                        textAppend ( const char *t, unsigned int length );
  virtual void                        textSet ( const std::string &t );
  virtual void                        textSet ( const char *t, unsigned int length );

protected:

  // Default construction.
  WxLogWindow();

  // Use reference counting.
  virtual ~WxLogWindow();

  void                                _textAppend ( const char *text, unsigned int length );
  void                                _textSet    ( const char *text, unsigned int length );

private:

  // Do not copy.
  WxLogWindow ( const WxLogWindow & );
  WxLogWindow &operator = ( const WxLogWindow & );

  AFW_DECLARE_OBJECT ( WxLogWindow );
};


#endif // _WX_GUI_LOG_WINDOW_CLASS_H_
