
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Utility class to work work with user-data.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_PLUGIN_SCENE_OPERATIONS_USER_DATA_TOOL_H_
#define _CV_PLUGIN_SCENE_OPERATIONS_USER_DATA_TOOL_H_

#include "CompileGuard.h"
#include "UserDataMap.h"

#include "CadViewer/ErrorChecker.h"

#include "osg/Object"


namespace CV {
namespace Plugins {
namespace Detail {


///////////////////////////////////////////////////////////////////////////////
//
//  Generic definition.
//
///////////////////////////////////////////////////////////////////////////////

template < class UserDataType_ > struct UserDataTool
{
  /////////////////////////////////////////////////////////////////////////////
  //
  //  See if the user-data exists.
  //
  /////////////////////////////////////////////////////////////////////////////

  static bool has ( const osg::Object* object )
  {
    CV::ErrorChecker ( 1071044577, 0x0 != object );

    // See if there is a user-data-map.
    const UserDataMap *map = dynamic_cast < const UserDataMap * > ( object->getUserData() );
    if ( map )
    {
      // Look for a user-data of the given type.
      return ( map->end() != map->find ( UserDataType_::classId() ) );
    }

    // No user-data map, so no user-data.
    return false;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Remove the user-data.
  //
  /////////////////////////////////////////////////////////////////////////////

  static void remove ( osg::Object* object )
  {
    CV::ErrorChecker ( 1071025314, 0x0 != object );

    // See if there is a user-data-map.
    UserDataMap::Ptr map ( dynamic_cast < UserDataMap * > ( object->getUserData() ) );
    if ( map.valid() )
    {
      // Look for a user-data of the given type.
      UserDataMap::iterator i = map->find ( UserDataType_::classId() );
      if ( map->end() != i )
      {
        // Remove it.
        map->erase ( i );
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return (perhaps create) the user-data.
  //
  /////////////////////////////////////////////////////////////////////////////

  static UserDataType_ *get ( osg::Object* object, bool create )
  {
    CV::ErrorChecker ( 1070867202, 0x0 != object );

    // See if there is a user-data-map.
    UserDataMap::Ptr map ( dynamic_cast < UserDataMap * > ( object->getUserData() ) );
    if ( map.valid() )
    {
      // Look for a user-data of the given type.
      UserDataMap::iterator i = map->find ( UserDataType_::classId() );
      if ( map->end() != i )
      {
        // Return the user-data.
        UserDataBase *base = (*i).second.get();
        return ( dynamic_cast < UserDataType_ * > ( base ) );
      }

      // If we are not supposed to create one then we are done.
      if ( !create )
        return 0x0;

      // Add a new one to the map.
      typename UserDataType_::Ptr ud ( new UserDataType_ );
      map->insert ( ud.get() );
      return ud.get();
    }

    // Otherwise...
    else
    {
      // If we are not supposed to creste one then we are done.
      if ( !create )
        return 0x0;

      // Should be true.
      CV::ErrorChecker ( 1071027151, 0x0 == object->getUserData() );

      // Add a map and call this function recursively.
      object->setUserData ( new UserDataMap );
      return get ( object, create );
    }
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Specialization for UserDataMap.
//
///////////////////////////////////////////////////////////////////////////////

template <> struct UserDataTool < UserDataMap >
{
  /////////////////////////////////////////////////////////////////////////////
  //
  //  See if the user-data exists.
  //
  /////////////////////////////////////////////////////////////////////////////

  static bool has ( const osg::Object* object )
  {
    CV::ErrorChecker ( 1071044578, 0x0 != object );
    return ( 0x0 != dynamic_cast < const UserDataMap * > ( object->getUserData() ) );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Remove the user-data.
  //
  /////////////////////////////////////////////////////////////////////////////

  static void remove ( osg::Object* object )
  {
    CV::ErrorChecker ( 1071026862, 0x0 != object );
  
    // See if there is a user-data-map.
    UserDataMap::Ptr map ( dynamic_cast < UserDataMap * > ( object->getUserData() ) );
    if ( map.valid() )
    {
      // Set it to null.
      object->setUserData ( 0x0 );
    }

    // Otherwise...
    else
    {
      // Should be true.
      CV::ErrorChecker ( 1071027152, 0x0 == object->getUserData() );
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return (perhaps create) the user-data.
  //
  /////////////////////////////////////////////////////////////////////////////

  static UserDataMap *get ( osg::Object* object, bool create )
  {
    CV::ErrorChecker ( 1071026863, 0x0 != object );

    // See if there is a user-data-map.
    UserDataMap::Ptr map ( dynamic_cast < UserDataMap * > ( object->getUserData() ) );
    if ( false == map.valid() )
    {
      // Should be true.
      CV::ErrorChecker ( 1071027153, 0x0 == object->getUserData() );

      // If we are not supposed to create one then we are done.
      if ( !create )
        return 0x0;

      // Add one to the object.
      map = new UserDataMap;
      object->setUserData ( map.get() );
    }

    // Return the map.
    return map.get();
  }
};


}; // namespace Detail
}; // namespace Plugins
}; // namespace CV


#endif // _CV_PLUGIN_SCENE_OPERATIONS_USER_DATA_TOOL_H_
