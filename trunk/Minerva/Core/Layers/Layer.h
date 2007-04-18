
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
#include "Usul/Interfaces/ILayer.h"
#include "Usul/Interfaces/IVectorLayer.h"
#include "Usul/Interfaces/IAddRowLegend.h"
#include "Usul/Interfaces/ISerialize.h"
#include "Usul/Interfaces/IClonable.h"

#include "Serialize/XML/Macros.h"

namespace osg { class Group; }
#include "osg/Vec4"

#ifdef _MSC_VER
#  pragma warning ( disable : 4561 )
#endif

#include <string>
#include <vector>
#include <iostream>

namespace Minerva {
namespace Core {
namespace Layers {

class MINERVA_EXPORT Layer : public Usul::Base::Referenced,
                             public Usul::Interfaces::ILayer,
                             public Usul::Interfaces::IVectorLayer,
                             public Usul::Interfaces::IAddRowLegend,
                             public Usul::Interfaces::ISerialize,
                             public Usul::Interfaces::IClonable
{
public:
  /// Typedefs.
  typedef Usul::Base::Referenced                    BaseClass;
  typedef Minerva::Core::DataObjects::DataObject    DataObject;
  typedef DataObject::RefPtr                        DataObjectPtr;
  typedef std::vector< DataObjectPtr >	            DataObjects;
  typedef Usul::Threads::RecursiveMutex             Mutex;
  typedef Usul::Threads::Guard<Mutex>               Guard;
  typedef Minerva::Core::Functors::BaseColorFunctor ColorFunctor;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( Layer );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  Layer();

  /// Get/Set the color functor. 
  void                        colorFunctor( ColorFunctor *colorFunctor );
  ColorFunctor *              colorFunctor();
  const ColorFunctor *        colorFunctor() const;

  /// Get/Set the name.
  void                        name( const std::string& name );
  const std::string&          name() const;

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
  void                        renderBin( Usul::Types::Uint32 bin );
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

  /// Get the guid.
  std::string                 guid() const;

  /// Get/Set show count in legend.
  void                        showCountLegend( bool b );
  bool                        showCountLegend() const;

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

  template < class T >
  osg::Vec4                   _color( const T& iter );

  void                        _labelDataObject ( DataObject* dataObject );

  void                        _registerMembers();

  /// Usul::Interfaces::IVectorLayer
  virtual void                buildVectorData  ( Usul::Interfaces::IUnknown *caller = 0x0 );
  virtual void                modifyVectorData ( Usul::Interfaces::IUnknown *caller = 0x0 );

  /// Usul::Interfaces::IAddRowLegend
  virtual void                             addLegendRow ( OsgTools::Legend::LegendObject* row );

  /// Clone the this layer.
  virtual Usul::Interfaces::IUnknown*      clone() const = 0;

private:

  std::string _guid;
  std::string _name;
  std::string _primaryKeyColumn;
  std::string _tablename;
  std::string _labelColumn;
  std::string _query;
  Usul::Types::Uint32 _renderBin;
  float _zOffset;
  DataObjects _dataObjects;
  DB::Connection::RefPtr _connection;
  Functors::BaseColorFunctor::RefPtr _colorFunctor;
  std::string            _legendText;
  bool                   _showLabel;
  bool                   _shown;
  osg::Vec4              _labelColor;
  float                  _labelZOffset;
  float                  _labelSize;
  std::string            _colorColumn;
  bool                   _customQuery;
  bool                   _showCountLegend;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_CLASS_NAME ( Layer );
  SERIALIZE_XML_SERIALIZE_FUNCTION;
public:
  virtual void deserialize ( const XmlTree::Node &node )
  {
    _dataMemberMap.deserialize ( node );
  }

  SERIALIZE_XML_ADD_MEMBER_FUNCTION;
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

}
}
}


#endif //__MINERVA_LAYERS_LAYER_H__
