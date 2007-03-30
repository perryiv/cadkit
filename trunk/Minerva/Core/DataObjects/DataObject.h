
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all data objects.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DATA_OBJECT_H__
#define __DATA_OBJECT_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/DB/Connection.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Interfaces/IUnknown.h"

#include "OsgTools/ShapeFactory.h"

#include <map>

namespace osg { class Node; }

namespace Minerva {
namespace Core {
namespace DataObjects {

class MINERVA_EXPORT DataObject : public Usul::Base::Referenced
{
public:
  typedef Usul::Base::Referenced      BaseClass;
  typedef Usul::Interfaces::IUnknown  Unknown;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( DataObject );

  /// Constructor.
  DataObject();

  /// Get the shape factory.
  static OsgTools::ShapeFactory* shapeFactory();

  /// Build the scene branch for the data object.
  virtual osg::Node*    buildScene() = 0;

  /// Get/Set the color.
  const osg::Vec4&      color () const;
  void                  color ( const osg::Vec4& );

  /// Get/Set the dirty flag.
  bool                  dirty() const;
  void                  dirty ( bool );

  /// Get/Set the geometry.
  void                  geometry( Unknown *geometry );
  Unknown*              geometry();
  const Unknown*        geometry() const;

  /// Get/Set the label
  void                  label ( const std::string& label );
  const std::string&    label () const;

  /// Get/Set the label position.
  void                  labelPosition ( const osg::Vec3& position );
  const osg::Vec3&      labelPosition () const;

  /// Get/Set the label color.
  void                  labelColor ( const osg::Vec4& color );
  const osg::Vec4&      labelColor () const;

  /// Get/Set the label size.
  void                  labelSize ( float size );
  float                 labelSize () const;

  /// Get/Set the render bin
  unsigned int          renderBin() const;
  void                  renderBin ( unsigned int );

  /// Get/Set table name
  void                  tableName ( const std::string& name );
  const std::string &   tableName () const;

  /// Get/Set row id.
  void                  rowId( int id );
  int                   rowId() const;

  /// Get/Set the database connection.
  void                                  connection( Minerva::Core::DB::Connection* );
  Minerva::Core::DB::Connection *       connection();
  const Minerva::Core::DB::Connection * connection() const;

protected:

  /// Use reference counting.
  virtual ~DataObject ();

private:

  bool _dirty;
  unsigned int _renderBin;
  osg::Vec4 _color;
  std::string _tableName;
  int _rowId;
  std::string _label;
  osg::Vec3 _labelPosition;
  osg::Vec4 _labelColor;
  float _labelSize;
  Unknown::QueryPtr _geometry;
  Minerva::Core::DB::Connection::RefPtr _connection;

  /// Shape Factory to share across all Data Objects.
  static OsgTools::ShapeFactory::Ptr _sf;
};

}
}
}

#endif // __DATA_OBJECT_H__

