
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  User-data for state-sets.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_PLUGIN_SCENE_OPERATIONS_USER_DATA_STATE_H_
#define _CV_PLUGIN_SCENE_OPERATIONS_USER_DATA_STATE_H_

#include "CompileGuard.h"
#include "UserDataBase.h"

#include "osg/StateSet"

#include <stack>


namespace CV {
namespace Plugins {


class UserDataState : public UserDataBase
{
public:

  CV_SCENE_OPERATIONS_DECLARE_USER_DATA ( UserDataState );
  typedef UserDataBase BaseClass;
  typedef osg::ref_ptr < osg::StateSet > StateSetPtr;
  typedef std::stack < StateSetPtr > Stack;
  typedef Stack::size_type size_type;

  UserDataState();
  UserDataState ( const UserDataState & );

  // Push/pop the state-set.
  void                    push ( osg::StateSet * );
  void                    pop();

  // Get the top state-set.
  const osg::StateSet *   top() const;
  osg::StateSet *         top();

  // See if the stack is empty.
  bool                    empty() const { return _states.empty(); }

  // Get the size of the stack.
  size_type               size() const { return _states.size(); }

protected:

  virtual ~UserDataState();

private:

  Stack _states;
};


}; // namespace Plugins
}; // namespace CV


#endif // _CV_PLUGIN_SCENE_OPERATIONS_USER_DATA_STATE_H_
