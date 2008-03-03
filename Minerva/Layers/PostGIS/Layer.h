
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_LAYER_H__
#define __MINERVA_LAYERS_LAYER_H__

#include "Minerva/Layers/PostGIS/Export.h"

#include "Minerva/DataSources/PG/Connection.h"

#include "Minerva/Core/Layers/Vector.h"
#include "Minerva/Core/Functors/BaseColorFunctor.h"
#include "Minerva/Core/DataObjects/DataObject.h"

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Base/Object.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Interfaces/IVectorLayer.h"
#include "Usul/Interfaces/IAddRowLegend.h"
#include "Usul/Interfaces/IClonable.h"
#include "Usul/Math/Vector2.h"

#include "Serialize/XML/Macros.h"

#include "osg/Vec4"

#ifdef _MSC_VER
#  pragma warning ( disable : 4561 )
#endif

#include <string>
#include <vector>
#include <iostream>

namespace Minerva { namespace Core { class Visitor; } }
  
namespace Minerva {
namespace Layers {
namespace PostGIS {

  
class MINERVA_POSTGIS_EXPORT Layer : public Minerva::Core::Layers::Vector,
                                     public Usul::Interfaces::IVectorLayer,
                                     public Usul::Interfaces::IAddRowLegend,
                                     public Usul::Interfaces::IClonable
{
public:
  
  /// Typedefs.
  typedef Minerva::Core::Layers::Vector             BaseClass;
  typedef Minerva::Core::DataObjects::DataObject    DataObject;
  typedef DataObject::RefPtr                        DataObjectPtr;
  typedef Minerva::Core::Functors::BaseColorFunctor ColorFunctor;
  typedef Usul::Interfaces::IUnknown                IUnknown;
  typedef Minerva::DataSources::PG::Connection      Connection;

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
  virtual IUnknown*           clone() const = 0;

  /// Get/Set the color functor. 
  void                        colorFunctor( ColorFunctor *colorFunctor );
  ColorFunctor *              colorFunctor();
  const ColorFunctor *        colorFunctor() const;

  /// Get/Set the connection.
  void                        connection ( Connection *connection );
  Connection*                 connection ();
  const Connection*           connection () const;
  
  /// Get/Set the tablename.
  void                        tablename( const std::string& table );
  const std::string&          tablename() const;

  /// Get/Set the label column
  void                        labelColumn( const std::string& column );
  const std::string&          labelColumn() const;

  /// Get/Set show label
  void                        showLabel( bool b );
  bool                        showLabel() const;

  /// Get/Set the label color.
  void                        labelColor( const osg::Vec4& color );
  const osg::Vec4&            labelColor() const;

  /// Get/Set the label zOffset
  void                        labelZOffset( float offset );
  float                       labelZOffset() const;

  /// Get/Set the label size/
  void                        labelSize( float size );
  float                       labelSize() const;

  /// Get/Set the render bin.
  void                        renderBin( Usul::Types::Uint32 bin );
  Usul::Types::Uint32         renderBin( ) const;

  /// Get/Set the x offset.
  void                        xOffset( float f );
  float                       xOffset( ) const;

  /// Get/Set the y offset.
  void                        yOffset( float f );
  float                       yOffset( ) const;

  /// Get/Set the z offset.
  void                        zOffset( float f );
  float                       zOffset( ) const;

  /// Get/Set the custom query flag.
  void                        customQuery( bool value );
  bool                        customQuery() const;

  /// Get/Set the query to get the geometry.
  void                        query ( const std::string& query );
  const std::string&          query ( ) const;

  /// Get the default query.
  virtual std::string         defaultQuery() const;

  /// Build the data objects.
  virtual void                buildDataObjects( Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress ) = 0;

  /// Modify data objects.
  virtual void                modify( Usul::Interfaces::IUnknown *caller = 0x0 ) = 0;

  /// Is this layer temporal.
  virtual bool                isTemporal() const;

  /// Get/Set the text for the legend.
  void                        legendText( const std::string& text );
  const std::string&          legendText() const;

  /// Get/Set flag to show layer in legend.
  void                        showInLegend ( bool b );
  bool                        showInLegend () const;

  /// Get/Set the color column.
  void                        colorColumn( const std::string& column );
  const std::string&          colorColumn() const;

  /// Get/Set the primary key column.
  void                        primaryKeyColumn( const std::string& );
  const std::string&          primaryKeyColumn() const;

  /// Get the geometry column name.
  std::string                 geometryColumn() const;

  /// Get/Set show count in legend.
  void                        showCountLegend( bool b );
  bool                        showCountLegend() const;

  /// Get/Set show min in legend.
  void                        showMinLegend( bool b );
  bool                        showMinLegend() const;

  /// Get/Set show max in legend.
  void                        showMaxLegend( bool b );
  bool                        showMaxLegend () const;

  /// Get/Set the alpha value.
  void                        alpha ( float a );
  float                       alpha () const;
  
  /// Get the Spatial Reference id (srid).
  int                         srid() const;
  
  /// Get the projection as "Well Known Text".
  std::string                 projectionWKT() const;
  std::string                 projectionWKT( int srid ) const;

  // Update.
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );
  
