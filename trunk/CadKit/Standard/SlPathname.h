
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
# define CADKIT_DEFINE_SL_TEMPLATE_STRING_SPLIT_INTO_LIST_FUNCTION
# include "SlAString.h"
# include "SlWString.h"
#endif

#define SL_PATHNAME_SAFE_BUFFER_SIZE 1024


namespace CadKit
{
template<class S> class SlPathname
{
public:

  SlPathname();
  SlPathname ( const S &pathname );
  SlPathname ( const SlPathname &pathname );

  S                     getDrive()     const { S temp; this->getDrive ( temp );     return temp; }
  S                     getDirectory() const { S temp; this->getDirectory ( temp ); return temp; }
  S                     getFilename()  const { S temp; this->getFilename ( temp );  return temp; }
  S                     getExtension() const { S temp; this->getExtension ( temp ); return temp; }
  S                     getFullpath()  const { S temp; this->getFullpath ( temp );  return temp; }
  const S &             getPathname()  const { return _pathname; }

  void                  getDrive     ( S &drive )     const;
  void                  getDirectory ( S &directory ) const;
  void                  getFilename  ( S &filename )  const;
  void                  getExtension ( S &extension ) const;

  void                  getComponents ( S &drive, S &directory, S &filename, S &extension ) const;
  void                  getFullpath ( S &fullpath ) const;
  void                  getPathname ( S &pathname ) const;

  bool                  isSamePath ( const SlPathname<S> &pathname );

  void                  setPathname ( const S &pathname );
  void                  setPathname ( const SlPathname<S> &pathname );

protected:

  S _pathname;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Function to split a full path name into its components.
//
///////////////////////////////////////////////////////////////////////////////

inline void _splitFullPathName ( const std::string &fullpath, 
                                       std::string &drive, 
                                       std::string &directory, 
                                       std::string &filename, 
                                       std::string &extension )
{
#ifdef _WIN32

  // Temporary storage.
  char tempDrive    [SL_PATHNAME_SAFE_BUFFER_SIZE];
  char tempDirectory[SL_PATHNAME_SAFE_BUFFER_SIZE];
  char tempFilename [SL_PATHNAME_SAFE_BUFFER_SIZE];
  char tempExtension[SL_PATHNAME_SAFE_BUFFER_SIZE];

  // Split the given path.
  ::_splitpath ( fullpath.c_str(), tempDrive, tempDirectory, tempFilename, tempExtension );

  // Lose the "." before the extension.
  char *ext = ( '.' == tempExtension[0] ) ? (char *) (&(tempExtension[1])) : (char *) tempExtension;

  // Copy to the given components.
  drive     = tempDrive;
  directory = tempDirectory;
  filename  = tempFilename;
  extension = ext;

#else

  // Split the path into its components.
  SlAString tempPath ( fullpath );
  std::list<SlAString> components;
  tempPath.split ( '/', components );

  // Split the last component again to get the extension.
  SlAString lastComponent;
  std::list<SlAString> filenameAndExtension;
  lastComponent.split ( '.', filenameAndExtension );

  // Assign the components.
  drive = "";
  //  directory = 


#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Function to split a full path name into its components.
//
///////////////////////////////////////////////////////////////////////////////

inline void _splitFullPathName ( const std::wstring &fullpath, 
                                       std::wstring &drive, 
                                       std::wstring &directory, 
                                       std::wstring &filename, 
                                       std::wstring &extension )
{
  // Temporary storage.
  wchar_t tempDrive    [SL_PATHNAME_SAFE_BUFFER_SIZE];
  wchar_t tempDirectory[SL_PATHNAME_SAFE_BUFFER_SIZE];
  wchar_t tempFilename [SL_PATHNAME_SAFE_BUFFER_SIZE];
  wchar_t tempExtension[SL_PATHNAME_SAFE_BUFFER_SIZE];

  // Split the given path.
  ::_wsplitpath ( fullpath.c_str(), tempDrive, tempDirectory, tempFilename, tempExtension );

  // Lose the "." before the extension.
  wchar_t *ext = ( L'.' == tempExtension[0] ) ? (wchar_t *) (&(tempExtension[1])) : (wchar_t *) tempExtension;

  // Copy to the given components.
  drive     = tempDrive;
  directory = tempDirectory;
  filename  = tempFilename;
  extension = ext;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the full path name from the given relative path.
//
///////////////////////////////////////////////////////////////////////////////

inline void _getFullPathName ( const std::string &pathname, std::string &fullpath )
{
  char temp [SL_PATHNAME_SAFE_BUFFER_SIZE];

  SL_VERIFY ( 0x0 != ::_fullpath ( temp, pathname.c_str(), SL_PATHNAME_SAFE_BUFFER_SIZE ) );

  fullpath = temp;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the full path name from the given relative path.
//
///////////////////////////////////////////////////////////////////////////////

inline void _getFullPathName ( const std::wstring &pathname, std::wstring &fullpath )
{
  wchar_t temp [SL_PATHNAME_SAFE_BUFFER_SIZE];

  SL_VERIFY ( 0x0 != ::_wfullpath ( temp, pathname.c_str(), SL_PATHNAME_SAFE_BUFFER_SIZE ) );

  fullpath = temp;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class S> inline SlPathname<S>::SlPathname() :
  _pathname ( "" )
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
  this->setPathname ( pathname._pathname );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class S> inline void SlPathname<S>::setPathname ( const S &pathname )
{
  _pathname = pathname;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the (unaltered) path name.
//
///////////////////////////////////////////////////////////////////////////////

template<class S> inline void SlPathname<S>::getPathname ( S &pathname ) const
{
  pathname = _pathname;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the full path name.
//
///////////////////////////////////////////////////////////////////////////////

template<class S> inline void SlPathname<S>::getFullpath ( S &fullpath ) const
{
  CadKit::_getFullPathName ( _pathname, fullpath );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the components of the path.
//
///////////////////////////////////////////////////////////////////////////////

template<class S> inline void SlPathname<S>::getComponents ( S &drive, S &directory, S &filename, S &extension ) const
{
  // Convert to the full path name and get the split components.
  S fullpath;
  this->getFullpath ( fullpath );
  CadKit::_splitFullPathName ( fullpath, drive, directory, filename, extension );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the drive letter.
//
///////////////////////////////////////////////////////////////////////////////

template<class S> inline void SlPathname<S>::getDrive ( S &drive ) const
{
  S directory, filename, extension;
  this->getComponents ( drive, directory, filename, extension );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the directory.
//
///////////////////////////////////////////////////////////////////////////////

template<class S> inline void SlPathname<S>::getDirectory ( S &directory ) const
{
  S drive, filename, extension;
  this->getComponents ( drive, directory, filename, extension );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filename.
//
///////////////////////////////////////////////////////////////////////////////

template<class S> inline void SlPathname<S>::getFilename ( S &filename ) const
{
  S drive, directory, extension;
  this->getComponents ( drive, directory, filename, extension );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the extension.
//
///////////////////////////////////////////////////////////////////////////////

template<class S> inline void SlPathname<S>::getExtension ( S &extension ) const
{
  S drive, directory, filename;
  this->getComponents ( drive, directory, filename, extension );
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
  CadKit::_convertStringToUpperCase ( fullpath1 );
  CadKit::_convertStringToUpperCase ( fullpath2 );

#endif

  // Compare the strings.
  return ( fullpath1 == fullpath2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//	Common types.
//
///////////////////////////////////////////////////////////////////////////////

typedef SlPathname<std::string>  SlAPathname;
typedef SlPathname<std::wstring> SlWPathname;


}; // End of namespace CadKit

#endif // _CADKIT_STANDARD_LIBRARY_PATHNAME_CLASS_H_
