
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

#include "Usul/File/BinaryInputFile.h"
#include "Usul/Exceptions/Exception.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/LastError.h"

#include <fstream>
#include <limits>

using namespace Usul::File;

USUL_IMPLEMENT_TYPE_ID ( BinaryInputFile );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

BinaryInputFile::BinaryInputFile ( const std::string &name ) : BaseClass ( name, true )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

BinaryInputFile::~BinaryInputFile()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the remainder of the file.
//
///////////////////////////////////////////////////////////////////////////////

void BinaryInputFile::_read ( Buffer &buffer, SizeType size )
{
  // Size the given buffer so that it can hold the file contents.
  buffer.resize ( static_cast < Buffer::size_type > ( size ), '\0' );

  // Read the entire file.
  {
    WriteLock lock ( this );
    this->_getFile().read ( &buffer[0], buffer.size() );
  }
}
