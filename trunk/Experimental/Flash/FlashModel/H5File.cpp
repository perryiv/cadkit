
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "H5File.h"
#include "Dataset.h"

#include "Usul/Strings/Format.h"

#include <iostream>


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

H5File::H5File ( const std::string& file ) : 
  _handle ( -1 ),
  _file ( file )
{
  // Open the file.
  _handle = ::H5Fopen ( file.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

H5File::~H5File()
{
  // Get the number of open objects.  This includes the file.
  const int numOpenObjects ( ::H5Fget_obj_count ( _handle, H5F_OBJ_ALL ) );
  if ( 1 < numOpenObjects )
  {
    const std::string message
      ( Usul::Strings::format ( "Warning 1403446461: Closing HDF5 file '", 
                                _file, "' while ", numOpenObjects, 
                                " of its objects are still open" ) );
    std::cout << message << std::endl;
    
    // Get the object id's.
    std::vector<hid_t> ids ( numOpenObjects );
    ::H5Fget_obj_ids ( _handle, H5F_OBJ_ALL, numOpenObjects, &ids[0] );
    
    std::cout << "Ids open for file " << _handle << ":" << std::endl;
    for ( unsigned int i = 0; i < ids.size(); ++i )
      std::cout << "Id: " << ids[i] << std::endl;
  }
  
  // Flush the file.
  ::H5Fflush ( _handle, H5F_SCOPE_GLOBAL );

  // Close the file.
  ::H5Fclose ( _handle );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the file open?
//
///////////////////////////////////////////////////////////////////////////////

bool H5File::isOpen() const
{
  return _handle >= 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the handle.  Use with caution.
//
///////////////////////////////////////////////////////////////////////////////

hid_t H5File::handle() const
{
  return _handle;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open dataset.
//
///////////////////////////////////////////////////////////////////////////////

Dataset* H5File::openDataset ( const std::string& name )
{
  return new Dataset ( _handle, name );
}
