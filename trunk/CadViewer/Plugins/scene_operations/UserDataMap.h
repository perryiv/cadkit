
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Set of unique user-data.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_PLUGIN_SCENE_OPERATIONS_USER_DATA_GROUP_H_
#define _CV_PLUGIN_SCENE_OPERATIONS_USER_DATA_GROUP_H_

#include "CompileGuard.h"
#include "UserDataBase.h"

#include <map>


namespace CV {
namespace Plugins {


class UserDataMap : public osg::Referenced
{
public:

  // Useful typedef(s).
  typedef osg::ref_ptr<UserDataMap> Ptr;
  typedef osg::Referenced BaseClass;
  typedef std::map < unsigned int, UserDataBase::Ptr > Map;
  typedef Map::size_type size_type;
  typedef Map::iterator iterator;
  typedef Map::const_iterator const_iterator;
  typedef Map::key_type key_type;

  UserDataMap();
  UserDataMap ( const UserDataMap & );

  // Iterators to the user data.
  const_iterator          begin() const { return _map.begin(); }
  iterator                begin()       { return _map.begin(); }
  const_iterator          end()   const { return _map.end(); }
  iterator                end()         { return _map.end(); }

  // Insert a user-data.
  void                    insert ( UserDataBase * );

  // Find the user-data.
  const_iterator          find ( key_type key ) const;
  iterator                find ( key_type key );

  // Clear (remove) all the user-data.
  void                    clear();

  // Erase (remove) the user-data indicated by the iterator.
  void                    erase ( iterator i );

  // The number of user-data.
  size_type               size() const { return _map.size(); }

protected:

  virtual ~UserDataMap();

private:

  Map _map;
};


}; // namespace Plugins
}; // namespace CV


#endif // _CV_PLUGIN_SCENE_OPERATIONS_USER_DATA_GROUP_H_
