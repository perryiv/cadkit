
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Jeff Conner, Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Components/Manager.h"
#include "Usul/Documents/Manager.h"

#include <osg/Vec3>

#include <vector>
#include <string>
#include <list>

class ToFVUNS : Usul::Base::Referenced,
                Usul::Interfaces::IProgressBar
{
public:

  typedef Usul::Base::Referenced BaseClass;
  typedef std::vector<std::string> Files;
  typedef std::list<std::string> Args;

  typedef std::vector< Usul::Math::Vec3ui > Vec3uiArray;
  typedef Usul::Types::Float64 GridType;
  typedef std::vector< GridType > Grid1D;
  typedef std::vector< Grid1D > Grid2D;
  typedef std::vector< Grid2D > GridSpace;

  typedef Usul::Types::Uint32 IndexValue;
  typedef std::vector < std::vector < std::vector < IndexValue > > > IndexGrid;
  
  typedef Usul::Types::Float32 Float32;
  typedef std::vector< Float32 > FloatVector;


  USUL_DECLARE_QUERY_POINTERS ( ToFVUNS );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  ToFVUNS ( int argc, char **argv );

  void          run();

protected:

  // Usul::Interfaces::IProgressBar
  virtual void  showProgressBar(){}
  virtual void  setTotalProgressBar ( unsigned int value ){}
  virtual void  updateProgressBar ( unsigned int value );
  virtual void  hideProgressBar(){}

  void          _processArguments();
  void          _processFiles();
  void          _processFile ( const std::string &file, Grid2D &grid );
  void          _procesGridFile( const std::string &file, Grid1D &grid );

  void          _writeResultsFile( const std::string &name );
  void          _writeResults( const std::string &name, std::ofstream* ofs );
  void          _writeGridFile( const std::string &name );
  void          _writeElementNodes( std::ofstream* ofs );
  void          _writeElementNodeHeader( std::ofstream* ofs );

  //Debug method to write the results in ascii for visual validation
  void          _writeDebugFile( const std::string& filename );

  // write an osg file for debugging
  void          _writeOsgSlice( unsigned int y, const std::string& filename );
  void          _findMinMax( float& min, float& max );
  osg::Vec3     _getInterpolatedValue( unsigned int x, unsigned int y, unsigned int z, float min, float max );
  void          _intializeColorRamp();

  virtual ~ToFVUNS();

private:

  void          _loadPlugin ( const std::string &file );

  void          _openDocument ( const std::string &file, Usul::Documents::Document *document );

  

  void          _saveDocument ( const std::string &file, Usul::Documents::Document *document );

 

  Args                        _args;
  Files                       _files;

  Usul::Math::Vec3ui          _dimensions;
  GridSpace                   _values;
  std::string                 _workingDirectory;
  std::string                 _baseFilename;
  std::vector< Grid1D >       _grid;
  IndexGrid                   _indexGrid;
  FloatVector                 _xValues;
  FloatVector                 _yValues;
  FloatVector                 _zValues;
  Vec3uiArray                _colorRamp;
  
};
