
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for modflow files.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_DOCUMENT_H_
#define _MODFLOW_MODEL_DOCUMENT_H_

#include "ModflowModel/CompileGuard.h"
#include "ModflowModel/Builders/Geometry.h"
#include "ModflowModel/Interfaces/IModflowModel.h"
#include "ModflowModel/Model/Layer.h"

#include "XmlTree/Document.h"

#include "OsgTools/Configure/OSG.h"

#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IBooleanState.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/ICoordinateTransform.h"
#include "Usul/Interfaces/IDirtyState.h"
#include "Usul/Interfaces/IGetBoundingBox.h"
#include "Usul/Interfaces/ILayer.h"
#include "Usul/Interfaces/IIntersectListener.h"
#include "Usul/Interfaces/ITimeVaryingData.h"
#include "Usul/Interfaces/ITreeNode.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector4.h"
#include "Usul/Pointers/WeakPointer.h"
#include "Usul/Types/Types.h"

#include "osg/BoundingBox"

#include <iosfwd>
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace osg { class Node; class Group; class MatrixTransform; }
namespace Usul { namespace Factory { template<class T> class BaseFactory; } }
namespace XmlTree { class Node; }
namespace Modflow { namespace Readers { class BaseReader; } }
class OGRSpatialReference; class OGRCoordinateTransformation;

namespace Modflow {


class ModflowDocument : public Usul::Documents::Document,
                        public Usul::Interfaces::IBuildScene,
                        public Usul::Interfaces::IDirtyState,
                        public Usul::Interfaces::IUpdateListener,
                        public Usul::Interfaces::IIntersectListener,
                        public Usul::Interfaces::ITreeNode,
                        public Usul::Interfaces::IBooleanState,
                        public Modflow::Interfaces::IModflowModel,
                        public Usul::Interfaces::ITimeVaryingData,
                        public Usul::Interfaces::IGetBoundingBox,
                        public Usul::Interfaces::ICoordinateTransformOSG,
                        public Usul::Interfaces::ILayer
{
public:

  // Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef Usul::Math::Vec2ui Vec2ui;
  typedef Usul::Math::Vec2d Vec2d;
  typedef Usul::Math::Vec3f Vec3f;
  typedef Usul::Math::Vec3d Vec3d;
  typedef std::vector < double > GridData;
  typedef std::pair < std::string, GridData > GridInfo;
  typedef std::vector < GridInfo > GridVector;
  typedef Modflow::Model::Layer Layer;
  typedef std::vector < Layer::RefPtr > Layers;
  typedef Usul::Factory::BaseFactory<Usul::Base::Referenced> ObjectFactory;
  typedef Modflow::Readers::BaseReader BaseReader;
  typedef Usul::Interfaces::ITreeNode ITreeNode;
  typedef Usul::Interfaces::IBooleanState IBooleanState;
  typedef Usul::Math::Vec4f Color;
  typedef std::map < std::string, Color > Colors;
  typedef Usul::Pointers::WeakPointer < ModflowDocument > WeakPtr;
  typedef Modflow::Interfaces::IModflowModel IModflowModel;
  typedef std::map < Modflow::Geometry::ID, Modflow::Builders::Geometry::RefPtr > BuilderMap;
  typedef Usul::Interfaces::ITimeVaryingData ITimeVaryingData;
  typedef std::pair < bool, long > NoDataValue;
  typedef std::map < std::string, NoDataValue > NoDataValues;
  typedef std::map < std::string, std::string > FileAttributes;
  typedef std::map < std::string, Vec3d > Offsets;
  typedef OGRSpatialReference CoordinateSystem;
  typedef std::auto_ptr<CoordinateSystem> CoordinateSystemPtr;
  typedef OGRCoordinateTransformation CoordinateTransformation;
  typedef std::auto_ptr<CoordinateTransformation> CoordinateTransformationPtr;
  typedef Modflow::Attributes::Attribute Attribute;
  typedef std::vector < Attribute::RefPtr > Attributes;
  typedef Usul::Types::Uint64 TimeStamp;
  typedef std::map < unsigned int, TimeStamp > TimeStamps;

  // Type information.
  USUL_DECLARE_TYPE_ID ( ModflowDocument );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ModflowDocument );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Construction.
  ModflowDocument();

