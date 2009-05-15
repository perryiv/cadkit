
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IVAPORINTRUSIONGUI_H__
#define __USUL_INTERFACES_IVAPORINTRUSIONGUI_H__

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector4.h"

namespace Usul {
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
    LESS_THAN_OR_EQUAL
  };

  // structs
  struct InputColumn
  {
    // #Name,Value,Description,Type,Activators
    std::string name;
    std::string value;
    std::string description;
    std::string type;
    ActivatorPairs activators;
    bool active;

    InputColumn():
      name (),
      value(),
      description(),
      type(),
      activators(),
      active( false )
      {}
    InputColumn( const std::string& n, const std::string& v, const std::string& d, const std::string& t ) :
      name ( n ),
      value( v ),
      description( d ),
      type( t ),
      activators(),
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

  //virtual void                  readParamaterFile( const std::string& filename ) = 0;
  virtual ParameterList         parameters() = 0;
  virtual void                  parameters( ParameterList plist ) = 0;

  virtual Categories            categories() = 0;
  virtual void                  categories( Categories c ) = 0;

  virtual void                  initialize() = 0;

  virtual void                  readConfigFile( const std::string& name, const std::string& filename ) = 0;
  virtual void                  updateCategory( Category category ) = 0;

  virtual bool                  checkValue( int comparitor, const std::string& value1, const std::string& value2 ) = 0;
  
    
}; // struct IVaporIntrusionGUI

}
}

#endif // __USUL_INTERFACES_IVAPORINTRUSIONGUI_H__
