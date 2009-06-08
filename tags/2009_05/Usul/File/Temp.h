
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Temporary file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FILE_TEMPORARY_H_
#define _USUL_FILE_TEMPORARY_H_

#include "Usul/Export/Export.h"

#include <string>
#include <iosfwd>


namespace Usul {
namespace File {


class USUL_EXPORT Temp
{
public:

  // Flags for the output stream.
  enum Format { ASCII, BINARY };

  // Constructor. Throws if fails to open temporary file.
  Temp ( Temp::Format f = Temp::ASCII );

  // Destructor. Should never throw.
  ~Temp();

  // Call this if you want to close the file before this instance goes 
  // out of scope, but still have the destructor remove the file.
  void                        close();

  // Return name of directory where temporary files can be created.
  static std::string          directory ( bool wantSlash = false );

  // Return a temporary file name.
  static std::string          file();

  // Return name of internal temporary file.
  const std::string &         name() const;

  // Rename the file. This closes the stream first.
  void                        rename ( const std::string &name );

  // Release ownership of the temporary file.
  void                        release();

  // Remove the file. Provided as a convenience.
  static void                 remove ( const std::string &file, bool allowThrow = false );

  // Access the output stream.
  std::ostream &              stream();

protected:

  // No copying.
  Temp ( const Temp & );
  Temp &operator = ( const Temp & );

private:

  std::string _name;
  std::ostream *_stream;
  bool _remove;
};


} // namespace File
} // namespace Usul


#endif // _USUL_FILE_TEMPORARY_H_
