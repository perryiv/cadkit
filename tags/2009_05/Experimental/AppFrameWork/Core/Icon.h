
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Icon class. Not thread-safe!
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_ICON_CLASS_H_
#define _APP_FRAME_WORK_ICON_CLASS_H_

#include "AppFrameWork/Core/Export.h"

#include <string>


namespace AFW {
namespace Core {


class APP_FRAME_WORK_EXPORT Icon
{
public:

  // Constructors and destructor.
  Icon ( const std::string &file = "" );
  Icon ( const Icon & );
  ~Icon();

  // Set/get the filename.
  void                                file ( const std::string & );
  std::string                         file() const;

  // Find the appropriate file.
  static std::string                  find ( const std::string &name );

  // Assignment.
  Icon &                              operator = ( const Icon & );
  Icon &                              operator = ( const std::string & );

private:

  std::string _file;
};


} // namespace Core
} // namespace AFW


#endif //_APP_FRAME_WORK_ICON_CLASS_H_
