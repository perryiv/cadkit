
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Removes the file in the destructor.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_SCOPED_FILE_REMOVE_H_
#define _USUL_SCOPED_FILE_REMOVE_H_

#include "Usul/File/Remove.h"


namespace Usul {
namespace Scope {


struct RemoveFile
{
  RemoveFile ( const std::string &file, bool allowThrow = false ) : 
    _file ( file ), 
    _remove ( true ),
    _allowThrow ( allowThrow )
  {
  }

  ~RemoveFile()
  {
    if ( true == _remove )
    {
      Usul::File::remove ( _file, _allowThrow );
    }
  }

  void remove ( bool state )
  {
    _remove = state;
  }

private:

  std::string _file;
  bool _remove;
  bool _allowThrow;
};


} // namespace Scope
} // namespace Usul


#endif // _USUL_SCOPED_FILE_REMOVE_H_
