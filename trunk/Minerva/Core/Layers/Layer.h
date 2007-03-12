
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

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Layer );

  Layer();

  /// Get/Set the color functor. 
  void                                                  colorFunctor( Minerva::Core::Functors::BaseColorFunctor *colorFunctor );
  Minerva::Core::Functors::BaseColorFunctor *           colorFunctor();
  const Minerva::Core::Functors::BaseColorFunctor *     colorFunctor() const;

  /// Get/Set the layer id.
  void                        layerID( unsigned int id );
  unsigned int                layerID() const;

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
  void                        renderBin( unsigned int bin );
  unsigned int                renderBin( ) const;

  /// Get/Set the z offset.
  void                        zOffset( float f );
  float                       zOffset( ) const;

  /// Build the scene.
  virtual void                buildScene( osg::Group* parent );

  /// Get/Set the query to get the geometry.
  void                        query ( const std::string& query );
  const std::string&          query ( ) const;

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

protected:
  virtual ~Layer();

  // The mutex.
  Mutex _mutex;

  void                        _addDataObject ( DataObject *dataObject );
  void                        _clearDataObjects ();
  DataObjects&                _getDataObjects();

  osg::Vec4                   _color( const pqxx::result::const_iterator& iter );

  void                        _labelDataObject ( DataObject* dataObject );


private:
  friend class boost::serialization::access;
  template<class Archive> void save(Archive & ar, const unsigned int version) const;
  template<class Archive> void load(Archive & ar, const unsigned int version);
  BOOST_SERIALIZATION_SPLIT_MEMBER()

  unsigned int _layerID;
  std::string _tablename;
  std::string _labelColumn;
  std::string _query;
  unsigned int _renderBin;
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
};

template<class Archive> void Layer::save(Archive & ar, const unsigned int version) const
{
  ar & boost::serialization::make_nvp ( "LayerID", _layerID );
  ar & boost::serialization::make_nvp ( "TableName", _tablename );
  ar & boost::serialization::make_nvp ( "LabelColumn", _labelColumn );
  ar & boost::serialization::make_nvp ( "Query", _query );
  ar & boost::serialization::make_nvp ( "RenderBin", _renderBin );
  ar & boost::serialization::make_nvp ( "Offset", _zOffset );
  //ar & _dataObjects;
  Minerva::Core::DB::Connection* connection ( _connection.get() );
  ar & boost::serialization::make_nvp ( "Connection", connection );

  Minerva::Core::Functors::BaseColorFunctor *colorFunctor ( _colorFunctor.get() );
  ar & boost::serialization::make_nvp ( "ColorFunctor", colorFunctor  );

  ar & boost::serialization::make_nvp ( "LegendText", _legendText );
  ar & boost::serialization::make_nvp ( "ShowLabel", _showLabel );
  ar & boost::serialization::make_nvp ( "Shown", _shown );
  ar & boost::serialization::make_nvp ( "LabelColor", _labelColor );
  ar & boost::serialization::make_nvp ( "LabelZOffset", _labelZOffset );
  ar & boost::serialization::make_nvp ( "LabelSize", _labelSize );
  ar & boost::serialization::make_nvp ( "ColorColumn", _colorColumn );
}

template<class Archive> void Layer::load(Archive & ar, const unsigned int version)
{
  ar & boost::serialization::make_nvp ( "LayerID", _layerID );
  ar & boost::serialization::make_nvp ( "TableName", _tablename );
  ar & boost::serialization::make_nvp ( "LabelColumn", _labelColumn );
  ar & boost::serialization::make_nvp ( "Query", _query );
  ar & boost::serialization::make_nvp ( "RenderBin", _renderBin );
  ar & boost::serialization::make_nvp ( "Offset", _zOffset );
  //ar & _dataObjects;

  Minerva::Core::DB::Connection* connection ( 0x0 );
  ar & boost::serialization::make_nvp ( "Connection", connection );
  _connection = connection;

  Minerva::Core::Functors::BaseColorFunctor *colorFunctor ( 0x0 );
  ar & boost::serialization::make_nvp ( "ColorFunctor", colorFunctor );
  _colorFunctor = colorFunctor;

  ar & boost::serialization::make_nvp ( "LegendText", _legendText );
  this->legendText( _legendText );

  ar & boost::serialization::make_nvp ( "ShowLabel", _showLabel );
  ar & boost::serialization::make_nvp ( "Shown", _shown );
  ar & boost::serialization::make_nvp ( "LabelColor", _labelColor );
  ar & boost::serialization::make_nvp ( "LabelZOffset", _labelZOffset );
  ar & boost::serialization::make_nvp ( "LabelSize", _labelSize );
  ar & boost::serialization::make_nvp ( "ColorColumn", _colorColumn );
}

#if 0
//template < typename Iter >
inline osg::Vec4 Layer::_color ( const pqxx::result::const_iterator& iter )
{
  osg::Vec4 color( 0.0, 0.0, 0.0, 1.0 );

  try
  {
    if( !this->colorColumn().empty() )
    {
      std::string column ( this->colorColumn() );
      double fieldValue = iter[ column.c_str() ].as < double > ();
      color = (*this->colorFunctor())(fieldValue);
    }
    else
    {
      color = (*this->colorFunctor())( 0.0 );
    }
  }
  catch ( const std::exception& e )
  {
    std::cout << "Error 2909352868: " << e.what() << std::endl;
  }

  return color;
}
#endif

}
}
}

BOOST_IS_ABSTRACT(Minerva::Core::Layers::Layer);

#endif //__MINERVA_LAYERS_LAYER_H__
