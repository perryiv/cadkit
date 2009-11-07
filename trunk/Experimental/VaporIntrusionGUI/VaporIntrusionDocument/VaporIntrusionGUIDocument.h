
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for mpd files
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VAPOR_INTRUSION_GUI_DOCUMENT_H_
#define _VAPOR_INTRUSION_GUI_DOCUMENT_H_

#include "VaporIntrusionGUI/Interfaces/IVaporIntrusionGUI.h"

#include "XmlTree/Document.h"

#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Interfaces/IKeyListener.h"
#include "Usul/Interfaces/IMpdNavigator.h"
#include "Usul/Interfaces/IMenuAdd.h"

#include "Usul/Jobs/Job.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector4.h"
#include "Usul/Policies/Update.h"

#include "osg/Image"
#include "osg/LineSegment"
#include "osg/Group"
#include "osg/Material"

#include <string>
#include <memory>
#include <fstream>

namespace osg { class Node; }


class VaporIntrusionGUIDocument : public Usul::Documents::Document,
                                  public Usul::Interfaces::IBuildScene,
                                  public Usul::Interfaces::IUpdateListener,
                                  public VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI,
                                  public Usul::Interfaces::IMenuAdd
                                  
{
public:
  /// Useful typedefs.
  typedef Usul::Documents::Document                             BaseClass;
  typedef Usul::Documents::Document                             Document;
  typedef Usul::Documents::Document::RefPtr                     DocumentPtr;
  typedef Usul::Documents::Manager                              DocManager;
  typedef DocManager::DocumentInfo                              Info;
  typedef osg::ref_ptr< osg::Group >                            GroupPtr;
  typedef osg::Vec4                                             Color;
  typedef VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI     IVPI;
  typedef IVPI::GridPoint                                       GridPoint;
  typedef IVPI::GridPoints                                      GridPoints;
  typedef IVPI::ParameterList                                   ParameterList;
  typedef IVPI::Object2D                                        Object2D;
  typedef IVaporIntrusionGUI::InputColumns                      InputColumns;
  typedef IVaporIntrusionGUI::InputColumn                       InputColumn;
  typedef IVaporIntrusionGUI::Category                          Category;
  typedef IVaporIntrusionGUI::Categories                        Categories;
  typedef IVaporIntrusionGUI::MaterialsMap                      MaterialsMap;
  typedef IVaporIntrusionGUI::GridMaterial                      GridMaterial;
  typedef IVaporIntrusionGUI::GridMaterials                     GridMaterials; 
  typedef IVaporIntrusionGUI::Building                          Building;
  typedef IVaporIntrusionGUI::Crack                             Crack;
  typedef IVaporIntrusionGUI::Cracks                            Cracks;
  typedef IVaporIntrusionGUI::CracksPair                        CracksPair;
  typedef IVaporIntrusionGUI::Source                            Source;
  typedef IVaporIntrusionGUI::Sources                           Sources;
  typedef IVaporIntrusionGUI::Soil                              Soil;
  typedef IVaporIntrusionGUI::Soils                             Soils;
  typedef std::vector< std::string >                            StringVec;
  typedef std::map< std::string, unsigned int >                 GridMap;

  // function typedefs
  typedef Usul::Convert::Type< std::string, float > StrToFloat;

  
  // structs and typedefs for structs
  struct Value
  {
    std::string name;
    std::string value;

    Value():
      name( "default" ),
      value( "0" )
      {};

    Value( const std::string& n, const std::string& v ) :
      name( n ),
      value( v )
      {};
  };
  typedef std::vector< Value > Values;

  struct Cell
  {
    osg::Vec4 color;
    GroupPtr group;
    std::string name;
    Values values;
    Usul::Math::Vec3f position, dimensions;

    Cell() : 
      color( 0.5, 0.5, 0.5, 0.5 ), 
      group ( new osg::Group ),
      name(),
      values(),
      position( 0.0f, 0.0f, 0.0f ),
      dimensions( 1.0f, 1.0f, 1.0f )
      {};

      Cell( osg::Vec4 c, GroupPtr g, std::string n, Values v, Usul::Math::Vec3f pos, Usul::Math::Vec3f dim ) :
        color( c), 
        group ( g ),
        name( n ),
        values( v ),
        position( pos ),
        dimensions( dim )
        {};
  };

  typedef Cell Cube;
  //typedef std::pair< ValueType, GroupPtr > Cube;
  typedef std::vector< std::vector< std::vector< Cube > > > Cubes;



  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( VaporIntrusionGUIDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  VaporIntrusionGUIDocument();

  /// Build the scene.
  virtual osg::Node *         buildScene ( const BaseClass::Options &options, Unknown *caller = 0x0 );

  /// Return true if this document can do it.
  virtual bool                canExport ( const std::string &file ) const;
  virtual bool                canInsert ( const std::string &file ) const;
  virtual bool                canOpen   ( const std::string &file ) const;
  virtual bool                canSave  ( const std::string &file ) const;

  /// Get the filters that correspond to what this document can read and write.
  virtual Filters             filtersOpen()   const;
  virtual Filters             filtersSave()   const;
  virtual Filters             filtersInsert() const;
  virtual Filters             filtersExport() const;

  //Usul::Interfaces::IMenuAdd
  virtual void                menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller = 0x0 );
  
  /// Read the document.
  virtual void                read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  /// Write the document to given file name.
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0  ) const;

  /// Clear any existing data.
  virtual void                clear ( Unknown *caller = 0x0 );

  /// VaporIntrusionGUI::Interfaces::IVaporIntrusionGUI
  virtual void                highlightCells( Usul::Math::Vec3ui set, unsigned int depth );
  virtual void                setAlpha( unsigned int x, unsigned int y, unsigned int z, float alpha );
  virtual void                setAlpha( float alpha );
  virtual Usul::Math::Vec3ui  dimensions();
  virtual void                dimensions( Usul::Math::Vec3ui d );
  virtual void                setInitialSpacing( Usul::Math::Vec3f spacing );
  virtual void                setMaterial( unsigned int x, unsigned int y, unsigned int z, Usul::Math::Vec4f c );
  virtual Usul::Math::Vec4f   getMaterial( unsigned int x, unsigned int y, unsigned int z );
  virtual void                requestRedraw();
  virtual void                setValueAt( unsigned int x, unsigned int y, unsigned int z, unsigned int index, const std::string& value );
  virtual void                setNameAt( unsigned int x, unsigned int y, unsigned int z, unsigned int index, const std::string& name );
  virtual void                addValueAt( unsigned int x, unsigned int y, unsigned int z, const std::string& name, const std::string& value );
  virtual void                addValueAt( unsigned int x, unsigned int y, unsigned int z, const std::string& value );
  virtual GridPoints          getXGrid();
  virtual GridPoints          getYGrid();
  virtual GridPoints          getZGrid();
  virtual void                setXGrid( GridPoints points );
  virtual void                setYGrid( GridPoints points );
  virtual void                setZGrid( GridPoints points );
  virtual void                rebuildScene();
  //virtual void                readParamaterFile( const std::string& filename );
  virtual ParameterList       parameters();
  virtual void                parameters( ParameterList plist );
  virtual Categories          categories();
  virtual void                categories( Categories c );
  virtual void                readConfigFile( const std::string& name, const std::string& filename );
  virtual void                initialize();
  virtual void                updateCategory( Category category );
  virtual bool                checkValue( int comparitor, const std::string& value1, const std::string& value2 );
  //dragger stuff
  virtual bool                draggerActive();
  virtual void                draggerActive( bool value );   

  virtual GridMaterials       gridMaterials();
  virtual void                gridMaterials( GridMaterials gm );

  virtual void                building( Building b );
  virtual Building            building();
  virtual void                useBuilding( bool value );
  virtual bool                useBuilding();

  virtual void                sources( Sources s );
  virtual Sources             sources();

  virtual void                contaminants( Contaminants c );
  virtual Contaminants        contaminants();

  virtual void                soils( Soils s );
  virtual Soils               soils();

  virtual void                cracks( CracksPair c );
  virtual CracksPair          cracks();

  virtual bool                symmetricalGrid();
  virtual void                symmetricalGrid( bool value );

  virtual GridAxisPoints      gridAxisPoints();
  virtual void                gridAxisPoints( GridAxisPoints ap );

  virtual void                addGridPointFromViewer( Usul::Math::Vec3f point );
  virtual void                removeGridPointFromViewer( Usul::Math::Vec3f point );

  virtual void                setEditMode2D( int mode );
  virtual int                 getEditMode2D();

  virtual void                setBuildMode2D( int mode );
  virtual int                 getBuildMode2D();

  virtual void                setObjectMode( int mode );
  virtual int                 getObjectMode();

  virtual void                keyMovementChange( int x, int y );


  // Show/Hide functions
  void                        showBuilding ( bool b );
  bool                        isShowBuilding () const;

  void                        showGrid( bool b );
  bool                        isShowGrid () const;

  void                        showCracks( bool b );
  bool                        isShowCracks () const;

  void                        showFoundation( bool b );
  bool                        isShowFoundation () const;

  void                        showSources ( bool b );
  bool                        isShowSources () const;

  // Object menu functions
  void                        objectMenuAddBuilding();

  void                        handleNewObject();