  // Add the attribute.
  void                                    addAttribute ( Attribute * );

  // Add a view.
  void                                    addView ( BaseClass::View* view );

  // Get the builder.
  Modflow::Builders::Geometry *           builder ( Modflow::Geometry::ID ) const;

  // Build the scene.
  virtual osg::Node *                     buildScene ( const BaseClass::Options &options, Unknown *caller = 0x0 );

  // Return true if this document can do it.
  virtual bool                            canExport ( const std::string &file ) const;
  virtual bool                            canInsert ( const std::string &file ) const;
  virtual bool                            canOpen   ( const std::string &file ) const;
  virtual bool                            canSave   ( const std::string &file ) const;

  // Get the cell size.
  Vec2d                                   cellSize() const;

  // Clear any existing data.
  virtual void                            clear ( Unknown *caller = 0x0 );

  // Get/set the color.
  Color                                   color ( const std::string &name ) const;
  void                                    color ( const std::string &name, const Color & );

  // Get the data-layer. Use for data that does not belong to a particular model layer.
  Layer *                                 dataLayer();
  
  // Get/Set the destination coordinate system.
  CoordinateSystem*                       destinationCoordinateSystem() const;
  void                                    destinationCoordinateSystem( CoordinateSystem* );

  // Set/get the dirty flag.
  virtual void                            dirtyState ( bool );
  virtual bool                            dirtyState() const;

  // Get the elevation for this cell position. Comes from the land surface in the 
  // discretization file. Throws if input is out of range.
  double                                  elevation ( unsigned int row, unsigned int column ) const;

  // Set the elevations. This is the land surface in the discretization file.
  void                                    elevations ( const GridData & );

  // Get the filters that correspond to what this document can read and write.
  virtual Filters                         filtersOpen()   const;
  virtual Filters                         filtersSave()   const;
  virtual Filters                         filtersInsert() const;
  virtual Filters                         filtersExport() const;

  // Set/get the flag.
  void                                    flags ( unsigned int );
  unsigned int                            flags() const;

  // Return the grid size.
  Vec2ui                                  gridSize() const;

  // Usul::Interfaces::IIntersectListener.
  virtual void                            intersectNotify ( float x, float y, const osgUtil::LineSegmentIntersector::Intersection &hit, Usul::Interfaces::IUnknown *caller );
  
  /// See if the given level falls within this layer's range of levels.
  bool                                    isInLevelRange ( unsigned int level ) const;

  // Set/get the layers.
  void                                    layers ( Layers & );
  const Layers &                          layers() const;
  Layers &                                layers();

  // Set/get the length conversion.
  void                                    lengthConversion ( double );
  double                                  lengthConversion() const;

  // Set/get the "no data" value.
  void                                    noDataSet ( const std::string &name, long );
  NoDataValue                             noDataGet ( const std::string &name ) const;

  // Set/get the number of sides to use when making cylinders.
  void                                    numCylinderSides ( unsigned int );
  unsigned int                            numCylinderSides() const;

  // Return number of layers.
  unsigned int                            numLayers() const;

  // Return the number of time-stamps.
  virtual unsigned int                    numTimeStamps() const;

  // Set/get the i'th time-stamp.
  virtual void                            timeStampSet ( unsigned int i, const TimeStamp &stamp );
  virtual bool                            timeStampGet ( unsigned int i, TimeStamp &stamp ) const;

  // Get/set the offset.
  Vec3d                                   offsetGet ( const std::string &name, const Vec3d &defaultValue = Vec3d ( 0, 0, 0 ) ) const;
  void                                    offsetSet ( const std::string &name, const Vec3d &value );
  
  // Get/Set the source coordinate system.
  CoordinateSystem*                       sourceCoordinateSystem() const;
  void                                    sourceCoordinateSystem( CoordinateSystem* );

  // Transform coordinates.
  void                                    transformCoordinate  ( osg::Vec3f& ) const;
  void                                    transformCoordinates ( osg::Vec3Array& ) const;

