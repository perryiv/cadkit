
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Dataset.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Dataset::Dataset( hid_t handle, const std::string& name ) : BaseClass(),
  _dataset ( ::H5Dopen ( handle, name.c_str() ) ),
  _space ( -1 ),
  _dimensions ()
{
  // Get the "space" of the dataset.
  if ( _dataset >= 0 )
    _space = ::H5Dget_space ( _dataset );
  
  if ( this->isOpen() )
  {
    _dimensions.resize ( this->dimensions() );
    Dimensions max ( this->dimensions() );
    
    /*herr_t status*/ ( ::H5Sget_simple_extent_dims ( _space, &_dimensions[0], &max[0] ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Dataset::~Dataset()
{
  // Close the space.
  ::H5Sclose ( _space );
  
  // Close the dataset.
  ::H5Dclose ( _dataset );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the data set open?
//
///////////////////////////////////////////////////////////////////////////////

bool Dataset::isOpen() const
{
  return _dataset >= 0 && _space >= 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of dimensions.
//
///////////////////////////////////////////////////////////////////////////////

int Dataset::dimensions() const
{
  return ::H5Sget_simple_extent_ndims ( _space );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of elements.
//
///////////////////////////////////////////////////////////////////////////////

hsize_t Dataset::elements() const
{
  return ::H5Sget_simple_extent_npoints ( _space );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read.
//
///////////////////////////////////////////////////////////////////////////////

void Dataset::read ( hid_t dataType, void *buffer )
{
  // Create the memory space.
  hid_t memspace ( ::H5Screate_simple ( this->dimensions(), &_dimensions[0], 0x0 ) );
  
  // Read the data.
  herr_t status ( ::H5Dread( _dataset, dataType, memspace, _space, H5P_DEFAULT, buffer ) );
  
  ::H5Sclose ( memspace );
  
  if ( status < 0 )
    throw std::runtime_error ( "Error 5554605590: Could not read data." );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get size of each dimension.
//
///////////////////////////////////////////////////////////////////////////////

hsize_t Dataset::size ( unsigned int dim ) const
{
  return _dimensions[dim];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the attribute.
//
///////////////////////////////////////////////////////////////////////////////

double Dataset::attribute ( const std::string& name )
{
  hid_t id ( ::H5Aopen_name ( _dataset, name.c_str() ) );
  
  // Make sure we got a valid id.
  if ( id < 0 )
    throw std::runtime_error ( "Error 2824719886: No attribute by name " + name + "." );
  
  // The value to read to.
  double value ( 0.0 );
  
  // Read the value.
  herr_t status ( ::H5Aread ( id, H5T_NATIVE_DOUBLE, &value ) );
  
  // Check the return status.
  if ( status < 0 )
    throw std::runtime_error ( "Error 1581158300: Could not retrieve value for " + name + "." );
  
  ::H5Aclose ( id );
  
  return value;
}
