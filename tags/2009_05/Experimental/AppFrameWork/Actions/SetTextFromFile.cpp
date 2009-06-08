
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Set the window's text from the contents of the given file.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Actions/SetTextFromFile.h"
#include "AppFrameWork/Windows/Window.h"

#include "Usul/Predicates/FileExists.h"
#include "Usul/File/Stats.h"

#include <fstream>
#include <limits>
#include <algorithm>

using namespace AFW::Actions;

USUL_IMPLEMENT_TYPE_ID ( SetTextFromFile );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SetTextFromFile::SetTextFromFile ( const std::string &file ) : BaseClass(),
  _file     ( file ),
  _position ( 0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SetTextFromFile::~SetTextFromFile()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Perform the action.
//
///////////////////////////////////////////////////////////////////////////////

void SetTextFromFile::execute ( AFW::Core::Object *object )
{
  typedef Usul::File::Stats<unsigned int> FileStats;

  Guard guard ( this->mutex() );

  // Get the window.
  AFW::Windows::Window::RefPtr window ( dynamic_cast < AFW::Windows::Window * > ( object ) );
  if ( false == window.valid() )
    return;

  // See if the file is the same as last time...
  const unsigned int size ( FileStats::size ( _file ) );
  if ( size <= _position || 0 == size )
    return;

  // Open the file.
  std::ifstream in ( _file.c_str(), std::ifstream::in | std::ifstream::binary );
  if ( false == in.is_open() )
    return;

  // The maximum we want to read.
  const unsigned int maxRead ( std::numeric_limits<int>::max() );

  // Make sure the position is in range; we don't want to read too much.
  if ( size - _position > maxRead )
    _position = size - maxRead;

  // Move the cursor forward to the proper position.
  in.seekg ( _position );

  // Make a buffer.
  const unsigned int length ( size - _position );
  typedef std::vector < char > Chars;
  Chars buffer ( length + 1, '\0' );

  // Read the last chunk of the file.
  in.read ( &buffer[0], length );

  // Remove any carriage-return values and non-keyboard characters. 
  // Note: Banking on this approach being faster than reading in ascii-mode.
  // TODO, profile!
  Chars::iterator end ( buffer.begin() + length );
  end = std::remove ( buffer.begin(), end, '\r' );

  // Make sure last character is null.
  *end = '\0';

  // Append to this window's text. Have to pass a null-terminated string. 
  // Constructing a string using the vector iterators does not work properly 
  // because of the way the vector was initialized with null characters, 
  // the size of the vector is not the size of the resulting c-string.
  window->textAppend ( &buffer[0], end - buffer.begin() );

  // Scroll to the end.
  window->scrollToEnd();

  // Save these state variables.
  _position = size;
}
