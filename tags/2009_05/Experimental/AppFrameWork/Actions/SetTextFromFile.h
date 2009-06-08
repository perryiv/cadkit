
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Set the window's text from the contents of the given file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_SET_TEXT_FROM_FILE_ACTION_CLASS_H_
#define _APP_FRAME_WORK_SET_TEXT_FROM_FILE_ACTION_CLASS_H_

#include "AppFrameWork/Actions/UpdateAction.h"

#include <string>


namespace AFW {
namespace Actions {


class APP_FRAME_WORK_EXPORT SetTextFromFile : public UpdateAction
{
public:

  // Typedefs.
  typedef UpdateAction BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( SetTextFromFile );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( SetTextFromFile );

  // Constructor
  SetTextFromFile ( const std::string &file );

  // Perform the action.
  virtual void                        execute ( AFW::Core::Object *object = 0x0 );

protected:

  // Use reference counting.
  virtual ~SetTextFromFile();

private:

  // No copying.
  SetTextFromFile ( const SetTextFromFile & );
  SetTextFromFile &operator = ( const SetTextFromFile & );

  std::string _file;
  unsigned int _position;
};


} // namespace Actions
} // namespace AFW


#endif //_APP_FRAME_WORK_SET_TEXT_FROM_FILE_ACTION_CLASS_H_
