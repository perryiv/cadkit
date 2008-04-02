#ifndef _TRIANGLE_MODEL_TriangleReaderFieldViewAscii_H
#define _TRIANGLE_MODEL_TriangleReaderFieldViewAscii_H

#include "TriangleDocument.h"
#include "OsgTools/Triangles/TriangleSet.h"

#include "Usul/Types/Types.h"
#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector4.h"

#include "osg/Group"

#include <string>
#include <vector>

class TriangleReaderFieldViewAscii 
{
public:

  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef osg::ref_ptr< osg::Group > GroupPtr;
  typedef Usul::Math::Vec4d Vec4d;
  typedef Usul::Math::Vec2d Vec2d;
  typedef Usul::Math::Vec4d Color4;
  typedef std::vector < Vec4d > ColorTable;
  typedef std::vector < double > ColorValues;
  typedef TriangleDocument::ValidRefPtr TriangleDocumentPtr;
  typedef OsgTools::Triangles::TriangleSet::ValidRefPtr  TriangleSetPtr;
  typedef std::pair < std::string, Usul::Types::Uint64 > FileInfo;
  typedef std::pair < unsigned int, unsigned int > Progress;
  typedef std::vector< Usul::Math::Vec3d > Vertices;
  typedef OsgTools::Triangles::SharedVertex SharedVertex;
  typedef std::vector< unsigned int > Face;
  typedef std::vector< Face > Faces;

 // USUL_DECLARE_REF_POINTERS ( TriangleReaderFieldViewAscii );

  TriangleReaderFieldViewAscii( const std::string &file, IUnknown *caller, TriangleDocument *doc );
  virtual               ~TriangleReaderFieldViewAscii();

   // Clear accumulated state.
  void                clear();

  // Read the file.
  void                operator()();

protected:

  /// Do not copy.
  TriangleReaderFieldViewAscii ( const TriangleReaderFieldViewAscii & );
  TriangleReaderFieldViewAscii &operator = ( const TriangleReaderFieldViewAscii & );

  void                  _read( const std::string & filename );
  void                  _readColorTable( const std::string & filename );
  
  Color4                _getInterpolatedColorValue( double value );
  void                  _incrementProgress ( bool state );
  void                  _createTriangeSet();
  void                  _createMirroredTriangle( unsigned int i, osg::Vec4Array * colors );
  

private:
  GroupPtr              _root;
  ColorTable            _colorTable;
  ColorValues           _values;
  TriangleDocumentPtr   _document;
  Vec2d                 _colorRange;
  std::string           _filename;
  IUnknown::RefPtr      _caller;
  Progress              _progress;
  Vertices              _vertices;
  Vertices              _mirrorVertices;
  Faces                 _faces;

};

#endif