  // Read the document.
  virtual void                            read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  // Write the document to given file name.
  virtual void                            write ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0  ) const;

  // Usul::Interfaces::IUpdateListener
  virtual void                            updateNotify ( Usul::Interfaces::IUnknown *caller );

  // Usul::Interfaces::ITreeNode.
  virtual unsigned int                    getNumChildNodes() const;
  virtual ITreeNode::RefPtr               getChildNode ( unsigned int which );
  virtual void                            setTreeNodeName ( const std::string & );
  virtual std::string                     getTreeNodeName() const;

  // Usul::Interfaces::IBooleanState.
  virtual void                            setBooleanState ( bool );
  virtual bool                            getBooleanState() const;

  // Get z-range [min,max].
  Vec2d                                   zRange() const;

  //
  // Modflow::Interfaces::IModflowModel
  //

  // Set/get the vertical scale.
  virtual void                            setVerticalScale ( double );
  virtual double                          getVerticalScale() const;

  // Set/get the cell margin.
  virtual void                            setCellMargin ( double );
  virtual double                          getCellMargin() const;

  //
  // Usul::Interfaces::ITimeVaryingData
  //

  // Set/get the current time step.
  void                                    setCurrentTimeStep ( unsigned int );
  unsigned int                            getCurrentTimeStep() const;

  // Set/get the number of time steps.
  void                                    setNumberOfTimeSteps ( unsigned int );
  unsigned int                            getNumberOfTimeSteps() const;

  //
  // Usul::Interfaces::IGetBoundingBox
  //

  // Get the bounding-box.
  virtual osg::BoundingBox                getBoundingBox() const;

  //
  // Usul::Interfaces::ILayer
  //

  // Get the guid.
  virtual std::string                     guid() const;

  // Get/Set the name.
  virtual std::string                     name() const;
  virtual void                            name ( const std::string & );

  // Get/Set show layer
  virtual void                            showLayer ( bool b );
  virtual bool                            showLayer() const;

protected:

  void                                    _buildScene ( Unknown *caller );

  void                                    _clearLayers();

  void                                    _destroy();

  void                                    _incrementProgress ( bool state, Unknown *progress, unsigned int &numerator, unsigned int denominator );

  void                                    _read ( ObjectFactory &, XmlTree::Node *file, Unknown *progress );
  void                                    _read ( ObjectFactory &, const std::string &type, const std::string &file, const std::string &noData, const FileAttributes &, Unknown *progress );

  void                                    _setCoordinateSystem ( XmlTree::Node * );

  void                                    _transformCoordinates ( osg::Geode *geode, Usul::Interfaces::IUnknown *caller ) const;

  std::string                             _wellKnownText() const;

  // Do not copy.
  ModflowDocument ( const ModflowDocument & );
  ModflowDocument &operator = ( const ModflowDocument & );

  // Use reference counting.
  virtual ~ModflowDocument();

private:

  GridVector _bounds;
  GridVector _startHeads;
  GridInfo _landElev;
  GridVector _bottomElev;
  Layers _layers;
  Layer::RefPtr _dataLayer;
  osg::ref_ptr<osg::MatrixTransform> _root;
  osg::ref_ptr<osg::Group> _built;
  unsigned int _flags;
  Colors _colors;
  Vec2d _zRange;
  double _verticalScale;
  double _cellMargin;
  BuilderMap _builders;
  unsigned int _timeStep;
  unsigned int _numTimeSteps;
  NoDataValues _noData;
  CoordinateSystemPtr _sourceCoordinateSystem;
  CoordinateSystemPtr _destinationCoordinateSystem;
  std::auto_ptr<CoordinateTransformation> _coordinateTransformation;
  Attributes _attributes;
  double _lengthConversion;
  Offsets _offsets;
  TimeStamps _timeStamps;
  unsigned int _cylinderSides;
  GridData _elevations;
  XmlTree::Document::RefPtr _xmlDoc;
};


} // namespace Modflow


#endif // _POLYGON_MODEL_DOCUMENT_H_
