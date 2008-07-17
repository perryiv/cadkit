
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to contain vector data.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DATA_OBJECT_H__
#define __DATA_OBJECT_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Extents.h"
#include "Minerva/Core/Animate/Date.h"
#include "Minerva/Core/Data/Geometry.h"
#include "Minerva/Interfaces/IElevationChangedListener.h"

#include "Usul/Base/Object.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IBooleanState.h"
#include "Usul/Interfaces/IElevationDatabase.h"
#include "Usul/Interfaces/ILayerExtents.h"
#include "Usul/Interfaces/ITreeNode.h"

#include "osg/Vec3"
#include "osg/Vec4"
#include "osg/Node"

#include <map>

namespace OsgTools { namespace Widgets { class Item; } }

namespace Minerva {
namespace Core {

  class Visitor;

namespace Data {
  
class DataObject;

class MINERVA_EXPORT ClickedCallback : public Usul::Base::Referenced
{
public:
  typedef OsgTools::Widgets::Item Item;
  
  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ClickedCallback );
  
  ClickedCallback();
  virtual ~ClickedCallback();
  
  virtual Item* operator() ( const DataObject&, Usul::Interfaces::IUnknown* ) const = 0;
};

class MINERVA_EXPORT DataObject : public Usul::Base::Object,
                                  public Usul::Interfaces::IBuildScene,
                                  public Usul::Interfaces::ITreeNode,
                                  public Usul::Interfaces::ILayerExtents,
                                  public Usul::Interfaces::IBooleanState,
                                  public Minerva::Interfaces::IElevationChangedListnerer
{
public:
  typedef Usul::Base::Object                  BaseClass;
  typedef Usul::Interfaces::IUnknown          Unknown;
  typedef Minerva::Core::Animate::Date        Date;
  typedef Minerva::Core::Extents<osg::Vec2d>  Extents;
  typedef Minerva::Core::Data::Geometry       Geometry;
  typedef std::vector<Geometry::RefPtr>       Geometries;
  typedef OsgTools::Widgets::Item             Item;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( DataObject );

  // Declare IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Constructor.
  DataObject();

  /// Accept the visitor.
  virtual void          accept ( Minerva::Core::Visitor& visitor );
  
  /// Add a geometry.
  void                  addGeometry ( Geometry* geometry );

  /// DataObject has been clicked.
  Item*                 clicked ( Usul::Interfaces::IUnknown* caller = 0x0 ) const;
  
  /// Set/get the clicked callback.
  void                  clickedCallback ( ClickedCallback* );
  ClickedCallback*      clickedCallback() const;

  /// Build the scene branch for the data object.
  virtual osg::Node*    buildScene( const Options& options = Options(), Usul::Interfaces::IUnknown* caller = 0x0 );
  void                  preBuildScene( Usul::Interfaces::IUnknown* caller = 0x0 );

  /// Get/Set the data source.
  void                  dataSource( Unknown* );
  Unknown *             dataSource();
  const Unknown *       dataSource() const;
  
  /// Set/get the description.
  void                  description ( const std::string& );
  const std::string&    description() const;

  /// Get/Set the dirty flag.
  bool                  dirty() const;
  void                  dirty ( bool );

  /// Elevation has changed within given extents (IElevationChangedListnerer).
  virtual bool          elevationChangedNotify ( const Extents& extents, ImagePtr elevationData, Unknown * caller = 0x0 );

  /// Set/get the extents.
  void                  extents ( const Extents& e );
  Extents               extents() const;

  /// Get/Set the first date.
  const Date&           firstDate() const;
  void                  firstDate ( const Date& );

  /// Get the geometries.
  Geometries            geometries() const;

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

  /// Get/Set the last date.
  const Date&           lastDate() const;
  void                  lastDate( const Date& );

  /// Get the min latitude and min longitude (ILayerExtents).
  virtual double        minLon() const;
  virtual double        minLat() const;
 
  /// Get the max latitude and max longitude (ILayerExtents).
  virtual double        maxLon() const;
  virtual double        maxLat() const;

  /// Get/Set id.
  void                  objectId ( const std::string& );
  const std::string&    objectId() const;

  /// Get/Set the flag to show the label.
  void                  showLabel ( bool value );
  bool                  showLabel() const;

  /// Get/Set the visibilty flag.
  void                  visibility ( bool b );
  bool                  visibility() const;
  
protected:

  /// Use reference counting.
  virtual ~DataObject ();

  osg::Node*            _buildLabel( const osg::Vec3& position );

  // Get the number of children (ITreeNode).
  virtual unsigned int        getNumChildNodes() const;
  
  // Get the child node (ITreeNode).
  virtual ITreeNode *         getChildNode ( unsigned int which );
  
  // Set/get the name (ITreeNode).
  virtual void                setTreeNodeName ( const std::string & );
  virtual std::string         getTreeNodeName() const;
  
  // Set/get the state (IBooleanState).
  virtual void                setBooleanState ( bool );
  virtual bool                getBooleanState() const;

private:

  bool         _dirty;
  bool         _visible;
  std::string  _objectId;
  std::string  _label;
  std::string  _description;
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
  Extents _extents;
  Geometries _geometries;
  ClickedCallback::RefPtr _clickedCallback;
};

}
}
}

#endif // __DATA_OBJECT_H__

