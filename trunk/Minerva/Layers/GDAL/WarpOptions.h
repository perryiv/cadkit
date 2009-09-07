
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Wrapper around GDALWarpOptions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MINERVA_GDAL_WARP_OPTIONS_WRAPPER_H_
#define _MINERVA_GDAL_WARP_OPTIONS_WRAPPER_H_

#include "gdalwarper.h"

class GDALDataset;

namespace Minerva {
namespace Layers {
namespace GDAL {

class WarpOptions
{
public:

  WarpOptions ( 
    GDALDataset* src, 
    GDALDataset* dst, 
    bool useAlpha,
    double defaultNoDataValue );
  ~WarpOptions();

  operator GDALWarpOptions*() const { return _options; }
  GDALWarpOptions* operator->() { return _options; }

private:

  GDALWarpOptions *_options;
};

}
}
}

#endif // _MINERVA_GDAL_WARP_OPTIONS_WRAPPER_H_
