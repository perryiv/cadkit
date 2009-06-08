
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
#include "AppFrameWork/Windows/LogWindow.h"

#include <list>

class wxTextCtrl;


class WxLogWindow : public AFW::Windows::LogWindow
{
public:

  // Typedefs.
  typedef AFW::Windows::LogWindow BaseClass;
  typedef std::list < std::string > Strings;

  // Create the internal window.
  virtual bool                        create ( AFW::Windows::Window * );

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

  std::string                         _getImage ( const std::string & ) const;

  void                                _update();

private:

  // Do not copy.
  WxLogWindow ( const WxLogWindow & );
  WxLogWindow &operator = ( const WxLogWindow & );

  AFW::Core::Icon _error;
  AFW::Core::Icon _info;
  AFW::Core::Icon _warning;

  AFW_DECLARE_OBJECT ( WxLogWindow );
};


#endif // _WX_GUI_LOG_WINDOW_CLASS_H_
