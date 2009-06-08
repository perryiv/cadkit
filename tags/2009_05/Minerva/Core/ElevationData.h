
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that contains elevation data.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_ELEVATION_DATA_H__
#define __MINERVA_CORE_ELEVATION_DATA_H__

#include "Minerva/Core/Export.h"

#include "Minerva/Interfaces/IElevationData.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Pointers/Pointers.h"

#include <vector>

namespace Minerva {
namespace Core {


class MINERVA_EXPORT ElevationData : public Usul::Base::Referenced,
                                     public Minerva::Interfaces::IElevationData
{
public:

  typedef Usul::Base::Referenced BaseClass;

  USUL_DECLARE_REF_POINTERS ( ElevationData );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  ElevationData ( SizeType width, SizeType height );
  ElevationData ( SizeType width, SizeType height, const std::vector<ValueType>& data );

  /// Set/get the value at a given row and column.
  virtual void      value ( SizeType row, SizeType column, ValueType value );
  virtual ValueType value ( SizeType row, SizeType column ) const;

  /// Set/get the no data value.
  void              noDataValue ( ValueType noData );
  virtual ValueType noDataValue() const;

  /// Get the size.
  virtual SizeType height() const;
  virtual SizeType width() const;

protected:

  virtual ~ElevationData();

private:

  // Get the index for the row and column.
  SizeType _index ( SizeType row, SizeType column ) const;

  ElevationData();
  ElevationData ( const ElevationData& );
  ElevationData& operator= ( const ElevationData& );

  SizeType _width;
  SizeType _height;
  std::vector<ValueType> _data;
  ValueType _noDataValue;
};


}
}

#endif // __MINERVA_CORE_ELEVATION_DATA_H__
