
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008,2009 Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IVAPORINTRUSIONGUI_H__
#define __USUL_INTERFACES_IVAPORINTRUSIONGUI_H__

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector4.h"

#include <vector>
#include <map>

namespace VaporIntrusionGUI {
namespace Interfaces {

struct IVaporIntrusionGUI : public Usul::Interfaces::IUnknown
{

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IVaporIntrusionGUI );

  // useful typedefs
  typedef std::pair < int, std::string > ActivatorValue; // expected < comparison enumeration, value >
  typedef std::pair < std::string, ActivatorValue > ActivatorPair; // expected < activator name, activator value >
  typedef std::vector< ActivatorPair > ActivatorPairs;

  /// Id for this interface.
  enum { IID = 1235852807u };

  // comparison enumerations
  enum
  {
    EQUAL,
    NOT_EQUAL,
    LESS_THAN,
    GREATER_THAN,
    GREATER_THAN_OR_EQUAL,
    LESS_THAN_OR_EQUAL,
    SIZEOF_EQUAL,
  };

  // view modes
  enum
  { 
    VIEW_MODE_2D_XY,
    VIEW_MODE_2D_XZ,
    VIEW_MODE_2D_YZ
  };

  // build modes
  enum
  {
    BUILD_MODE_NONE,
    BUILD_MODE_GRID_EDIT,
    BUILD_MODE_CRACK_PLACE1,
    BUILD_MODE_CRACK_PLACE2,
    BUILD_MODE_OBJECT_PLACEMENT_XY,
    BUILD_MODE_OBJECT_PLACEMENT_XZ,
    BUILD_MODE_OBJECT_SIZE_XY,
    BUILD_MODE_OBJECT_SIZE_XZ
  };
  // 2D edit modes
  enum
  {
    EDIT_MODE_IDLE,
    EDIT_X_GRID_2D,
    EDIT_Y_GRID_2D,
    EDIT_Z_GRID_2D
  };

  // Objects
  enum
  {
    OBJECT_NOTHING,
    OBJECT_BUILDING,
    OBJECT_SOIL,
    OBJECT_SOURCE,
    OBJECT_FOUNDATION,
    OBJECT_CRACK
  };


  // structs

  struct Object2D
  {
    int sx, sy, sz, ex, ey, ez;

    Object2D() : sx ( 0 ), sy ( 0 ), sz ( 0 ), ex ( 0 ), ey ( 0 ), ez ( 0 ) {};
    Object2D( int sxi, int syi, int szi, int exi, int eyi, int ezi ) : 
    sx ( sxi ), sy ( syi ), sz ( szi ), ex ( exi ), ey ( eyi ), ez ( ezi ) {};

  };

  struct GridAxisPoint
  {
    std::string axis, value;
    GridAxisPoint() : axis( "x" ), value( "0" ) {};
    GridAxisPoint( const std::string& a, const std::string& v ) : axis ( a ), value ( v ){};
  };
  typedef std::vector< GridAxisPoint > GridAxisPoints;

  struct Crack
  {
    std::string start, end, value;
    Usul::Math::Vec4f color;
    Crack(): start(), end(), value(), color( 0.0f, 1.0f, 0.0f, 1.0f ){};
    Crack( const std::string& s, const std::string& e, const std::string& v ):
    start( s ), end( e ), value( v ), color( 0.0f, 1.0f, 0.0f, 1.0f ){};
  };
  typedef std::vector< Crack > Cracks;
  typedef std::pair< Cracks, Cracks > CracksPair;

  struct Building
  {
    std::string l,w,h;
    std::string x,y,z;
    std::string depth, v, xrate, thickness;
    Usul::Math::Vec4f bColor, fColor;

    Building( const std::string& length, const std::string& width, const std::string& height,
              const std::string& xpos, const std::string& ypos, const std::string& zpos, 
              const std::string& d, const std::string& vol,
              const std::string& xr, const std::string& th ):
    l ( length ), w ( width ), h ( height ), x ( xpos ), y ( ypos ), z ( zpos ),
    depth( d ), v ( vol ), xrate ( xr ), thickness( th ), bColor( 0.4f, 0.0f, 0.4f, 1.0f ), fColor( 1.0f, 1.0f, 1.0f, 1.0f )
    {};

    Building():
    l ( "1" ), w ( "1" ), h ( "1" ), x ( "0" ), y ( "0" ), z ( "0" ), depth( "0" ), v(  "1" ), xrate ( "" ), thickness( "" )
    {};
  };
  
  struct Soil
  {
    
    std::string name, type, porosity, waterPorosity, permeability, viscosity, carbon;
    std::string layerName;
    std::string x, y, z, l, w, h;
    Usul::Math::Vec4f color;

