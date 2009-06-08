
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

#include "Usul/File/InputFile.h"


namespace Usul {
namespace File {


class USUL_EXPORT BinaryInputFile : public Usul::File::InputFile
{
public:

  // Useful typedefs.
  typedef Usul::File::InputFile BaseClass;

  // Type information.
  USUL_DECLARE_TYPE_ID ( BinaryInputFile );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( BinaryInputFile );

  // Constructor.
  BinaryInputFile ( const std::string &name );

  // Input operator.
  template < class T > BinaryInputFile &operator >> ( T &v )
  {
    this->_getFile().read ( &v, sizeof ( T ) );
    return *this;
  }

protected:

  // Use reference counting.
  virtual ~BinaryInputFile();

  virtual void                _read ( Buffer &contents, SizeType size );

private:

  // No copying or assignment.
  BinaryInputFile ( const BinaryInputFile & );
  BinaryInputFile &operator = ( const BinaryInputFile & );
};


} // namespace File
} // namespace Usul


#endif // _USUL_FILE_BINARY_INPUT_FILE_CLASS_H_
