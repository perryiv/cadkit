
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all models.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_MODEL_CLASS_H_
#define _APP_FRAME_WORK_MODEL_CLASS_H_

#include "AppFrameWork/Core/Object.h"
#include "AppFrameWork/Core/Icon.h"

#include "Usul/Threads/RecursiveMutex.h"


namespace AFW {
namespace Core {


class APP_FRAME_WORK_EXPORT Model : public Object
{
public:

  // Typedefs.
  typedef Object BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Model );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( Model );

  // Set/get the modified flag.
  virtual void                        modified ( bool );
  virtual bool                        modified() const;

  // Set/get the icon.
  void                                icon ( const Icon & );
  Icon                                icon() const;

protected:

  // Constructor
  Model ( const Icon &icon = Icon() );

  // Use reference counting.
  virtual ~Model();

private:

  // No copying.
  Model ( const Model & );
  Model &operator = ( const Model & );

  unsigned int _flags;
  Icon _icon;
};


} // namespace Core
} // namespace AFW


#endif //_APP_FRAME_WORK_MODEL_CLASS_H_