    // SCS soil type, SCS soil name, Total porosity, water-filled porosity, effective soil gas permeability, 
    // Gas phase dynamic viscosity,  Fraction of organic carbon

    Soil(): name(), type(), porosity(), waterPorosity(), permeability(), viscosity(), carbon() {};
    Soil( const std::string& n, const std::string& t, const std::string& p, const std::string& wp,
          const std::string& perm, const std::string& visc, const std::string& carb ):
          name( n ), type( t ), porosity( p ), waterPorosity( wp ), 
          permeability( perm ), viscosity( visc ), carbon( carb ) {};

    void dimensions( const std::string& x1, const std::string& y1, const std::string& z1,
                const std::string& l1, const std::string& w1, const std::string& h1 )
              {  x = x1;  y = y1;  z = z1;  l = l1;  w = w1;  h = h1;  };

    void attributes( const std::string& n, const std::string& t, const std::string& p, const std::string& wp,
                     const std::string& perm, const std::string& visc, const std::string& carb )
    { name = n; type = t; porosity = p; waterPorosity = wp; permeability = perm; viscosity = visc; carbon = carb; };

  };
  typedef std::vector< Soil > Soils;
  typedef std::vector< Soil > SoilLibrary;


  struct Chemical
  {
    unsigned int index;
    std::string name, henry, koc, airDiff, waterDiff, atmoConc;
    std::string sourceConc;
    bool present;
    
    Chemical():
    index( 0 ), name(), henry(), koc(), airDiff(), waterDiff(), atmoConc(), present( true )
    {
      sourceConc = "0";
    };

    Chemical( unsigned int i, const std::string& n, const std::string& h, const std::string& k,
                 const std::string& ad, const std::string& wd, const std::string& ac ):
    index( i ), name( n ), henry( h ), koc( k ), airDiff( ad ), waterDiff( wd ), atmoConc( ac ), present( true )
    {
      sourceConc = "0";
    };

  };
  typedef std::vector< Chemical > Chemicals;

  typedef std::vector< std::string > Concentrations;
  struct Source
  {
    std::string l,w,h;
    std::string x,y,z;
    std::string name;
    Chemicals chemicals;
    Concentrations concentrations;
    Usul::Math::Vec4f color;

    Source():
    l ( "1" ), w ( "1" ), h ( "1" ), x ( "0" ), y ( "0" ), z ( "0" ), name ( "" ), chemicals()
    {};

    Source( const std::string& length, const std::string& width, const std::string& height,
                 const std::string& xpos, const std::string& ypos, const std::string& zpos,
                 const std::string& n, Chemicals c ):
    l ( length ), w ( width ), h ( height ), x ( xpos ), y ( ypos ), z ( zpos ), name( n ), chemicals( c )
    {};

  };
  typedef std::vector< Source > Sources;

  struct InputColumn
  {
    // #Name,Value,Description,Type,Activators
    std::string name;
    std::string value;
    std::string description;
    std::string type;
    ActivatorPairs allofActivators;
    ActivatorPairs oneofActivators;
    bool active;

    InputColumn():
      name (),
      value(),
      description(),
      type(),
      allofActivators(),
      oneofActivators(),
      active( false )
      {}
    InputColumn( const std::string& n, const std::string& v, const std::string& d, const std::string& t ) :
      name ( n ),
      value( v ),
      description( d ),
      type( t ),
      allofActivators(),
      oneofActivators(),
      active( false )
      {}
  };

  // vector of InputColumn
  typedef std::map< std::string, InputColumn > InputColumns;

  // Menu holders
  struct Category
  {
    std::string name;
    std::string filename;
    std::vector< std::string > columnNames;
    InputColumns columns;

    Category( const std::string& n, const std::string& fn ):
      name( n ),
      filename( fn ),
      columnNames(),
      columns()
      {}

      Category():
      name(),
      filename(),
      columnNames(),
      columns()
      {}
  };

  typedef std::vector< Category > Categories;

  typedef std::map< std::string, std::string > MaterialsMap;

  struct GridMaterial
  {
    unsigned int x,y,z;
    MaterialsMap materials;

    GridMaterial() :
    x( 0 ), y( 0 ), z( 0 ), materials()
    {};

    GridMaterial( unsigned int ix, unsigned int iy, unsigned int iz ) :
    x( ix ), y( iy ), z( iz ), materials()
    {};
  };

  typedef std::map< std::string, GridMaterial > GridMaterials;

  // Highlight the set of cubes at depth <depth>
  virtual void                  highlightCells( Usul::Math::Vec3ui set, unsigned int depth ) = 0;
  
  // Set the transparency
  virtual void                  setAlpha( unsigned int x, unsigned int y, unsigned int z, float alpha ) = 0;

  // Set the transparency
  virtual void                  setAlpha( float alpha ) = 0;

