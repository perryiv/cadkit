
////////////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlPathname.h: A class that represent a pathname.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_PATHNAME_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_PATHNAME_CLASS_H_

#include "SlAssert.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <list>
#endif

#define SL_PATHNAME_SAFE_BUFFER_SIZE 1024

#ifdef _WIN32
# define SL_PATHNAME_SLASH ( static_cast<S::value_type>('\\') )
#else
# define SL_PATHNAME_SLASH ( static_cast<S::value_type>('/') )
#endif


namespace CadKit
{
template<class S> class SlPathname
{
public:

  SlPathname();
  SlPathname ( const S &pathname );
  SlPathname ( const SlPathname &pathname );

  const S &             getDrive()     const { return _drive; }
  const S &             getDirectory() const { return _dir; }
  const S &             getFilename()  const { return _file; }
  const S &             getExtension() const { return _ext; }
  S                     getFullpath()  const { S temp; this->getFullpath  ( temp ); return temp; }
  S                     getPathname()  const { S temp; this->getPathname  ( temp ); return temp; }

  void                  getComponents ( S &drive, S &directory, S &filename, S &extension ) const;
  void                  getDrive      ( S &drive )     const { drive = this->getDrive(); }
  void                  getDirectory  ( S &directory ) const { directory = this->getDirectory(); }
  void                  getFilename   ( S &filename )  const { filename = this->getFilename(); }
  void                  getExtension  ( S &extension ) const { extension = this->getExtension(); }
  void                  getFullpath   ( S &fullpath )  const;
  void                  getPathname   ( S &pathname )  const;

  bool                  isSamePath ( const SlPathname<S> &pathname );

  void                  setPathname ( const S &pathname );
  void                  setPathname ( const SlPathname &pathname );

  void                  toFullPath();

protected:

  typedef std::list<S> Dirs;
  S _drive;
  S _dir;
  S _file;
  S _ext;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class S> inline SlPathname<S>::SlPathname()
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class S> inline SlPathname<S>::SlPathname ( const S &pathname )
{
  this->setPathname ( pathname );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class S> inline SlPathname<S>::SlPathname ( const SlPathname &pathname )
{
  this->setPathname ( pathname );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class S> inline void SlPathname<S>::setPathname ( const SlPathname<S> &pathname )
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

#include "SlPrint.h"

template<class S> inline void SlPathname<S>::setPathname ( const S &p )
{
  // Initialize.
  _drive.clear();
  _dir.clear();
  _file.clear();
  _ext.clear();

  // Handle trivial case.
  if ( p.empty() )
    return;

  // Make a copy.
  S pathname = p;

  // Split the given string at the drive.
  S drive, dir, file, ext, temp1, temp2;
  pathname.splitAtFirst ( static_cast<S::value_type>(':'), drive, temp1 );
  SL_WPRINT2 ( L"In SlPathname::setPathname(), pathname = %s\n", pathname.c_str() );
  SL_WPRINT2 ( L"In SlPathname::setPathname(), drive = %s\n", drive.c_str() );
  SL_WPRINT2 ( L"In SlPathname::setPathname(), temp1 = %s\n", temp1.c_str() );

  // See if we have a drive.
  if ( drive.size() )
  {
    // Assign it.
    _drive = drive;

    // Pop the drive and colin off the begining of the string.
    pathname.popFront();
    pathname.popFront();
  }

  // Now split at the extension.
  pathname.splitAtLast ( static_cast<S::value_type>('.'), temp1, ext );
  SL_WPRINT2 ( L"In SlPathname::setPathname(), pathname = %s\n", pathname.c_str() );
  SL_WPRINT2 ( L"In SlPathname::setPathname(), temp1 = %s\n", temp1.c_str() );
  SL_WPRINT2 ( L"In SlPathname::setPathname(), ext = %s\n", ext.c_str() );

  // See if we have an extension.
  if ( ext.size() )
  {
    // Make sure that the period we just split on is not the right-most slash 
    // in a relative pathname. If it is then the first character in the extension 
    // will be a slash.
    if ( SL_PATHNAME_SLASH != ext.getFirst() )
    {
      // Assign it.
      _ext = ext;

      // Pop the extension and period off.
      for ( unsigned int i = 0; i < ext.size() + 1; ++i )
      {
        pathname.popBack();
      }
    }
  }

  // When we get to here we should only have a directory and filename.
  // Split at the last slash to get the filename.
  pathname.splitAtLast ( SL_PATHNAME_SLASH, dir, file );
  SL_WPRINT2 ( L"In SlPathname::setPathname(), pathname = %s\n", pathname.c_str() );
  SL_WPRINT2 ( L"In SlPathname::setPathname(), dir = %s\n", dir.c_str() );
  SL_WPRINT2 ( L"In SlPathname::setPathname(), file = %s\n", file.c_str() );

  // If we didn't get a split...
  if ( dir.empty() && file.empty() )
  {
    // If we have an extension...
    if ( ext.size() )
    {
      // Then the pathname is the file.
      file = pathname;
    }

    // Otherwise..
    else
    {
      // Assume it is a directory (which it may not be).
      dir = pathname;
    }
  }

  // Set the directory and filename.
  _dir = dir;
  _file = file;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the (unaltered) path name.
//
///////////////////////////////////////////////////////////////////////////////

template<class S> inline void SlPathname<S>::getPathname ( S &pathname ) const
{
  // Get the components.
  S drive, dir, filename, ext;
  this->getComponents ( drive, dir, filename, ext );

  // Initialize.
  pathname.clear();

  // Append the components to create the pathname.
  if ( drive.size() )
  {
    pathname += drive;
    pathname += static_cast<S::value_type>(':');
  }

  if ( dir.size() )
  {
    pathname += dir;
  }

  if ( filename.size() )
  {
    pathname += filename;
  }

  if ( ext.size() )
  {
    pathname += static_cast<S::value_type>('.');
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
  TODO
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
  TODO
#elif _SGI_NATIVE_COMPILER
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

template<class S> inline void SlPathname<S>::getFullpath ( S &fullpath ) const
{
  // Get the current path.
  S current = this->getPathname();

  // Convert to the full path.
  const unsigned int bufSize ( SL_PATHNAME_SAFE_BUFFER_SIZE );
  S::value_type temp[bufSize];
  _getFullPathName ( current.c_str(), bufSize, temp );

  // Set the given string.
  fullpath = temp;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the components of the path.
//
///////////////////////////////////////////////////////////////////////////////

template<class S> inline void SlPathname<S>::getComponents ( S &drive, S &directory, S &filename, S &extension ) const
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

template<class S> inline bool SlPathname<S>::isSamePath ( const SlPathname<S> &pathname )
{
  // Get the full paths.
  S fullpath1, fullpath2;
  this->getFullpath ( fullpath1 );
  pathname.getFullpath ( fullpath2 );

#ifdef _WIN32

  // Make them both upper case.
  fullpath1.toUpper();
  fullpath2.toUpper();

#endif

  // Compare the strings.
  return ( fullpath1 == fullpath2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert to the full path name.
//
///////////////////////////////////////////////////////////////////////////////

template<class S> inline void SlPathname<S>::toFullPath()
{
  // Get the full path name.
  S full = this->getFullPath();

  // Set this instance.
  this->setPathname ( full );
}


}; // End of namespace CadKit


#endif // _CADKIT_STANDARD_LIBRARY_PATHNAME_CLASS_H_
