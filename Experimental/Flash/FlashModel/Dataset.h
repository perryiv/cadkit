
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __FLASH_DATASET_H__
#define __FLASH_DATASET_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include "hdf5.h"

//#define USE_HDF5_LITE 1

#include <string>
#include <vector>

class Dataset : public Usul::Base::Referenced
{
public:
  typedef Usul::Base::Referenced BaseClass;
  typedef std::vector<hsize_t> Dimensions;
  
  USUL_DECLARE_REF_POINTERS ( Dataset );
  
  Dataset ( hid_t FileHandle, const std::string& name );
  
  // Get the attribute.
  double     attribute ( const std::string& name );
  
  // Get the number of dimensions.
  int        dimensions() const;
  
  // Get the number of elements.
  hsize_t    elements() const;
  
  // Is this data set open?
  bool       isOpen() const;
  
  // Read.
  void       read ( hid_t dataType, void *buf );
  
  // Get size of each dimension.
  hsize_t    size ( unsigned int dim ) const;
  
protected:
  
  virtual ~Dataset();
  
private:
  
#ifdef USE_HDF5_LITE
  std::string _name;
  hid_t _handle;
#else
  hid_t _dataset;
  hid_t _space;
#endif
  Dimensions _dimensions;
};

#endif
