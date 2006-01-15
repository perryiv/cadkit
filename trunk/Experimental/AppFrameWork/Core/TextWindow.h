
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Displays text.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_TEXT_WINDOW_CLASS_H_
#define _APP_FRAME_WORK_TEXT_WINDOW_CLASS_H_

#include "AppFrameWork/Core/Window.h"

#include <string>


namespace AFW {
namespace Core {


class APP_FRAME_WORK_EXPORT TextWindow : public Window
{
public:

  // Typedefs.
  typedef Window BaseClass;

  // Accept the visitor.
  virtual void                        accept ( AFW::Core::BaseVisitor * );

  // If this string is not empty then it is interpreted as the menu in which 
  // a check-button should be added to hide/show this window.
  void                                menu ( const std::string &text );
  std::string                         menu() const;

protected:

  // Constructor
  TextWindow();

  // Use reference counting.
  virtual ~TextWindow();

private:

  // No copying.
  TextWindow ( const TextWindow & );
  TextWindow &operator = ( const TextWindow & );

  std::string _menu;

  AFW_DECLARE_OBJECT ( TextWindow );
};


} // namespace Core
} // namespace AFW


#endif //_APP_FRAME_WORK_TEXT_WINDOW_CLASS_H_
