
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Utility class to work with zip files. This should be moved to a more accessable library.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ZIP_FILE_H__
#define __ZIP_FILE_H__

#include <string>
#include <vector>

class ZipFileImpl;


namespace Minerva {
namespace Layers {
namespace Kml {

class ZipFile
{
public:
  typedef std::vector<std::string> Strings;

  ZipFile();
  ~ZipFile();

  /// Get the contents of the zip file.
  void contents ( Strings& containter );

  /// Is the file open?
  bool isOpen() const;

  /// Open the zip file.
  void open ( const std::string& filename );

  /// Read the contents into the string.
  bool readFile ( const std::string& filename, std::string& buffer );

private:

  // Use private implementation.
  ZipFileImpl *_implementation;
};

}
}
}

#endif // __ZIP_FILE_H__
