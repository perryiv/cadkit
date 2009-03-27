
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_POSTGIS_LAYER_H__
#define __MINERVA_LAYERS_POSTGIS_LAYER_H__

#include "Minerva/Layers/PostGIS/Export.h"
#include "Minerva/Layers/PostGIS/LabelData.h"

#include "Minerva/Core/Animate/Date.h"
#include "Minerva/Core/Data/Container.h"
#include "Minerva/Core/Functors/BaseColorFunctor.h"
#include "Minerva/Core/Data/DataObject.h"
#include "Minerva/Core/Data/Geometry.h"

#include "Minerva/DataSources/PG/Connection.h"

#include "Serialize/XML/Macros.h"

#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Interfaces/IAddRowLegend.h"
#include "Usul/Interfaces/IClonable.h"
#include "Usul/Math/Vector2.h"

#include "osg/Vec4"

#ifdef _MSC_VER
#  pragma warning ( disable : 4561 )
#endif

#include <string>
#include <vector>
#include <iostream>

namespace Minerva {
namespace Layers {
namespace PostGIS {

  
class MINERVA_POSTGIS_EXPORT Layer : public Minerva::Core::Data::Container,
                                     public Usul::Interfaces::IAddRowLegend,
                                     public Usul::Interfaces::IClonable
{
public:
  
  /// Typedefs.
  typedef Minerva::Core::Data::Container            BaseClass;
  typedef Minerva::Core::Data::DataObject           DataObject;
  typedef Minerva::Core::Functors::BaseColorFunctor ColorFunctor;
  typedef Usul::Interfaces::IUnknown                IUnknown;
  typedef Minerva::DataSources::PG::Connection      Connection;
  typedef Minerva::Core::Data::Geometry             Geometry;
  typedef Minerva::Core::Animate::Date              Date;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( Layer );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  Layer();

  enum LegendFlags
  {
    COUNT = 0x00000001,
    MIN   = 0x00000002,
    MAX   = 0x00000004
  };

  /// Clone this layer.
  virtual IUnknown*           clone() const;

  /// Get the extents.
  virtual Extents             calculateExtents() const;
  
  /// Get/Set the color functor. 
  void                        colorFunctor( ColorFunctor *colorFunctor );
  ColorFunctor *              colorFunctor();
  const ColorFunctor *        colorFunctor() const;

  /// Get/Set the connection.
  void                        connection ( Connection *connection );
  Connection*                 connection();
  const Connection*           connection() const;
  
  /// Get/Set the custom query flag.
  void                        customQuery ( bool value );
  bool                        customQuery() const;
  
  /// Get/Set First date column name.
  void                        firstDateColumn ( const std::string& );
  const std::string&          firstDateColumn() const;

  /// Get/Set the tablename.
  void                        tablename ( const std::string& table );
  const std::string&          tablename() const;

  /// Get/Set the label column
  void                        labelColumn ( const std::string& column );
  const std::string&          labelColumn() const;

  /// Get/Set show label
  void                        showLabel ( bool b );
  bool                        showLabel() const;

  /// Get/Set the label color.
  void                        labelColor ( const Usul::Math::Vec4f& color );
  Usul::Math::Vec4f           labelColor() const;

  /// Get/Set the label zOffset
  void                        labelZOffset ( float offset );
  float                       labelZOffset() const;

  /// Get/Set the label size/
  void                        labelSize ( float size );
  float                       labelSize() const;
  
  /// Get/Set last date column name.
  void                        lastDateColumn ( const std::string& );
  const std::string&          lastDateColumn() const;
  
  /// Get/Set the min date.
  void                        minDate ( const Date& date );
  void                        minDate ( unsigned int day, unsigned int month, unsigned int year );
  const Date&                 minDate() const;
  
  /// Get/Set the max date.
  void                        maxDate ( const Date& date );
  void                        maxDate ( unsigned int day, unsigned int month, unsigned int year );
  const Date&                 maxDate() const;

  /// Get/Set the render bin.
  void                        renderBin ( Usul::Types::Uint32 bin );
  Usul::Types::Uint32         renderBin() const;

  /// Get/Set the query to get the geometry.
  void                        query ( const std::string& query );
  const std::string&          query() const;

  /// Get the default query.
  virtual std::string         defaultQuery() const;

  /// Get/Set the text for the legend.
  void                        legendText ( const std::string& text );
  const std::string&          legendText() const;

  /// Get/Set flag to show layer in legend.
  void                        showInLegend ( bool b );
  bool                        showInLegend() const;

  /// Get/Set the color column.
  void                        colorColumn ( const std::string& column );
  const std::string&          colorColumn() const;

  /// Get/Set the primary key column.
  void                        primaryKeyColumn ( const std::string& );
  const std::string&          primaryKeyColumn() const;

  /// Get the geometry column name.
  std::string                 geometryColumn() const;

  /// Get/Set show count in legend.
  void                        showCountLegend ( bool b );
  bool                        showCountLegend() const;

  /// Get/Set show min in legend.
  void                        showMinLegend ( bool b );
  bool                        showMinLegend() const;

  /// Get/Set show max in legend.
  void                        showMaxLegend ( bool b );
  bool                        showMaxLegend () const;

  /// Get/Set the alpha value.
  void                        alpha ( float a );
  float                       alpha () const;
  
  /// Serialize.
  virtual void                serialize ( XmlTree::Node &parent ) const;
  
  /// Get the Spatial Reference id (srid).
  int                         srid() const;
  
  /// Get the projection as "Well Known Text".
  std::string                 projectionWKT() const;
  std::string                 projectionWKT ( int srid ) const;

  // Update.
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );
  
