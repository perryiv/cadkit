
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for user-data.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_PLUGIN_SCENE_OPERATIONS_USER_DATA_H_
#define _CV_PLUGIN_SCENE_OPERATIONS_USER_DATA_H_

#include "CompileGuard.h"

#include "osg/Referenced"
#include "osg/ref_ptr"


namespace CV {
namespace Plugins {


class UserDataBase : public osg::Referenced
{
public:

  // Useful typedef(s).
  typedef osg::ref_ptr<UserDataBase> Ptr;
  typedef osg::Referenced BaseClass;

  virtual unsigned int id() const = 0;
  static unsigned int classId() { return 1070872262; }

protected:

  UserDataBase();
  UserDataBase ( const UserDataBase & );
  virtual ~UserDataBase();
};


}; // namespace Plugins
}; // namespace CV


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience macros so that classes can easily implement some members.
//
///////////////////////////////////////////////////////////////////////////////

#define CV_SCENE_OPERATIONS_DECLARE_USER_DATA(class_name) \
  typedef osg::ref_ptr<class_name> Ptr; \
  virtual unsigned int id() const; \
  static unsigned int classId()

#define CV_SCENE_OPERATIONS_IMPLEMENT_USER_DATA(class_name,the_id) \
  unsigned int class_name::classId() \
  { \
    return the_id; \
  } \
  unsigned int class_name::id() const \
  { \
    return class_name::classId(); \
  }


#endif // _CV_PLUGIN_SCENE_OPERATIONS_USER_DATA_H_
