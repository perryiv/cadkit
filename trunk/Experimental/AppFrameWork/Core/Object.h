
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all objects.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_OBJECT_CLASS_H_
#define _APP_FRAME_WORK_OBJECT_CLASS_H_

#include "AppFrameWork/Core/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <list>


namespace AFW {
namespace Core {


class APP_FRAME_WORK_EXPORT Object : public Usul::Base::Referenced
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Base::Referenced UserData;
  typedef USUL_REF_POINTER ( UserData ) UserDataPtr;
  typedef std::list < Object * > ObjectList; // Not reference-counted by design!
  typedef ObjectList::iterator ObjectListItr;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Object );

  // Iterators to the list of all objects.
  static ObjectListItr                allObjectsBegin();
  static ObjectListItr                allObjectsEnd();

  // Set/get the user-data.
  void                                userData ( UserData * );
  const UserData *                    userData() const;
  UserData *                          userData();

protected:

  // Constructor
  Object();

  // Use reference counting.
  virtual ~Object();

private:

  // No copying.
  Object ( const Object & );
  Object &operator = ( const Object & );

  static ObjectList _allObjects;
  ObjectListItr _whichObject;
  UserDataPtr _userData;
};


} // namespace Core
} // namespace AFW


#endif //_APP_FRAME_WORK_OBJECT_CLASS_H_