protected:

  /// Do not copy.
  VaporIntrusionGUIDocument ( const VaporIntrusionGUIDocument & );
  VaporIntrusionGUIDocument &operator = ( const VaporIntrusionGUIDocument & );

  /// Use reference counting.
  virtual ~VaporIntrusionGUIDocument();

  /// Usul::Interfaces::IUpdateListener
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );

  // Build the scene in the read
  void                        _build3DScene( Usul::Interfaces::IUnknown *caller = 0x0 );
  void                        _build2DScene( Usul::Interfaces::IUnknown *caller = 0x0 );

  // Test method for multiview verification
  osg::Node*                  _buildCube( osg::Vec3Array* points, Color c, Usul::Math::Vec3ui location );
  osg::Node*				          _buildPlane ( osg::Vec3Array* points, osg::Vec4f color );

  // 2d build methods
  osg::Node*                  _buildXYScene();
  osg::Node*                  _buildZScene();
  osg::Node*                  _buildObject();

  void                        _makeGrid();
  void                        _makeSymmetricalBuilding();
  void                        _makeBuilding();
  void                        _makeSource3D();
  void                        _makeFoundation( osg::Vec3f ll );
  void						            _makeCracks();

  void                        _initCubes();

  // Write the necessary files for the Vapor Intrusion Process
  void                        _write( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0  ) const;
  void                        _writeLayerFile( const std::string& directory ) const;
  void                        _writeLayerFile2( const std::string& directory ) const;
  void                        _writeCoordinatesFiles( const std::string& directory ) const;

  // read layer file
  void                        _readConfigFile( const std::string& catName, const std::string& filename );
  void                        _readInitializationFile( const std::string& filename );
  void                        _readContaminants( const std::string& filename );
  void                        _readSources( const std::string& filename );
  void                        _readSoils( const std::string& filename );
  void                        _readCracks( const std::string& filename );
  
  int                         _getComparitor( const std::string& comparitor );

  
  void                        _activateDragger();

  void                        _initializeGridMaterials();

  void                        _createDock( const std::string& name );
  void                        _addMaterialToDock( const std::string& dock, const std::string& name, const std::string& value );
  

  void                        _readAllofValues( unsigned int& index, InputColumn& column, const StringVec& sv );
  void                        _readOneofValues( unsigned int& index, InputColumn& column, const StringVec& sv );

  void                        _insertGridSpacing( const std::string& axis, unsigned int startPoint, unsigned int endPoint, unsigned int numToAdd );
  void                        _insertSingleGridSpace( const std::string& axis, unsigned int startPoint, unsigned int endPoint, float value );
  void                        _insertGridPoint( const std::string& axis, float pos );
  void                        _removeGridPoint( const std::string& axis, float pos );
  void                        _fixDimensions();

  GridPoints                  _getGridFromAxis( const std::string& axis );
  void                        _setGridFromAxis( const std::string& axis, GridPoints grid );

  Usul::Math::Vec2ui          _snapToGrid( float value, GridPoints grid );
  osg::Vec2f                  _snapToGrid2D( osg::Vec2f corner );
  osg::Vec3f                  _snapToGrid3D( osg::Vec3f corner );

  void                        _addPoints();
  void                        _addGridPadding();
  bool                        _gridHasPoint( const std::string& axis, float pos );

  void                        _restoreGrid();

  // object creation methods
  void                        _createNewBuilding();

  


  

