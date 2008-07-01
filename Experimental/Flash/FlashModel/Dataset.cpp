
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Dataset.h"

#ifdef USE_HDF5_LITE
#include "H5LT.h"
#endif

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Dataset::Dataset( hid_t handle, const std::string& name ) : BaseClass(),
#ifdef USE_HDF5_LITE
  _name ( name ),
  _handle ( handle ),
#else
  _dataset ( ::H5Dopen ( handle, name.c_str() ) ),
  _space ( -1 ),
#endif
  _dimensions ()
{
#ifdef USE_HDF5_LITE
  _dimensions.resize ( this->dimensions() );
  herr_t status ( ::H5LTget_dataset_info ( _handle, _name.c_str(), &_dimensions[0], 0x0, 0x0 ) );
  
  if ( status < 0 )
    throw std::runtime_error ( "Error 5786627730: Could not determine the number of dimensions." );
#else
  // Get the "space" of the dataset.
  if ( _dataset >= 0 )
    _space = ::H5Dget_space ( _dataset );
  
  if ( this->isOpen() )
  {
    _dimensions.resize ( this->dimensions() );
    Dimensions max ( this->dimensions() );
    
    herr_t status ( ::H5Sget_simple_extent_dims ( _space, &_dimensions[0], &max[0] ) );
    
    if ( status < 0 )
      throw std::runtime_error ( "Error 2857680810: Could not determine the number of dimensions." );
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Dataset::~Dataset()
{
#ifndef USE_HDF5_LITE
  // Close the space.
  ::H5Sclose ( _space );
  
  // Close the dataset.
  ::H5Dclose ( _dataset );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the data set open?
//
///////////////////////////////////////////////////////////////////////////////

bool Dataset::isOpen() const
{
#ifdef USE_HDF5_LITE
  return true;
#else
  return _dataset >= 0 && _space >= 0;
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of dimensions.
//
///////////////////////////////////////////////////////////////////////////////

int Dataset::dimensions() const
{
#ifdef USE_HDF5_LITE
  int rank ( 0 );
  herr_t status ( ::H5LTget_dataset_ndims ( _handle, _name.c_str(), &rank ) );
                 
  if ( status < 0 )
    throw std::runtime_error ( "Error 1061579875: Could not determine the number of dimensions." );
  
  return rank;
#else
  return ::H5Sget_simple_extent_ndims ( _space );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of elements.
//
///////////////////////////////////////////////////////////////////////////////

hsize_t Dataset::elements() const
{
#ifdef USE_HDF5_LITE
  if ( true == _dimensions.empty() )
    return 0;
  
  hsize_t size ( _dimensions[0] );
  for ( unsigned int i = 1; i < _dimensions.size(); ++i )
    size *= _dimensions[i];
  
  return size;
#else
  return ::H5Sget_simple_extent_npoints ( _space );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read.
//
///////////////////////////////////////////////////////////////////////////////

void Dataset::read ( hid_t dataType, void *buffer )
{
#ifdef USE_HDF5_LITE
  herr_t status ( ::H5LTread_dataset ( _handle, _name.c_str(), dataType, buffer ) );
  
  if ( status < 0 )
    throw std::runtime_error ( "Error 2421191629: Could not read data." );
#else
  // Create the memory space.
  hid_t memspace ( ::H5Screate_simple ( this->dimensions(), &_dimensions[0], 0x0 ) );
  
  // Read the data.
  herr_t status ( ::H5Dread( _dataset, dataType, memspace, _space, H5P_DEFAULT, buffer ) );
  
  ::H5Sclose ( memspace );
  
  if ( status < 0 )
    throw std::runtime_error ( "Error 5554605590: Could not read data." );
#endif
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
#ifdef USE_HDF5_LITE
  // The value to read to.
  double value ( 0.0 );
  
  // Read the value.
  herr_t status ( H5LTget_attribute_double ( _handle, _name.c_str(), name.c_str(), &value ) );
  
  // Check the return status.
  if ( status < 0 )
    throw std::runtime_error ( "Error 2421191629: Could not retrieve value for " + name + "." );
  
  return value;
#else
  hid_t id ( ::H5Aopen_name ( _dataset, name.c_str() ) );
  
  // Make sure we got a valid id.
  if ( id < 0 )
    throw std::runtime_error ( "Error 2824719886: No attribute by name " + name + "." );
  
  // The value to read to.
  double value ( 0.0 );
  
  // Read the value.
  herr_t status ( ::H5Aread ( id, H5T_NATIVE_DOUBLE, &value ) );
  
  ::H5Aclose ( id );
  
  // Check the return status.
  if ( status < 0 )
    throw std::runtime_error ( "Error 1581158300: Could not retrieve value for " + name + "." );
  
  return value;
#endif
}
