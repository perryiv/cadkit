
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for input files.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FILE_BASE_INPUT_FILE_CLASS_H_
#define _USUL_FILE_BASE_INPUT_FILE_CLASS_H_

#include "Usul/File/BaseFile.h"
#include "Usul/Strings/Format.h"
#include "Usul/Types/Types.h"

#include <iosfwd>
#include <iterator>
#include <stdexcept>
#include <vector>


namespace Usul {
namespace File {


///////////////////////////////////////////////////////////////////////////////
//
//  Base class for input files.
//
///////////////////////////////////////////////////////////////////////////////

class USUL_EXPORT InputFile : public Usul::File::BaseFile
{
public:

  // Useful typedefs.
  typedef Usul::File::BaseFile BaseClass;
  typedef std::vector<char> Buffer;

  // Type information.
  USUL_DECLARE_TYPE_ID ( InputFile );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( InputFile );

  // Is the stream position at the end of the file?
  bool                        endOfFile() const;

  // Read the remainder of the file. If the stream position is at 
  // the beginning then it will be the entire file.
  void                        read ( Buffer &contents );
  void                        read ( std::string &contents );

protected:

  // Constructor.
  InputFile ( const std::string &name, bool binary );

  // Use reference counting.
  virtual ~InputFile();

  virtual void                _read ( Buffer &contents, SizeType size ){}

  std::ifstream &             _getFile() const;

private:

  // No copying or assignment.
  InputFile ( const InputFile & );
  InputFile &operator = ( const InputFile & );

  std::ifstream *_file;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Interator class for input files.
//
///////////////////////////////////////////////////////////////////////////////

template < class InputFileType, class T > struct InputFileIterator : public std::iterator < std::input_iterator_tag, T, Usul::Types::Int64 >
{
  typedef std::iterator < std::input_iterator_tag, T, Usul::Types::Int64 > BaseClass;
  typedef typename BaseClass::value_type value_type;
  typedef value_type ValueType;
  typedef typename BaseClass::iterator_category iterator_category;
  typedef typename InputFileType::RefPtr InputFilePtr;

private:

  void _getNext()
  {
    // We're already at the end if the file is invalid.
    if ( false == _file.valid() )
    {
      throw std::range_error ( "Error 1392715233: Iterator is already at end" );
    }

    // Should never happen.
    if ( true == _file->endOfFile() )
    {
      throw std::runtime_error ( Usul::Strings::format
        ( "Error 3134083723: Internal error in iterator for file: ", _file->name() ) );
    }

    // Read the next value.
    *_file >> _value;

    // See if we're at the end.
    if ( true == _file->endOfFile() )
    {
      _file = 0x0;
    }
  }

public:

  InputFileIterator ( InputFilePtr file ) : _file ( file ), _value()
  {
    // If the file is valid...
    if ( true == _file.valid() )
    {
      // Handle valid but empty files.
      if ( ( 0 == _file->size() ) || ( true == _file->endOfFile() ) )
      {
        _file = 0x0;
      }

      // Otherwise, get the first value.
      else
      {
        this->_getNext();
      }
    }
  }

  InputFileIterator ( const InputFileIterator &itr ) : _file ( itr._file ), _value ( itr._value )
  {
  }

  ~InputFileIterator()
  {
  }

  InputFileIterator &operator = ( const InputFileIterator &itr )
  {
    _file = itr._file;
    _value = itr._value;
    return *this;
  }

  // Pre-increment means grabbing the next value and returning self.
  InputFileIterator &operator ++ ()
  {
    this->_getNext();
    return *this;
  }

  // Post-increment saves a copy of self before grabbing the next value.
  InputFileIterator &operator ++ ( int )
  {
    InputFileIterator copy ( *this );
    this->_getNext();
    return copy;
  }

  bool equal ( const InputFileIterator &rhs )
  {
    const bool thisValid ( _file.valid() );
    const bool rhsValid ( rhs._file.valid() );

    // If just one is valid then they are not equal.
    if ( thisValid != rhsValid )
    {
      return false;
    }

    // When we get to here if this object is invalid then they both are,
    // and therefore they are the same.
    if ( false == thisValid )
    {
      return true;
    }

    // When we get to here we know that they are both valid so 
    // they better point to the same file.
    if ( rhs._file.get() != _file.get() )
    {
      throw std::invalid_argument ( Usul::Strings::format (
        "Error 3305885381: Comparing iterators for different files",
        ". Left: ", ( ( true == _file.valid() ) ? _file->name() : "" ),
        ", Right: ", ( ( true == rhs._file.valid() ) ? rhs._file->name() : "" ) ) );
    }

    // If we get to here then the iterators are equal if their values are the same.
    const bool same ( rhs._value == _value );
    return same;
  }

  bool operator == ( const InputFileIterator &rhs )
  {
    return this->equal ( rhs );
  }

  bool operator != ( const InputFileIterator &rhs )
  {
    return !this->equal ( rhs );
  }

  ValueType & operator * ()
  {
    return _value;
  }

private:

  InputFilePtr _file;
  ValueType _value;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function for reading the entire file.
//
///////////////////////////////////////////////////////////////////////////////

template < class FileType > void read ( const std::string &file, InputFile::Buffer &buffer, bool throwIfError )
{
  typedef typename FileType::RefPtr FilePtr;

  // Initialize.
  buffer.clear();

  try
  {
    FilePtr file ( new FileType ( file ) );
    file->read ( buffer );
  }
  catch ( ... )
  {
    if ( true == throwIfError )
    {
      throw;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function for reading the entire file.
//
///////////////////////////////////////////////////////////////////////////////

template < class FileType > void read ( const std::string &file, std::string &contents, bool throwIfError )
{
  // Initialize.
  contents.clear();

  // Make the buffer.
  InputFile::Buffer buffer;
  FileType::read ( file, buffer, throwIfError );

  // Copy to the string if there is content.
  if ( false == buffer.empty() )
  {
    contents.reserve ( buffer.size() );
    contents.assign ( buffer.begin(), buffer.end() );
  }
}


} // namespace File
} // namespace Usul


#endif // _USUL_FILE_BASE_INPUT_FILE_CLASS_H_
