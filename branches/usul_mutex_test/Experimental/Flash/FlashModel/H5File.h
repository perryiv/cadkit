
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __FLASH_H5FILE_H__
#define __FLASH_H5FILE_H__

#include "hdf5.h"

#include <string>

class Dataset;

class H5File
{
public:
  
  /// Construction/Destruction.
  H5File( const std::string& filename );
  ~H5File();
  
  /// Get the handle.  Use with caution.
  hid_t      handle() const;
  
  /// Is the file open?
  bool       isOpen() const;
  
  /// Open dataset.
  Dataset*   openDataset ( const std::string& name );
  
private:

  hid_t _handle;
  std::string _file;
};

#endif
