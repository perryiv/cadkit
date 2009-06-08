
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The wxWindows text window class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GUI_TEXT_WINDOW_CLASS_H_
#define _WX_GUI_TEXT_WINDOW_CLASS_H_

#include "AppFrameWork/Wx/CompileGuard.h"
#include "AppFrameWork/Windows/TextWindow.h"

class wxTextCtrl;


class WxTextWindow : public AFW::Windows::TextWindow
{
public:

  // Typedefs.
  typedef AFW::Windows::TextWindow BaseClass;

  // Create the internal text window.
  virtual bool                        create ( AFW::Windows::Window * );

  // Detach from gui object.
  virtual void                        detach();

  // Get the text control.
  const wxTextCtrl *                  get() const;
  wxTextCtrl *                        get();

  // Set/get/append the text.
  virtual void                        textAppend ( const std::string &t );
  virtual void                        textAppend ( const char *t, unsigned int length );
  virtual void                        textSet ( const std::string &t );
  virtual void                        textSet ( const char *t, unsigned int length );

protected:

  // Default construction.
  WxTextWindow();

  // Use reference counting.
  virtual ~WxTextWindow();

  void                                _textAppend ( const char *text, unsigned int length );
  void                                _textSet    ( const char *text, unsigned int length );

private:

  // Do not copy.
  WxTextWindow ( const WxTextWindow & );
  WxTextWindow &operator = ( const WxTextWindow & );

  AFW_DECLARE_OBJECT ( WxTextWindow );
};


#endif // _WX_GUI_TEXT_WINDOW_CLASS_H_