  /// Set/get the updating state.
  void                        updating ( bool b );
  bool                        isUpdating() const;
  
  /// Get/Set the x offset.
  void                        xOffset ( double f );
  double                      xOffset() const;
  
  /// Get/Set the y offset.
  void                        yOffset ( double f );
  double                      yOffset() const;
  
  /// Get/Set the z offset.
  void                        zOffset ( double f );
  double                      zOffset() const;
  
protected:

  /// Use reference counting.
  virtual ~Layer();

  /// Copy constructor.
  Layer ( const Layer& layer );

  /// Build the data objects.
  virtual void                _buildDataObjects ( Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress );

  osg::Vec4                   _color ( const Minerva::DataSources::Result &result );

  void                        _setDataObjectMembers ( DataObject* dataObject, Usul::Interfaces::IUnknown* caller );
  virtual void                _setGeometryMembers   ( Geometry* geometry, const Minerva::DataSources::Result &result );

  /// Register members for serialization.
  void                        _registerMembers();

  /// Update the min and max.
  void                        _updateMinMax ( double value );
  void                        _updateMinMaxDate ( const std::string&, const std::string& );
  
  /// Get the where clause.
  std::string                 _whereClause() const;

  void                        buildVectorData  ( Usul::Interfaces::IUnknown *caller = 0x0, Usul::Interfaces::IUnknown *progress = 0x0 );
  void                        modifyVectorData ( Usul::Interfaces::IUnknown *caller = 0x0, Usul::Interfaces::IUnknown *progress = 0x0 );

  /// Usul::Interfaces::IAddRowLegend
  virtual void                addLegendRow ( OsgTools::Widgets::LegendObject* row );

private:

  std::string _primaryKeyColumn;
  std::string _tablename;
  std::string _labelColumn;
  std::string _query;
  Usul::Types::Uint32 _renderBin;
  Usul::Math::Vec3d _offset;
  Connection::RefPtr _connection;
  Minerva::Core::Functors::BaseColorFunctor::RefPtr _colorFunctor;
  std::string                  _legendText;
  bool                         _showInLegend;
  LabelData::RefPtr            _labelData;
  std::string                  _colorColumn;
  bool                         _customQuery;
  unsigned int                 _legendFlags;
  std::pair < double, double > _minMax;
  float                        _alpha;
  bool                         _updating;
  std::string                  _firstDateColumn;
  std::string                  _lastDateColumn;
  Date                         _minDate;
  Date                         _maxDate;

  SERIALIZE_XML_CLASS_NAME ( Layer );
};


}
}
}


#endif //__MINERVA_LAYERS_POSTGIS_LAYER_H__
