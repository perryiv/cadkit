
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for reading a binary file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FILE_BINARY_INPUT_FILE_CLASS_H_
#define _USUL_FILE_BINARY_INPUT_FILE_CLASS_H_

#include "Usul/File/BaseFile.h"

#include <iosfwd>
#include <vector>


namespace Usul {
namespace File {


class USUL_EXPORT BinaryInputFile : public Usul::File::BaseFile
{
public:

  // Useful typedefs.
  typedef Usul::File::BaseFile BaseClass;
  typedef std::vector<char> Buffer;

  // Type information.
  USUL_DECLARE_TYPE_ID ( BinaryInputFile );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( BinaryInputFile );

  // Constructor.
  BinaryInputFile ( const std::string &name );

  // Read the remainder of the file. If the stream position is at 
  // the beginning then it will be the entire file.
  void                        read ( Buffer &contents );
  void                        read ( std::string &contents );

  // Read the entire file.
  static void                 read ( const std::string &file, Buffer &contents, bool throwIfError = true );
  static void                 read ( const std::string &file, std::string &contents, bool throwIfError = true );

protected:

  // Use reference counting.
  virtual ~BinaryInputFile();

private:

  // No copying or assignment.
  BinaryInputFile ( const BinaryInputFile & );
  BinaryInputFile &operator = ( const BinaryInputFile & );

  std::ifstream *_file;
};


} // namespace File
} // namespace Usul


#endif // _USUL_FILE_BINARY_INPUT_FILE_CLASS_H_
