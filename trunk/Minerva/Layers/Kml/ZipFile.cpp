
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "ZipFile.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Scope/Caller.h"

#include "minizip/unzip.h"

using namespace Minerva::Layers::Kml;

///////////////////////////////////////////////////////////////////////////////
//
//  Actual implementation.
//
///////////////////////////////////////////////////////////////////////////////

class ZipFileImpl
{
public:
  ZipFileImpl();
  ~ZipFileImpl();

  void contents ( ZipFile::Strings& container );

  bool isOpen() const;

  void open ( const std::string& filename );

  /// Read the contents into the string.
  bool readFile ( const std::string& filename, std::string& buffer );

private:

  /// Read the current file.
  bool _readFile ( std::string& buffer );

  unzFile _unzfile;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ZipFile::ZipFile() : _implementation ( new ZipFileImpl )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ZipFile::~ZipFile()
{
  delete _implementation;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open a file.
//
///////////////////////////////////////////////////////////////////////////////

void ZipFile::open ( const std::string& filename )
{
  _implementation->open ( filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the contents of the file.
//
///////////////////////////////////////////////////////////////////////////////

void ZipFile::contents ( Strings& container )
{
  _implementation->contents ( container );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the file open?
//
///////////////////////////////////////////////////////////////////////////////

bool ZipFile::isOpen() const
{
  return _implementation->isOpen();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the contents into the string.
//
///////////////////////////////////////////////////////////////////////////////

bool ZipFile::readFile ( const std::string& filename, std::string& buffer )
{
  return _implementation->readFile ( filename, buffer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ZipFileImpl::ZipFileImpl() : 
  _unzfile ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ZipFileImpl::~ZipFileImpl()
{
  if ( this->isOpen() )
    ::unzClose ( _unzfile );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open a file.
//
///////////////////////////////////////////////////////////////////////////////

void ZipFileImpl::open ( const std::string& filename )
{
  _unzfile = ::unzOpen ( filename.c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the contents of the file.
//
///////////////////////////////////////////////////////////////////////////////

void ZipFileImpl::contents ( ZipFile::Strings& container )
{
  // Return now if we are not open.
  if ( false == this->isOpen() )
    return;

  // Go to the first file.
  if ( UNZ_OK == ::unzGoToFirstFile ( _unzfile ) )
  {
    // Size for the buffer.
    const unsigned int size ( 2047 );

    // Buffer to write to.
    char buffer [size + 1];

    // File info.
    unz_file_info finfo;
    
    // Loop through the files.
    do
    {
      // Get the current file name.
      if ( UNZ_OK == ::unzGetCurrentFileInfo ( _unzfile, &finfo, buffer, size, 0x0, 0, 0x0, 0 ) )
      {
        container.push_back ( buffer );
      }
    } while ( UNZ_OK == ::unzGoToNextFile ( _unzfile ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the file open?
//
///////////////////////////////////////////////////////////////////////////////

bool ZipFileImpl::isOpen() const
{
  return 0x0 != _unzfile;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the contents into the string.
//
///////////////////////////////////////////////////////////////////////////////

bool ZipFileImpl::readFile ( const std::string& filename, std::string& buffer )
{
  if ( false == this->isOpen() )
    return false;

  if ( UNZ_OK == ::unzLocateFile ( _unzfile, filename.c_str(), false ) )
  {
    return this->_readFile ( buffer );
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the current file.
//
///////////////////////////////////////////////////////////////////////////////

bool ZipFileImpl::_readFile ( std::string& buffer )
{
  if ( UNZ_OK != ::unzOpenCurrentFile ( _unzfile ) )
    return false;

  // Make sure the file is closed.
  Usul::Scope::Caller::RefPtr closeFile ( Usul::Scope::makeCaller ( Usul::Adaptors::bind1 ( _unzfile, ::unzCloseCurrentFile ) ) );

  // Get the file info.
  unz_file_info finfo;
  if ( UNZ_OK != ::unzGetCurrentFileInfo ( _unzfile, &finfo, 0x0, 0, 0x0, 0, 0x0, 0 ) )
    return false;

  // Get the number of bytes.
  const unsigned int numBytes ( finfo.uncompressed_size );
  std::vector<std::string::value_type> fileData ( numBytes );

  // Read the file data into the buffer.
  if ( numBytes > 0 && numBytes == ::unzReadCurrentFile ( _unzfile, &fileData[0], numBytes ) )
  {
    buffer.assign ( &fileData[0], numBytes );
    return true;
  }
  
  return false;
}
