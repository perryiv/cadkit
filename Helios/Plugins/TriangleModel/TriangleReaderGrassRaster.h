#ifndef __TriangleReaderGrassRaster_H__
#define __TriangleReaderGrassRaster_H__

//#include <string>

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Types/Types.h"

#include "OsgTools/Triangles/TriangleSet.h"
#include "Helios/Plugins/TriangleModel/TriangleDocument.h"

#include "XmlTree/Document.h"

class TriangleReaderGrassRaster : public Usul::Base::Referenced
{
public:

  typedef Usul::Base::Referenced BaseClass;
  typedef OsgTools::Triangles::TriangleSet::RefPtr TriangleSetPtr;
  typedef TriangleDocument::ValidRefPtr TriangleDocumentPtr;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef std::pair < std::string, Usul::Types::Uint64 > FileInfo;
  typedef std::pair < unsigned int, unsigned int > Progress;
  typedef osg::ref_ptr< osg::Vec3Array > Vec3ArrayPtr;

  // Data types
  typedef Usul::Types::Int16 Int16;
  typedef Usul::Types::Int8 Int8;
  typedef std::vector< Int8 > Int8Vec;
  typedef std::vector< Int16 > Int16Vec;
  typedef std::vector< float > FloatVec;

  struct Header
  { 
    unsigned int    proj;
    unsigned int    zone;
    double          north;
    double          south;
    double          east;
    double          west;
    unsigned int    cols;
    unsigned int    rows;
    double          ew_resol;
    double          ns_resol;
    int             format;
    int             compressed;
 };

  USUL_DECLARE_REF_POINTERS ( TriangleReaderGrassRaster );

  /// Usul::Interfaces::IUnknown members.
  //USUL_DECLARE_IUNKNOWN_MEMBERS;

  TriangleReaderGrassRaster ( const std::string &file, Usul::Interfaces::IUnknown *caller, TriangleDocument *doc  );
  virtual                   ~TriangleReaderGrassRaster();

  // Read the file.
  void                operator()();

  // Clear accumulated state.
  void                      clear();

  // Read the file.
  void                      read();

  // Build the scene
  osg::Node*                buildScene();


protected:

  

  void                      _read();
  void                      _readHeader();
  void                      _incrementProgress ( bool state, float i, float num );

  
  void                      _readXML();
  void                      _parseXML( XmlTree::Node &node, Usul::Interfaces::IUnknown *caller );


  template < class VectorType > void _makeTriangleDocument( VectorType vertices );
  void                      _loadTexture ( const std::string& filename, Usul::Interfaces::IUnknown *caller );

  void                      _stats();


private:
   // No copying.
  TriangleReaderGrassRaster ( const TriangleReaderGrassRaster & );
  TriangleReaderGrassRaster &operator = ( const TriangleReaderGrassRaster & );

  Header                    _header; 
  std::string               _dir;
  //TriangleSetPtr            _triangleSet;
  IUnknown::RefPtr          _caller;
  Progress                  _progress;
  FileInfo                  _file;
  TriangleDocumentPtr       _document;
  std::string               _xmlFilename;
  std::string               _textureFilename;

};

#endif // __TriangleReaderGrassRaster_H__
