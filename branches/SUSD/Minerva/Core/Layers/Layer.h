
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

#include "Minerva/Core/Export.h"

#include "Minerva/Core/Functors/BaseColorFunctor.h"
#include "Minerva/Core/DataObjects/DataObject.h"
#include "Minerva/Core/DB/Connection.h"

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include "Serialize/XML/Macros.h"

#include "OsgTools/Legend/LegendObject.h"

namespace osg { class Group; }
#include "osg/Vec4"

#ifdef _MSC_VER
#  pragma warning ( disable : 4561 )
#endif

#include "boost/serialization/split_member.hpp"
#include "boost/serialization/is_abstract.hpp"

namespace boost { namespace serialization { class access; } }

#include <string>
#include <vector>

namespace Minerva {
namespace Core {
namespace Layers {

class MINERVA_EXPORT Layer : public Usul::Base::Referenced
{
public:
  /// Typedefs.
  typedef Usul::Base::Referenced                    BaseClass;
  typedef Minerva::Core::DataObjects::DataObject    DataObject;
  typedef DataObject::RefPtr                        DataObjectPtr;
  typedef std::vector< DataObjectPtr >	            DataObjects;
  typedef Usul::Threads::RecursiveMutex             Mutex;
  typedef Usul::Threads::Guard<Mutex>               Guard;
  typedef OsgTools::Legend::LegendObject            LegendObject; 
  typedef Minerva::Core::Functors::BaseColorFunctor ColorFunctor;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Layer );

  Layer();

  /// Clone the this layer.
  virtual Layer*              clone() const = 0;

  /// Get/Set the color functor. 
  void                        colorFunctor( ColorFunctor *colorFunctor );
  ColorFunctor *              colorFunctor();
  const ColorFunctor *        colorFunctor() const;

  /// Get/Set the name.
  void                        name( const std::string& name );
  const std::string&          name() const;

  /// Get/Set the layer id.
  void                        layerID( Usul::Types::Uint32 id );
  Usul::Types::Uint32         layerID() const;

  /// Get/Set the connection.
  void                        connection ( DB::Connection *connection );
  DB::Connection*             connection ();

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
  void                        renderBin(Usul::Types::Uint32 bin );
  Usul::Types::Uint32         renderBin( ) const;

  /// Get/Set the z offset.
  void                        zOffset( float f );
  float                       zOffset( ) const;

  /// Build the scene.
  virtual void                buildScene( osg::Group* parent );

  /// Get/Set the custom query flag.
  void                        customQuery( bool value );
  bool                        customQuery() const;

  /// Get/Set the query to get the geometry.
  void                        query ( const std::string& query );
  const std::string&          query ( ) const;

  /// Get the default query.
  virtual std::string         defaultQuery() const;

  /// Build the data objects.
  virtual void                buildDataObjects( Usul::Interfaces::IUnknown *caller = 0x0 ) = 0;

  /// Modify data objects
  virtual void                modify( Usul::Interfaces::IUnknown *caller = 0x0 ) = 0;

  /// Is this layer temporal.
  virtual bool                isTemporal() const;

  /// Set data members from given layer.
  virtual void                setDataMembers ( Layer * );

  /// Get the legend object.
  virtual LegendObject*       legendObject();

  /// Get/Set the text for the legend.
  void                        legendText( const std::string& text );
  const std::string&          legendText() const;

  /// Get/Set show layer
  void                        showLayer( bool b );
  bool                        showLayer() const;

  /// Get/Set the color column.
  void                        colorColumn( const std::string& column );
  const std::string&          colorColumn() const;

  /// Get the number of data objects in this layer.
  virtual unsigned int        number() const;

  /// Get/Set the primary key column.
  void                        primaryKeyColumn( const std::string& );
  const std::string&          primaryKeyColumn() const;

  /// Get the geometry column name.
  std::string                 geometryColumn() const;

protected:

  /// Use reference counting.
  virtual ~Layer();

  /// Copy constructor.
  Layer( const Layer& layer );

  // The mutex.
  Mutex _mutex;

  void                        _addDataObject ( DataObject *dataObject );
  void                        _clearDataObjects ();
  DataObjects&                _getDataObjects();

  osg::Vec4                   _color( const pqxx::result::const_iterator& iter );

