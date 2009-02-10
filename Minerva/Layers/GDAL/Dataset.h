
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Wrapper around GDALDataset.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MINERVA_GDAL_DATASET_WRAPPER_H_
#define _MINERVA_GDAL_DATASET_WRAPPER_H_

#include "Minerva/Layers/GDAL/Export.h"
#include "Minerva/Core/Extents.h"
#include "Minerva/Interfaces/IElevationData.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include "osg/Vec2d"

#include <vector>

class GDALDataset;

namespace Minerva {
namespace Layers {
namespace GDAL {

class MINERVA_GDAL_EXPORT Dataset : public Usul::Base::Referenced
{
public:

  typedef Usul::Base::Referenced BaseClass;
  typedef Minerva::Interfaces::IElevationData IElevationData;
  typedef Minerva::Interfaces::IElevationData::RefPtr ElevationDataPtr;
  typedef Minerva::Core::Extents<osg::Vec2d>  Extents;
  typedef std::vector<double> GeoTransform;

  USUL_DECLARE_REF_POINTERS ( Dataset );

  static void createGeoTransform ( GeoTransform &transfrom, const Extents& e, unsigned int width, unsigned int height );

  Dataset ( GDALDataset * );
  Dataset ( const std::string& filename, const Extents& extents, unsigned int width, unsigned int height, int bands, unsigned int type );

  /// Close the file.
  void close();

  /// Convert to elevation data.
  ElevationDataPtr convertToElevationData() const;

  /// Get the underlying dataset.
  GDALDataset* dataset() const;

  /// Delete all files of the dataset.
  void deleteFiles();

protected:

  virtual ~Dataset();

private:

  GDALDataset *_data;
};

}
}
}

#endif // _MINERVA_GDAL_DATASET_WRAPPER_H_
