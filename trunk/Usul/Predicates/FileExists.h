
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Predicate class that determines if the file exists.
//  TODO, figure out how to do this without opening/closing the file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_PREDICATES_FILE_EXISTS_H_
#define _USUL_PREDICATES_FILE_EXISTS_H_

#include <string>


namespace Usul {
namespace Predicates {


struct FileExists : public std::unary_function < const std::string &, bool >
{
  struct File
  {
    File ( const std::string &filename ) : _fp ( 0x0 )
    {
      if ( !filename.empty() )
        _fp = ::fopen ( filename.c_str(), "r" );
    }
    ~File()
    {
      if ( _fp )
        ::fclose ( _fp );
    }
    bool exists() const { return 0x0 != _fp; }
  protected:
    File();                 // No default construction.
    File ( const File &f ); // No copy construction.
  private:
    FILE *_fp;
  };
  FileExists()
  {
  }
  bool operator () ( const std::string &s ) const
  {
    File file ( s );
    return file.exists();
  }
};


}; // namespace Predicates
}; // namespace Usul


#endif // _USUL_PREDICATES_FILE_EXISTS_H_
