
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_FILE_BOOST_H__
#define __USUL_FILE_BOOST_H__

#include "Usul/Adaptors/Boost.h"
#include "Usul/File/Path.h"

#include "boost/filesystem/operations.hpp"

namespace Usul 
{
  namespace File
  {
    ///////////////////////////////////////////////////////////////////////////////
    //
    //  Predicate to test for a directory.
    //
    ///////////////////////////////////////////////////////////////////////////////

    struct IsDirectory
    {
      template < class T >
      bool operator () ( const T& t ) const
      {
        try
        {
          return boost::filesystem::is_directory ( t );
        }
        catch ( ... )
        {
          return false;
        }
      }

      static bool test ( const std::string& s )
      {
        IsDirectory dir;
        return dir ( s );
      }
    };

    ///////////////////////////////////////////////////////////////////////////////
    //
    //  Find all files with the given extension in the given directory..
    //
    ///////////////////////////////////////////////////////////////////////////////

    template < class Container >
    void findFiles ( const boost::filesystem::path & path, const std::string& ext, Container& c )
    {
      typedef boost::filesystem::directory_iterator Iterator;

      Iterator iter ( path );
      Iterator end;
      for( ; iter != end; ++iter )
      {
        const boost::filesystem::path &path = BOOST_FILE_SYSTEM_ITERATOR_TO_PATH ( iter );

        // Make a recursive call if its a directory.
        if ( boost::filesystem::is_directory ( BOOST_FILE_SYSTEM_ITERATOR_TO_STATUS ( iter ) ) )
        {
          findFiles ( path, ext, c );
        }

        // Add it to our list if its a file and the extenstion matches.
        else if ( Usul::File::extension ( path.native_directory_string() ) == ext )
        {
          c.push_back ( path.native_directory_string() );
        }
      }
    }
  }
}

#endif // __USUL_FILE_BOOST_H__
