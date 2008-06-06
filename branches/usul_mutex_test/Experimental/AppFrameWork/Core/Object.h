
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
#include "AppFrameWork/Core/Define.h"
#include "AppFrameWork/Core/NewObjectFunctor.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/Variable.h"

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
  typedef Usul::Threads::Variable < ObjectList > ObjectListVar;
  typedef ObjectList::iterator ObjectListItr;
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard < Mutex > Guard;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Object );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( Object );

  // Iterators to the list of all objects.
  static ObjectListItr                allObjectsBegin();
  static ObjectListItr                allObjectsEnd();

  // Detach from gui object.
  virtual void                        detach();

  // Access to the mutex.
  Mutex &                             mutex() const;

  // Unreference the instance.
  void                                unref ( bool allowDeletion = true );

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

  static ObjectListVar _allObjects;
  mutable Mutex _mutex;
  ObjectListItr _whichObject;
  UserDataPtr _userData;
};


} // namespace Core
} // namespace AFW


#endif //_APP_FRAME_WORK_OBJECT_CLASS_H_
