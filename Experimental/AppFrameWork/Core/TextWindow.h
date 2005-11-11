
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


namespace AFW {
namespace Core {


class APP_FRAME_WORK_EXPORT TextWindow : public Window
{
public:

  // Typedefs.
  typedef Window BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( TextWindow );

  // Constructor
  TextWindow();

  // Accept the visitor.
  virtual void                        accept ( AFW::Core::BaseVisitor * );

  // Set/get/append the text.
  virtual void                        textAppend ( const std::string & );
  virtual void                        textAppend ( const char *s, unsigned int length );
  virtual std::string                 textGet() const;
  virtual void                        textGet ( std::string & ) const;
  virtual void                        textSet ( const std::string & );
  virtual void                        textSet ( const char *s, unsigned int length );

  // Append an action.
  virtual void                        append ( AFW::Actions::CommandAction * );
  virtual void                        append ( AFW::Conditions::Condition *, AFW::Actions::UpdateAction * );

protected:

  // Use reference counting.
  virtual ~TextWindow();

private:

  // No copying.
  TextWindow ( const TextWindow & );
  TextWindow &operator = ( const TextWindow & );
};


} // namespace Core
} // namespace AFW


#endif //_APP_FRAME_WORK_TEXT_WINDOW_CLASS_H_
