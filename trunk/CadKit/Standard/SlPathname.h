
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlPathname.h: A class that represent a pathname.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_PATHNAME_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_PATHNAME_CLASS_H_

#include "SlAssert.h"
#include "SlStringFunctions.h"
#include "SlStringCase.h"
//#include "SlPrint.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <list>
# include <wchar.h>
#endif

#define SL_PATHNAME_SAFE_BUFFER_SIZE 2048

#define SL_PATHNAME_PERIOD         ( static_cast<Char>('.') )
#define SL_PATHNAME_FORWARD_SLASH  ( static_cast<Char>('/') )
#define SL_PATHNAME_BACKWARD_SLASH ( static_cast<Char>('\\') )
#ifdef _WIN32
# define SL_PATHNAME_SLASH SL_PATHNAME_BACKWARD_SLASH
#else
# define SL_PATHNAME_SLASH SL_PATHNAME_FORWARD_SLASH
#endif


namespace CadKit
{
template<class String> class SlPathname
{
public:

  typedef typename String::value_type Char;

  SlPathname();
  SlPathname ( const String &pathname );
  SlPathname ( const SlPathname &pathname );

  const String &        getDrive()     const { return _drive; }
  const String &        getDirectory() const { return _dir; }
  const String &        getFilename()  const { return _file; }
  const String &        getExtension() const { return _ext; }
  String                getFullpath()  const { String temp; this->getFullpath  ( temp ); return temp; }
  String                getPathname()  const { String temp; this->getPathname  ( temp ); return temp; }

  void                  getComponents ( String &drive, String &directory, String &filename, String &extension ) const;
  void                  getDrive      ( String &drive )     const { drive     = this->getDrive(); }
  void                  getDirectory  ( String &directory ) const { directory = this->getDirectory(); }
  void                  getFilename   ( String &filename )  const { filename  = this->getFilename(); }
  void                  getExtension  ( String &extension ) const { extension = this->getExtension(); }
  void                  getFullpath   ( String &fullpath )  const;
  void                  getPathname   ( String &pathname )  const;

  bool                  isSamePath ( const SlPathname<String> &pathname );

  void                  setDrive     ( const String &drive )     { _drive = drive; }
  void                  setDirectory ( const String &directory );
  void                  setFilename  ( const String &filename )  { _file = filename; }
  void                  setExtension ( const String &extension ) { _ext = extension; }
  void                  setPathname  ( const String &pathname );
  void                  setPathname  ( const SlPathname &pathname );

  void                  toFullPath();

protected:

  String _drive;
  String _dir;
  String _file;
  String _ext;

  void                  _addSlash ( String &pathname ) const;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class String> inline SlPathname<String>::SlPathname()
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class String> inline SlPathname<String>::SlPathname ( const String &pathname )
{
  this->setPathname ( pathname );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class String> inline SlPathname<String>::SlPathname ( const SlPathname &pathname )
{
  this->setPathname ( pathname );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class String> inline void SlPathname<String>::setPathname ( const SlPathname<String> &pathname )
{
  _drive = pathname._drive;
  _dir   = pathname._dir;
  _file  = pathname._file;
  _ext   = pathname._ext;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class String> inline void SlPathname<String>::setPathname ( const String &p )
{
  // Convenient typedef.
  typedef typename String::value_type Char;

  // Initialize.
  _drive.erase();
  _dir.erase();
  _file.erase();
  _ext.erase();

  // Handle trivial case.
  if ( p.empty() )
    return;

  // Make a copy.
  String pathname = p;

  // Split the given string at the drive.
  String drive, dir, file, ext, temp1, temp2;
  CadKit::splitAtFirst ( pathname, static_cast<Char>(':'), drive, temp1 );
  //SL_PRINT2 ( "In SlPathname::setPathname(), pathname = %s\n", pathname.c_str() );
  //SL_PRINT2 ( "In SlPathname::setPathname(), drive = %s\n", drive.c_str() );
  //SL_PRINT2 ( "In SlPathname::setPathname(), temp1 = %s\n", temp1.c_str() );

  // See if we have a drive.
  if ( drive.size() )
  {
    // Assign it.
    _drive = drive;

    // Pop the drive and colon off the begining of the string.
    pathname.erase ( pathname.begin() );
    pathname.erase ( pathname.begin() );
  }

  // Now split at the extension.
  CadKit::splitAtLast ( pathname, SL_PATHNAME_PERIOD, temp1, ext );
  //SL_PRINT2 ( "In SlPathname::setPathname(), pathname = %s\n", pathname.c_str() );
  //SL_PRINT2 ( "In SlPathname::setPathname(), temp1 = %s\n", temp1.c_str() );
  //SL_PRINT2 ( "In SlPathname::setPathname(), ext = %s\n", ext.c_str() );

  // See if we have an extension.
  if ( ext.size() )
  {
    // Make sure that the period we just split on is not the right-most period 
    // in a relative pathname. If it is then the first character in the extension 
    // will be a slash.
    if ( SL_PATHNAME_FORWARD_SLASH != ext.at ( 0 ) && SL_PATHNAME_BACKWARD_SLASH != ext.at ( 0 ) )
    {
      // Assign it.
      _ext = ext;

      // Pop the extension and period off.
      for ( unsigned int i = 0; i < ext.size() + 1; ++i )
      {
        pathname.erase ( pathname.end() );
      }
    }
  }

  // When we get to here we should only have a directory and filename.
  // Split at the last slash to get the filename.
  SL_ASSERT ( false == pathname.empty() );
  CadKit::splitAtLast ( pathname, SL_PATHNAME_SLASH, dir, file );
  //SL_PRINT2 ( "In SlPathname::setPathname(), pathname = %s\n", pathname.c_str() );
  //SL_PRINT2 ( "In SlPathname::setPathname(), dir = %s\n", dir.c_str() );
  //SL_PRINT2 ( "In SlPathname::setPathname(), file = %s\n", file.c_str() );

#ifdef _WIN32

  // If we didn't get a split...
  if ( dir.empty() && file.empty() )
  {
    // Try again with a forward slash.
    SL_ASSERT ( false == pathname.empty() );
    CadKit::splitAtLast ( pathname, SL_PATHNAME_FORWARD_SLASH, dir, file );
    //SL_PRINT2 ( "In SlPathname::setPathname(), pathname = %s\n", pathname.c_str() );
    //SL_PRINT2 ( "In SlPathname::setPathname(), dir = %s\n", dir.c_str() );
    //SL_PRINT2 ( "In SlPathname::setPathname(), file = %s\n", file.c_str() );
  }

#endif

  // If we didn't get a split...
  if ( dir.empty() && file.empty() )
  {
    // If we have an extension...
    if ( ext.size() )
    {
      // Then the pathname is the file.
      file = pathname;
    }

    // Otherwise...
    else
    {
      // Assume it is a directory (which it may not be).
      dir = pathname;
    }
  }

  // Otherwise...
  else
  {
    // Make sure the first
  }

  // Set the directory and filename.
  _dir = dir;
  _file = file;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the directory.
//
///////////////////////////////////////////////////////////////////////////////

template<class String> inline void SlPathname<String>::setDirectory ( const String &directory )
{
  // Convenient typedef.
  typedef typename String::value_type Char;
  
  String drive, temp;
  String dirname = directory;

  CadKit::splitAtFirst ( directory, static_cast<Char>(':'), drive, temp );

  // See if we have a drive at the beginning.
  if ( drive.size() )
  {
    // Assign it.
    _drive = drive;

    // Pop the drive and colon off the begining of the string.
    dirname.erase ( dirname.begin() );
    dirname.erase ( dirname.begin() );
  }
  
  SL_ASSERT ( false == dirname.empty() );
  _dir = dirname;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a slash if it isn't there.
//
///////////////////////////////////////////////////////////////////////////////

template<class String> inline void SlPathname<String>::_addSlash ( String &pathname ) const
{
  if ( pathname.empty() )
    return;

  if ( SL_PATHNAME_FORWARD_SLASH == pathname.at ( pathname.size() - 1 ) )
    return;

  if ( SL_PATHNAME_BACKWARD_SLASH == pathname.at ( pathname.size() - 1 ) )
    return;

  pathname += SL_PATHNAME_SLASH;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the (unaltered) path name.
//
///////////////////////////////////////////////////////////////////////////////

template<class String> inline void SlPathname<String>::getPathname ( String &pathname ) const
{
  // Get the components.
  String drive, dir, filename, ext;
  this->getComponents ( drive, dir, filename, ext );

  // Initialize.
  pathname.erase();

  // Append the components to create the pathname.
  if ( drive.size() )
  {
    pathname += drive;
    pathname += static_cast<Char>(':');
  }

  if ( dir.size() )
  {
    pathname += dir;
  }

  if ( filename.size() )
  {
    this->_addSlash ( pathname );
    pathname += filename;
  }

  if ( ext.size() )
  {
    pathname += SL_PATHNAME_PERIOD;
    pathname += ext;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the full path name from the given relative path.
//
///////////////////////////////////////////////////////////////////////////////

inline void _getFullPathName ( const char *pathname, const unsigned int &bufSize, char *fullpath )
{
#ifdef _WIN32
  SL_VERIFY ( NULL != ::_fullpath ( fullpath, pathname, bufSize ) );
#else
  SL_VERIFY ( NULL != ::realpath ( pathname, fullpath ) );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the full path name from the given relative path.
//
///////////////////////////////////////////////////////////////////////////////

inline void _getFullPathName ( const wchar_t *pathname, const unsigned int &bufSize, wchar_t *fullpath )
{
#ifdef _WIN32
  SL_VERIFY ( NULL != ::_wfullpath ( fullpath, pathname, bufSize ) );
#elif __GNUC__
  SL_ASSERT ( 0 ); // Not supported (that I know of).
#elif __sgi
  SL_ASSERT ( 0 ); // Not supported (that I know of).
#else
  TODO
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the full path name.
//
///////////////////////////////////////////////////////////////////////////////

template<class String> inline void SlPathname<String>::getFullpath ( String &fullpath ) const
{
  // Get the current path.
  String current = this->getPathname();

  // Convert to the full path.
  unsigned int bufSize ( SL_PATHNAME_SAFE_BUFFER_SIZE );
  Char temp[bufSize];
  _getFullPathName ( current.c_str(), bufSize, temp );

  // Set the given string.
  fullpath = temp;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the components of the path.
//
///////////////////////////////////////////////////////////////////////////////

template<class String> inline void SlPathname<String>::getComponents ( String &drive, String &directory, String &filename, String &extension ) const
{
  // Get the components.
  this->getDrive ( drive );
  this->getDirectory ( directory );
  this->getFilename ( filename );
  this->getExtension ( extension );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the two paths are really the same thing. Does a case-insensitive 
//  (or case-sensitive if unix) string comparison.
//
///////////////////////////////////////////////////////////////////////////////

template<class String> inline bool SlPathname<String>::isSamePath ( const SlPathname<String> &pathname )
{
  // Get the full paths.
  String fullpath1, fullpath2;
  this->getFullpath ( fullpath1 );
  pathname.getFullpath ( fullpath2 );

#ifdef _WIN32

  // Make them both upper case.
  CadKit::toUpper ( fullpath1 );
  CadKit::toUpper ( fullpath2 );

#endif

  // Compare the strings.
  return ( fullpath1 == fullpath2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert to the full path name.
//
///////////////////////////////////////////////////////////////////////////////

template<class String> inline void SlPathname<String>::toFullPath()
{
  // Get the full path name.
  String full = this->getFullPath();

  // Set this instance.
  this->setPathname ( full );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return just the filename.
//
///////////////////////////////////////////////////////////////////////////////

template<class String> inline String justFilename ( const String &pathname )
{
  SlPathname<String> temp ( pathname );
  return temp.getFilename();
}


}; // End of namespace CadKit


#endif // _CADKIT_STANDARD_LIBRARY_PATHNAME_CLASS_H_
