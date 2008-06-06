
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  User-data for designating a selected state.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_PLUGIN_SCENE_OPERATIONS_USER_DATA_SELECTED_H_
#define _CV_PLUGIN_SCENE_OPERATIONS_USER_DATA_SELECTED_H_

#include "CompileGuard.h"
#include "UserDataBase.h"

#include "osg/ref_ptr"


namespace CV {
namespace Plugins {


class UserDataSelected : public UserDataBase
{
public:

  CV_SCENE_OPERATIONS_DECLARE_USER_DATA ( UserDataSelected );
  typedef UserDataBase BaseClass;

  UserDataSelected();
  UserDataSelected ( const UserDataSelected & );

protected:

  virtual ~UserDataSelected();
};


}; // namespace Plugins
}; // namespace CV


#endif // _CV_PLUGIN_SCENE_OPERATIONS_USER_DATA_SELECTED_H_
