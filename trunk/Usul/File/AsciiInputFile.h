
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for reading an ascii file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FILE_ASCII_INPUT_FILE_CLASS_H_
#define _USUL_FILE_ASCII_INPUT_FILE_CLASS_H_

#include "Usul/File/InputFile.h"


namespace Usul {
namespace File {


class USUL_EXPORT AsciiInputFile : public Usul::File::InputFile
{
public:

  // Useful typedefs.
  typedef Usul::File::InputFile BaseClass;

  // Type information.
  USUL_DECLARE_TYPE_ID ( AsciiInputFile );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( AsciiInputFile );

  // Constructor.
  AsciiInputFile ( const std::string &name );

  // Input operator.
  template < class T > AsciiInputFile &operator >> ( T &v )
  {
    this->_getFile() >> v;
    return *this;
  }

protected:

  // Use reference counting.
  virtual ~AsciiInputFile();

  virtual void                _read ( Buffer &contents, SizeType size );

private:

  // No copying or assignment.
  AsciiInputFile ( const AsciiInputFile & );
  AsciiInputFile &operator = ( const AsciiInputFile & );
};


} // namespace File
} // namespace Usul


#endif // _USUL_FILE_ASCII_INPUT_FILE_CLASS_H_
