
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that represents a dynamically loaded library.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_DYNAMIC_LOAD_LIBRARY_CLASS_H_
#define _USUL_DYNAMIC_LOAD_LIBRARY_CLASS_H_

#include "Usul/Base/Referenced.h"

#include "Usul/Pointers/Pointers.h"

#include <string>


namespace Usul {
namespace DLL {


class USUL_EXPORT Library : public Base::Referenced
{
public:

  USUL_DECLARE_REF_POINTERS ( Library );
  typedef void* Function;
  typedef void* ModuleHandle;

  // Construction.
  Library ( const std::string &filename );

  // Get a pointer to the function.
  Function              function ( const std::string &name ) const;

  // Get the library's filename.
  const std::string &   filename() const { return _filename; }

protected:

  // No default construction or copying.
  Library();
  Library ( const Library & );
  Library &operator = ( const Library & );

  // Only construct on the heap. Use reference counting.
  virtual ~Library();

private:

  ModuleHandle _module;
  std::string _filename;
};


}; // namespace DLL
}; // namespace Usul


#endif // _USUL_DYNAMIC_LOAD_LIBRARY_CLASS_H_
