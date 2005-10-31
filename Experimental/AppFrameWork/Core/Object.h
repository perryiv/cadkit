
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


namespace AFW {
namespace Core {


class APP_FRAME_WORK_EXPORT Object : public Usul::Base::Referenced
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Base::Referenced UserData;
  typedef Usul::Base::Referenced GuiObject;
  typedef USUL_REF_POINTER ( UserData ) UserDataPtr;
  typedef USUL_REF_POINTER ( GuiObject ) GuiObjectPtr;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Object );

  // Set/get the user-data.
  void                                userData ( UserData * );
  const UserData *                    userData() const;
  UserData *                          userData();

  // Set/get the handle to the graphical object. The gui-server uses these.
  void                                guiObject ( GuiObject * );
  const UserData *                    guiObject() const;
  UserData *                          guiObject();

protected:

  // Constructor
  Object();

  // Use reference counting.
  virtual ~Object();

private:

  // No copying.
  Object ( const Object & );
  Object &operator = ( const Object & );

  UserDataPtr _userData;
  GuiObjectPtr _guiObject;
};


} // namespace Core
} // namespace AFW


#endif //_APP_FRAME_WORK_OBJECT_CLASS_H_
