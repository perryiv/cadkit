
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __WNV_LAYERS_POINT_TIME_LAYER_H__
#define __WNV_LAYERS_POINT_TIME_LAYER_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Layers/PointLayer.h"
#include "Minerva/Core/Interfaces.h"

#include "OsgTools/Animate/Date.h"

#include "Usul/Interfaces/IPointLayer.h"
#include "Usul/Interfaces/ITemporalData.h"

#include <map>

namespace Minerva {
namespace Core {
namespace Layers {

class MINERVA_EXPORT PointTimeLayer : public Minerva::Core::Layers::PointLayer,
                                      public Usul::Interfaces::ITemporalData,
                                      public Minerva::Core::IPointTimeLayerRawPointer
{
public:
  typedef Minerva::Core::Layers::PointLayer BaseClass;

  USUL_DECLARE_QUERY_POINTERS ( PointTimeLayer );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  PointTimeLayer();

  /// Get/Set First date column name.
  void                    firstDateColumn( const std::string& );
  const std::string&      firstDateColumn() const;

  /// Get/Set last date column name.
  void                    lastDateColumn( const std::string& );
  const std::string&      lastDateColumn() const;

  /// Build the scene.
  virtual void            buildScene( osg::Group* parent );

  /// Build the data objects.
  virtual void            buildDataObjects( Usul::Interfaces::IUnknown *caller = 0x0 );

  /// Modify the data objects.
  virtual void            modify( Usul::Interfaces::IUnknown *caller = 0x0 );

  /// Is this layer temporal.
  virtual bool            isTemporal() const;

  /// Get the default query.
  virtual std::string     defaultQuery() const;

  /// Get/Set the min date.
  void                             minDate( const OsgTools::Animate::Date& date );
  void                             minDate( unsigned int day, unsigned int month, unsigned int year );
  const OsgTools::Animate::Date&   minDate() const;

  /// Get/Set the max date.
  void                             maxDate ( const OsgTools::Animate::Date& date );
  void                             maxDate (unsigned int day, unsigned int month, unsigned int year );
  const OsgTools::Animate::Date&   maxDate() const;

protected:
  virtual ~PointTimeLayer();

  PointTimeLayer( const PointTimeLayer& layer );

  std::string             _whereClause() const;

  void _registerMembers();

  virtual PointTimeLayer*         getRawPointer();
  virtual const PointTimeLayer*   getRawPointer() const;

  /// Clone the this layer.
  virtual Usul::Interfaces::IUnknown*          clone() const;

  void                            _updateMinMaxDate ( const std::string&, const std::string& );

private:

  std::string _firstDateColumn;
  std::string _lastDateColumn;
  
  OsgTools::Animate::Date _minDate;
  OsgTools::Animate::Date _maxDate;

  SERIALIZE_XML_CLASS_NAME ( PointTimeLayer );
};


}
}
}

#endif // __WNV_LAYERS_POINT_LAYER_H__
