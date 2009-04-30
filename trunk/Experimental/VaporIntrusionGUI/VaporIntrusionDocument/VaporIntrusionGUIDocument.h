
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

#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Interfaces/IMpdNavigator.h"
#include "Usul/Interfaces/IMenuAdd.h"
#include "Usul/Interfaces/IVaporIntrusionGUI.h"

#include "Usul/Jobs/Job.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector4.h"
#include "Usul/Policies/Update.h"

#include "XmlTree/Document.h"

#include "osg/Image"
#include "osg/LineSegment"
#include "osg/Group"
#include "osg/Material"

#include <string>
#include <memory>

namespace osg { class Node; }


class VaporIntrusionGUIDocument : public Usul::Documents::Document,
                                  public Usul::Interfaces::IBuildScene,
                                  public Usul::Interfaces::IUpdateListener,
                                  public Usul::Interfaces::IVaporIntrusionGUI
                                  
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
  typedef std::vector< std::pair< double, double > >            GridPoints;
  typedef Usul::Interfaces::IVaporIntrusionGUI::ParameterList   ParameterList;
  typedef IVaporIntrusionGUI::InputColumns                      InputColumns;
  typedef IVaporIntrusionGUI::InputColumn                       InputColumn;
  typedef IVaporIntrusionGUI::Category                          Category;
  typedef IVaporIntrusionGUI::Categories                        Categories;
  
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
  
  /// Read the document.
  virtual void                read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  /// Write the document to given file name.
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0  ) const;

  /// Clear any existing data.
  virtual void                clear ( Unknown *caller = 0x0 );

  /// Usul::Interfaces::IVaporIntrusionGUI
  virtual void                highlightCells( Usul::Math::Vec3ui set, unsigned int depth );
  virtual void                setAlpha( unsigned int x, unsigned int y, unsigned int z, float alpha );
  virtual void                setAlpha( float alpha );
  virtual Usul::Math::Vec3ui  dimensions();
  virtual void                dimensions( Usul::Math::Vec3ui d );
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

protected:

  /// Do not copy.
  VaporIntrusionGUIDocument ( const VaporIntrusionGUIDocument & );
  VaporIntrusionGUIDocument &operator = ( const VaporIntrusionGUIDocument & );

  /// Use reference counting.
  virtual ~VaporIntrusionGUIDocument();

  /// Usul::Interfaces::IUpdateListener
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );

  // Build the scene in the read
  void                        _buildScene( Usul::Interfaces::IUnknown *caller = 0x0 );

  // Test method for multiview verification
  osg::Node*                  _buildTestCube( osg::Vec3Array* points, Color c, Usul::Math::Vec3ui location );

  void                        _initCubes();

  // Write the necessary files for the Vapor Intrusion Process
  void                        _write( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0  ) const;
  void                        _writeLayerFile( const std::string& directory ) const;
  void                        _writeCoordinatesFiles( const std::string& directory ) const;

  // read layer file
  void                        _readConfigFile( const std::string& name, const std::string& filename );
  void                        _readInitializationFile( const std::string& filename );
  //void                      _readConfig( const std::string& filename );


private:
    GroupPtr                  _root;
    Usul::Math::Vec3ui        _dimensions;
    Cubes                     _cubes;
    GridPoints                _xValues;
    GridPoints                _yValues;
    GridPoints                _zValues;
    ParameterList             _inputParameters;
    std::string               _configFileName;
    Categories                _categories;
  
};



#endif // _VAPOR_INTRUSION_GUI_DOCUMENT_H_
