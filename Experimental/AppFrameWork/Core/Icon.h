
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Icon class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_ICON_CLASS_H_
#define _APP_FRAME_WORK_ICON_CLASS_H_

#include "AppFrameWork/Core/Object.h"

#include <string>


namespace AFW {
namespace Core {


class APP_FRAME_WORK_EXPORT Icon : public Object
{
public:

  // Typedefs.
  typedef Object BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Icon );

  // Constructor
  Icon ( const std::string &file = "" );

  // Set/get the filename.
  void                                file ( const std::string & );
  const std::string &                 file() const;

protected:

  // Use reference counting.
  virtual ~Icon();

private:

  // No copying.
  Icon ( const Icon & );
  Icon &operator = ( const Icon & );

  std::string _file;
};


} // namespace Core
} // namespace AFW


#endif //_APP_FRAME_WORK_ICON_CLASS_H_
