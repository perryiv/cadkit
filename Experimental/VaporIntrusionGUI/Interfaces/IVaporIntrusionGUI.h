
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
    SIZEOF_EQUAL
  };

  // structs

  struct GridRefinement
  {
    std::string axis, start, end, value;

    GridRefinement(): axis( "x" ), start( "0" ), end( "0" ), value( "0" ) {};
    GridRefinement( const std::string& a, const std::string& s, const std::string& e, const std::string& v ): 
    axis( a ), start( s ), end( e ), value( v ) {};
  };
  typedef std::vector< GridRefinement > GridRefinements;

  struct Crack
  {
    std::string x1, x2, y1, y2, w;

    Crack(): x1(), x2(), y1(), y2(), w() {};
    Crack( const std::string& sx, const std::string& ex, const std::string& sy, const std::string& ey, const std::string& width ):
    x1( sx ), x2( ex ), y1( sy ), y2( ey ), w( width ) {};
  };
  typedef std::vector< Crack > Cracks;

  struct Building
  {
    std::string l,w,h;
    std::string x,y,z;
    std::string v, xrate, thickness;

    Building( const std::string& length, const std::string& width, const std::string& height,
              const std::string& xpos, const std::string& ypos, const std::string& zpos, const std::string& vol,
              const std::string& xr, const std::string& th ):
    l ( length ), w ( width ), h ( height ), x ( xpos ), y ( ypos ), z ( zpos ),
    v ( vol ), xrate ( xr ), thickness( th )
    {};

    Building():
    l ( "1" ), w ( "1" ), h ( "1" ), x ( "0" ), y ( "0" ), z ( "0" ), v(  "1" ), xrate ( "" ), thickness( "" )
    {};
  };
  
  struct Soil
  {
    std::string name, elevation, porosity, waterPorosity, organicCarbon, permeability, viscosity;
    Usul::Math::Vec4f color;

    Soil(): name(), elevation(), porosity(), waterPorosity(), organicCarbon(), permeability(), viscosity() {};

    // Soil c ( name, elevation, porosity, h2oPorosity, organicC, perm, viscosity );
    Soil( const std::string& n, const std::string& elev, const std::string& p, const std::string& wp,
      const std::string& oc, const std::string& perm, const std::string& v ):
    name( n ), elevation( elev ), porosity( p ), waterPorosity( wp ), organicCarbon( oc ), permeability( perm ), viscosity( v ) {};

  };
  typedef std::vector< Soil > Soils;

  struct Contaminant
  {
    unsigned int index;
    std::string name, henry, koc, airDiff, waterDiff, atmoConc;
    
    Contaminant():
    index( 0 ), name(), henry(), koc(), airDiff(), waterDiff(), atmoConc()
    {};

    Contaminant( unsigned int i, const std::string& n, const std::string& h, const std::string& k,
                 const std::string& ad, const std::string& wd, const std::string& ac ):
    index( i ), name( n ), henry( h ), koc( k ), airDiff( ad ), waterDiff( wd ), atmoConc( ac )
    {};

  };
  typedef std::vector< Contaminant > Contaminants;

  struct Source
  {
    std::string l,w,h;
    std::string x,y,z;
    std::string name;
    Contaminants contaminants;
    Usul::Math::Vec4f color;

    Source():
    l ( "1" ), w ( "1" ), h ( "1" ), x ( "0" ), y ( "0" ), z ( "0" ), name ( "" ), contaminants()
    {};

    Source( const std::string& length, const std::string& width, const std::string& height,
                 const std::string& xpos, const std::string& ypos, const std::string& zpos,
                 const std::string& n, std::vector< Contaminant > c ):
    l ( length ), w ( width ), h ( height ), x ( xpos ), y ( ypos ), z ( zpos ), name( n ), contaminants( c )
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

  // Get/Set the material
  virtual void                  setMaterial( unsigned int x, unsigned int y, unsigned int z, Usul::Math::Vec4f c ) = 0;
  virtual Usul::Math::Vec4f     getMaterial( unsigned int x, unsigned int y, unsigned int z ) = 0;

  // Request redraw
  virtual void                  requestRedraw() = 0;

  virtual void                  setValueAt( unsigned int x, unsigned int y, unsigned int z, unsigned int index, const std::string& value ) = 0;
  virtual void                  setNameAt( unsigned int x, unsigned int y, unsigned int z, unsigned int index ,  const std::string& name ) = 0;
  virtual void                  addValueAt( unsigned int x, unsigned int y, unsigned int z, const std::string& name, const std::string& value ) = 0;
  virtual void                  addValueAt( unsigned int x, unsigned int y, unsigned int z, const std::string& value ) = 0;

  typedef std::vector< std::pair< double, double > > GridPoints;

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

  virtual void                  contaminants( Contaminants c ) = 0;
  virtual Contaminants          contaminants() = 0;

  virtual void                  soils( Soils s ) = 0;
  virtual Soils                 soils() = 0;

  virtual void                  cracks( Cracks c ) = 0;
  virtual Cracks                cracks() = 0;
  
  virtual void                  refinements( GridRefinements gr ) = 0;
  virtual GridRefinements       refinements() = 0;

  virtual bool                  symmetricalGrid() = 0;
  virtual void                  symmetricalGrid( bool value ) = 0;
    
}; // struct IVaporIntrusionGUI

}
}

#endif // __USUL_INTERFACES_IVAPORINTRUSIONGUI_H__