  void                        _labelDataObject ( DataObject* dataObject );

  void                        _registerMembers();

private:
  friend class boost::serialization::access;
  template<class Archive> void save(Archive & ar, const unsigned int version) const;
  template<class Archive> void load(Archive & ar, const unsigned int version);
  BOOST_SERIALIZATION_SPLIT_MEMBER();

  static unsigned int _currentLayerID;
  std::string _name;
  Usul::Types::Uint32 _layerID;
  std::string _primaryKeyColumn;
  std::string _tablename;
  std::string _labelColumn;
  std::string _query;
  Usul::Types::Uint32 _renderBin;
  float _zOffset;
  DataObjects _dataObjects;
  DB::Connection::RefPtr _connection;
  Functors::BaseColorFunctor::RefPtr _colorFunctor;
  LegendObject::RefPtr   _legendObject;
  std::string            _legendText;
  bool                   _showLabel;
  bool                   _shown;
  osg::Vec4              _labelColor;
  float                  _labelZOffset;
  float                  _labelSize;
  std::string            _colorColumn;
  bool                   _customQuery;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS ( Layer );
};

template<class Archive> void Layer::save(Archive & ar, const unsigned int version) const
{
  ar & boost::serialization::make_nvp ( "Name", _name );
  ar & boost::serialization::make_nvp ( "LayerID", _layerID );
  ar & boost::serialization::make_nvp ( "TableName", _tablename );
  ar & boost::serialization::make_nvp ( "LabelColumn", _labelColumn );
  ar & boost::serialization::make_nvp ( "Query", _query );
  ar & boost::serialization::make_nvp ( "RenderBin", _renderBin );
  ar & boost::serialization::make_nvp ( "Offset", _zOffset );
  //ar & _dataObjects;
  ar & boost::serialization::make_nvp ( "Connection", _connection );
  ar & boost::serialization::make_nvp ( "ColorFunctor", _colorFunctor  );

  ar & boost::serialization::make_nvp ( "LegendText", _legendText );
  ar & boost::serialization::make_nvp ( "ShowLabel", _showLabel );
  ar & boost::serialization::make_nvp ( "Shown", _shown );
  ar & boost::serialization::make_nvp ( "LabelColor", _labelColor );
  ar & boost::serialization::make_nvp ( "LabelZOffset", _labelZOffset );
  ar & boost::serialization::make_nvp ( "LabelSize", _labelSize );
  ar & boost::serialization::make_nvp ( "ColorColumn", _colorColumn );
  ar & boost::serialization::make_nvp ( "CustomQuery", _customQuery );
}

template<class Archive> void Layer::load(Archive & ar, const unsigned int version)
{
  ar & boost::serialization::make_nvp ( "Name", _name );
  ar & boost::serialization::make_nvp ( "LayerID", _layerID );
  ar & boost::serialization::make_nvp ( "TableName", _tablename );
  ar & boost::serialization::make_nvp ( "LabelColumn", _labelColumn );
  ar & boost::serialization::make_nvp ( "Query", _query );
  ar & boost::serialization::make_nvp ( "RenderBin", _renderBin );
  ar & boost::serialization::make_nvp ( "Offset", _zOffset );
  //ar & _dataObjects;
  ar & boost::serialization::make_nvp ( "Connection", _connection );
  ar & boost::serialization::make_nvp ( "ColorFunctor", _colorFunctor );

  ar & boost::serialization::make_nvp ( "LegendText", _legendText );
  this->legendText( _legendText );

  ar & boost::serialization::make_nvp ( "ShowLabel", _showLabel );
  ar & boost::serialization::make_nvp ( "Shown", _shown );
  ar & boost::serialization::make_nvp ( "LabelColor", _labelColor );
  ar & boost::serialization::make_nvp ( "LabelZOffset", _labelZOffset );
  ar & boost::serialization::make_nvp ( "LabelSize", _labelSize );
  ar & boost::serialization::make_nvp ( "ColorColumn", _colorColumn );
  ar & boost::serialization::make_nvp ( "CustomQuery", _customQuery );
}


}
}
}

BOOST_IS_ABSTRACT(Minerva::Core::Layers::Layer);

#endif //__MINERVA_LAYERS_LAYER_H__
