
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_POINT_TIME_LAYER_H__
#define __MINERVA_LAYERS_POINT_TIME_LAYER_H__

#include "Minerva/Layers/PostGIS/PointLayer.h"
#include "Minerva/Core/Animate/Date.h"
#include "Minerva/Interfaces/IPointLayer.h"
#include "Minerva/Interfaces/ITemporalData.h"

#include <map>

namespace Minerva {
namespace Layers {
namespace PostGIS {

class MINERVA_POSTGIS_EXPORT PointTimeLayer : public Minerva::Layers::PostGIS::PointLayer,
                                              public Minerva::Interfaces::ITemporalData
{
public:
  typedef Minerva::Layers::PostGIS::PointLayer BaseClass;
  typedef Minerva::Core::Animate::Date      Date;

  USUL_DECLARE_QUERY_POINTERS ( PointTimeLayer );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  PointTimeLayer();

  /// Accept the visitor.
  virtual void            accept ( Minerva::Core::Visitor& visitor );

  /// Get/Set First date column name.
  void                    firstDateColumn( const std::string& );
  const std::string&      firstDateColumn() const;

  /// Get/Set last date column name.
  void                    lastDateColumn( const std::string& );
  const std::string&      lastDateColumn() const;

  /// Build the data objects.
  virtual void            buildDataObjects( Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress );

  /// Modify the data objects.
  virtual void            modify( Usul::Interfaces::IUnknown *caller = 0x0 );

  /// Is this layer temporal.
  virtual bool            isTemporal() const;

  /// Get the default query.
  virtual std::string     defaultQuery() const;

  /// Get/Set the min date.
  void                    minDate( const Date& date );
  void                    minDate( unsigned int day, unsigned int month, unsigned int year );
  const Date&             minDate() const;

  /// Get/Set the max date.
  void                    maxDate ( const Date& date );
  void                    maxDate (unsigned int day, unsigned int month, unsigned int year );
  const Date&             maxDate() const;

protected:
  virtual ~PointTimeLayer();

  PointTimeLayer( const PointTimeLayer& layer );

  std::string             _whereClause() const;

  void _registerMembers();

  /// Clone the this layer.
  virtual Usul::Interfaces::IUnknown*          clone() const;

  void                            _updateMinMaxDate ( const std::string&, const std::string& );

private:

  std::string _firstDateColumn;
  std::string _lastDateColumn;
  
  Date _minDate;
  Date _maxDate;

  SERIALIZE_XML_CLASS_NAME ( PointTimeLayer );
};


}
}
}

#endif // __MINERVA_LAYERS_POINT_TIME_LAYER_H__
