
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
#include "Minerva/Core/Animate/Date.h"

#include "Usul/Base/Object.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/IElevationDatabase.h"

#include "OsgTools/ShapeFactory.h"

#include <map>

namespace osg { class Node; }

namespace Minerva {
namespace Core {

  class Visitor;

namespace DataObjects {

class MINERVA_EXPORT DataObject : public Usul::Base::Object
{
public:
  typedef Usul::Base::Object           BaseClass;
  typedef Usul::Interfaces::IUnknown   Unknown;
  typedef Minerva::Core::Animate::Date Date;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( DataObject );

  /// Constructor.
  DataObject();
  
  // Altitude Mode.
  enum AltitudeMode
  {
    CLAMP_TO_GROUND,
    RELATIVE_TO_GROUND,
    ABSOLUTE
  };

  /// Get the shape factory.
  static OsgTools::ShapeFactory* shapeFactory();

  /// Accept the visitor.
  virtual void          accept ( Minerva::Core::Visitor& visitor );
  
  /// Get/Set the altitude mode.
  void                  altitudeMode ( AltitudeMode mode );
  AltitudeMode          altitudeMode () const;

  /// Build the scene branch for the data object.
  virtual osg::Node*    buildScene( Usul::Interfaces::IUnknown* caller = 0x0 );
  void                  preBuildScene( Usul::Interfaces::IUnknown* caller = 0x0 );

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

  /// Get/Set the flag to show the label.
  void                  showLabel ( bool value );
  bool                  showLabel () const;

  /// Get/Set the render bin
  unsigned int          renderBin() const;
  void                  renderBin ( unsigned int );

  /// Get/Set id.
  void                  objectId ( const std::string& );
  const std::string&    objectId() const;

  /// Is the data object transparent?
  bool                  transparent() const;

  /// Get/Set the visibilty flag.
  void                  visibility ( bool b );
  bool                  visibility ( ) const;

  /// Get/Set the data source.
  void                  dataSource( Unknown* );
  Unknown *             dataSource();
  const Unknown *       dataSource() const;

  /// Get/Set the first date.
  const Date&           firstDate() const;
  void                  firstDate ( const Date& );
  
  /// Get/Set the last date.
  const Date&           lastDate() const;
  void                  lastDate( const Date& );
  
  /// Get/Set extrude flag.
  void                  extrude ( bool b );
  bool                  extrude() const;
  
protected:

  /// Use reference counting.
  virtual ~DataObject ();

  osg::Node*            _buildLabel( const osg::Vec3& position );

  virtual osg::Node*    _preBuildScene( Usul::Interfaces::IUnknown* caller = 0x0 ) = 0;
  
  template<class Vertex>
  double                _elevation ( const Vertex& point, Usul::Interfaces::IElevationDatabase* elevation )
  {
    switch ( this->altitudeMode() )
    {
      case CLAMP_TO_GROUND:
        return ( 0x0 != elevation ? elevation->elevationAtLatLong ( point[1], point[0] ) : 0.0 );
      case RELATIVE_TO_GROUND:
        return ( point[2] + ( 0x0 != elevation ? elevation->elevationAtLatLong ( point[1], point[0] ) : 0.0 ) );
      case ABSOLUTE:
        return point[2];
    }
    return 0.0;
  }

private:

  bool         _dirty;
  unsigned int _renderBin;
  osg::Vec4    _color;
  std::string  _objectId;
  std::string  _label;
  osg::Vec3    _labelPosition;
  osg::Vec4    _labelColor;
  float        _labelSize;
  bool         _showLabel;
  Unknown::QueryPtr _geometry;
  Unknown::QueryPtr _dataSource;
  osg::ref_ptr < osg::Node > _root;
  osg::ref_ptr < osg::Node > _preBuiltScene;
  Minerva::Core::Animate::Date _firstDate;
  Minerva::Core::Animate::Date _lastDate;
  AltitudeMode _altitudeMode;
  bool _extrude;

  /// Shape Factory to share across all Data Objects.
  static OsgTools::ShapeFactory::Ptr _sf;
};

}
}
}

#endif // __DATA_OBJECT_H__