  /// Set/get the updating state.
  void                        updating( bool b );
  bool                        isUpdating() const;
protected:

  /// Use reference counting.
  virtual ~Layer();

  /// Copy constructor.
  Layer( const Layer& layer );
  
  /// Get the extents.
  virtual void                _calculateExtents ( Usul::Math::Vec2d& lowerLeft, Usul::Math::Vec2d& upperRight ) const;

  template < class T >
  osg::Vec4                   _color( const T& iter );

  void                        _setDataObjectMembers ( DataObject* dataObject, Usul::Interfaces::IUnknown* caller );

  /// Register members for serialization.
  void                        _registerMembers();

  /// Update the min and max.
  void                        _updateMinMax ( double value );

  /// Usul::Interfaces::IVectorLayer
  virtual void                buildVectorData  ( Usul::Interfaces::IUnknown *caller = 0x0, Usul::Interfaces::IUnknown *progress = 0x0 );
  virtual void                modifyVectorData ( Usul::Interfaces::IUnknown *caller = 0x0 );

  /// Usul::Interfaces::IAddRowLegend
  virtual void                addLegendRow ( OsgTools::Legend::LegendObject* row );

private:

  std::string _primaryKeyColumn;
  std::string _tablename;
  std::string _labelColumn;
  std::string _query;
  Usul::Types::Uint32 _renderBin;
  float _xOffset;
  float _yOffset;
  float _zOffset;
  Connection::RefPtr _connection;
  Minerva::Core::Functors::BaseColorFunctor::RefPtr _colorFunctor;
  std::string                  _legendText;
  bool                         _showInLegend;
  bool                         _showLabel;
  osg::Vec4                    _labelColor;
  float                        _labelZOffset;
  float                        _labelSize;
  std::string                  _colorColumn;
  bool                         _customQuery;
  unsigned int                 _legendFlags;
  std::pair < double, double > _minMax;
  float                        _alpha;
  bool                         _updating;

  SERIALIZE_XML_CLASS_NAME ( Layer );
};

///////////////////////////////////////////////////////////////////////////////
//
//  Set the color.
//
///////////////////////////////////////////////////////////////////////////////

template < class T >
osg::Vec4 Layer::_color ( const T& iter )
{
  osg::Vec4 color( 0.0, 0.0, 0.0, 1.0 );

  try
  {
    if( !this->colorColumn().empty() )
    {
      std::string column ( this->colorColumn() );
      double fieldValue = iter[ column.c_str() ].as( static_cast < double > ( 0 ) );
      color = ( *this->colorFunctor() ) ( fieldValue );
    }
    else
    {
      Minerva::Core::Functors::BaseColorFunctor::RefPtr functor ( this->colorFunctor() );
      if ( functor.valid() )
        color = (*functor)( 0.0 );
    }
  }
  catch ( const std::exception& e )
  {
    std::cout << "Error 2909352868: " << e.what() << std::endl;
  }

  color.w() = this->alpha();
  return color;
}

}
}
}


#endif //__MINERVA_LAYERS_LAYER_H__