private:
    GroupPtr                  _root;
    GroupPtr                  _root2D;
    Usul::Math::Vec3ui        _dimensions;
    Usul::Math::Vec3f         _initialSpacing;
    Cubes                     _cubes;
    GridPoints                _xValues;
    GridPoints                _yValues;
    GridPoints                _zValues;
    ParameterList             _inputParameters;
    std::string               _configFileName;
    Categories                _categories;
    bool                      _draggerState;
    GridMaterials             _gridMaterials;
    Building                  _building;
    bool                      _useBuilding;
    Sources                   _sources;
    Contaminants              _contaminants;
    Soils                     _soils;
    CracksPair                _cracks;
    GridAxisPoints            _axisPoints;
    GridMap                   _originalToCurrentIndex;
    bool                      _symmetricalGrid;

    GridPoints                _originalXValues;
    GridPoints                _originalYValues;
    GridPoints                _originalZValues;

    bool                      _showGrid;
    bool                      _showBuilding;
    bool                      _showFoundation;
    bool                      _showSources;
    bool                      _showCracks;
    bool                      _showLabels;

    float                     _maxCrackGridDistance;
    int                       _buildMode2D;
    int                       _editGridMode2D;
    int                       _objectMode;

    Object2D                  _currentObject;
    float                     _minimumGridDistance;
  
};



#endif // _VAPOR_INTRUSION_GUI_DOCUMENT_H_
