
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Command action for opening an existing model.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_OPEN_MODEL_ACTION_CLASS_H_
#define _APP_FRAME_WORK_OPEN_MODEL_ACTION_CLASS_H_

#include "AppFrameWork/Actions/CommandAction.h"

#include <vector>
#include <string>


namespace AFW {
namespace Actions {


class APP_FRAME_WORK_EXPORT OpenModelAction : public CommandAction
{
public:

  // Typedefs.
  typedef CommandAction BaseClass;
  typedef std::vector < std::string > Names;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( OpenModelAction );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( OpenModelAction );

  // Constructor
  OpenModelAction();
  OpenModelAction ( const std::string &name );
  OpenModelAction ( const Names &names );

  // Perform the action.
  virtual void                        execute ( AFW::Core::Object *object = 0x0 );

protected:

  // Use reference counting.
  virtual ~OpenModelAction();

  void                                _askForNames();

  void                                _open ( const std::string & );

  void                                _reportErrors ( const std::string & ) const;

private:

  // No copying.
  OpenModelAction ( const OpenModelAction & );
  OpenModelAction &operator = ( const OpenModelAction & );

  Names _names;
};


} // namespace Actions
} // namespace AFW


#endif //_APP_FRAME_WORK_OPEN_MODEL_ACTION_CLASS_H_