  // Get/set the dimensions of the space
  virtual Usul::Math::Vec3ui    dimensions() = 0;
  virtual void                  dimensions( Usul::Math::Vec3ui ) = 0;
  virtual void                  setInitialSpacing( Usul::Math::Vec3f spacing ) = 0;

  // Get/Set the material
  virtual void                  setMaterial( unsigned int x, unsigned int y, unsigned int z, Usul::Math::Vec4f c ) = 0;
  virtual Usul::Math::Vec4f     getMaterial( unsigned int x, unsigned int y, unsigned int z ) = 0;

  // Request redraw
  virtual void                  requestRedraw() = 0;

  virtual void                  setValueAt( unsigned int x, unsigned int y, unsigned int z, unsigned int index, const std::string& value ) = 0;
  virtual void                  setNameAt( unsigned int x, unsigned int y, unsigned int z, unsigned int index ,  const std::string& name ) = 0;
  virtual void                  addValueAt( unsigned int x, unsigned int y, unsigned int z, const std::string& name, const std::string& value ) = 0;
  virtual void                  addValueAt( unsigned int x, unsigned int y, unsigned int z, const std::string& value ) = 0;

  typedef std::pair< double, double > GridPoint;
  typedef std::vector< GridPoint > GridPoints;

  virtual GridPoints            getXGrid() = 0;
  virtual GridPoints            getYGrid() = 0;
  virtual GridPoints            getZGrid() = 0;

  virtual void                  setXGrid( GridPoints points ) = 0;
  virtual void                  setYGrid( GridPoints points ) = 0;
  virtual void                  setZGrid( GridPoints points ) = 0;

  virtual void                  rebuildScene() = 0;

  typedef std::pair< std::string, std::string > Parameter;
  typedef std::pair< unsigned int, Parameter > ParameterPair;
  typedef std::vector< ParameterPair > ParameterList;

  virtual ParameterList         parameters() = 0;
  virtual void                  parameters( ParameterList plist ) = 0;

  virtual Categories            categories() = 0;
  virtual void                  categories( Categories c ) = 0;

  virtual void                  initialize() = 0;

  virtual void                  readConfigFile( const std::string& name, const std::string& filename ) = 0;
  virtual void                  updateCategory( Category category ) = 0;

  virtual bool                  checkValue( int comparitor, const std::string& value1, const std::string& value2 ) = 0;

  //dragger stuff
  virtual bool                  draggerActive() = 0;
  virtual void                  draggerActive( bool value ) = 0; 

  virtual GridMaterials         gridMaterials() = 0;
  virtual void                  gridMaterials( GridMaterials gm ) = 0;

  virtual void                  building( Building b ) = 0;
  virtual Building              building() = 0;
  virtual void                  useBuilding( bool value ) = 0;
  virtual bool                  useBuilding() = 0;

  virtual void                  sources( Sources s ) = 0;
  virtual Sources               sources() = 0;

  virtual void                  chemicals( Chemicals c ) = 0;
  virtual Chemicals             chemicals() = 0;
  virtual Chemicals             library() = 0;
  virtual void                  library( Chemicals l ) = 0;

  virtual void                  soils( Soils s ) = 0;
  virtual Soils                 soils() = 0;
  virtual void                  soilLibrary( SoilLibrary l ) = 0;
  virtual SoilLibrary           soilLibrary() = 0;

  virtual void                  cracks( CracksPair c ) = 0;
  virtual CracksPair            cracks() = 0;

  virtual bool                  symmetricalGrid() = 0;
  virtual void                  symmetricalGrid( bool value ) = 0;

  virtual GridAxisPoints        gridAxisPoints() = 0;
  virtual void                  gridAxisPoints( GridAxisPoints ap ) = 0;

  virtual void                  handleLeftMouseClick( Usul::Math::Vec3f point ) = 0;
  virtual void                  handleRightMouseClick( Usul::Math::Vec3f point ) = 0;
  virtual void                  setMouseCoords( Usul::Math::Vec3f point ) = 0;

  // edit modes for the 2D grid
  virtual void                  setEditMode2D( int mode ) = 0;
  virtual int                   getEditMode2D() = 0;

  virtual void                  setBuildMode2D( int mode ) = 0;
  virtual int                   getBuildMode2D() = 0;

  virtual void                  setObjectMode( int mode ) = 0;
  virtual int                   getObjectMode() = 0;

  virtual void                  setViewMode2D( int mode ) = 0;
  virtual int                   getViewMode2D() = 0;

  virtual void                  keyMovementChange( int x, int y ) = 0;
  virtual void                  handleNewObject() = 0;

  // clear functions
  virtual void                  clearObject() = 0;

  

    
}; // struct IVaporIntrusionGUI

}
}

#endif // __USUL_INTERFACES_IVAPORINTRUSIONGUI_H__
