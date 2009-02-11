
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
//
//  For use with CFDViz for spatial partitioning of Computational Fluid
//  Dynamics visualization.  Will generalize at a later date when 
//  proof of concept is working.
//
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __EXPERIMENTAL_OCTTREENODE_H__
#define __EXPERIMENTAL_OCTTREENODE_H__

#include "OsgTools/Export.h"
#include "OsgTools/Points/PointSetRecords.h"

#include "Usul/Base/Object.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Documents/Document.h"

#include "Usul/Math/Vector3.h"
#include "Usul/Types/Types.h"

#include "osg/BoundingBox"
#include "osg/Vec3"
#include "osg/Vec4"
#include "osg/Node"
#include "osg/PrimitiveSet"

#include "boost/shared_ptr.hpp"

#include <vector>
#include <list>
#include <iosfwd>

#define UNDEFINED_NODE 0
#define NODE_HOLDER 1
#define POINT_HOLDER 2

namespace OsgTools {
namespace Points {

class OSG_TOOLS_EXPORT OctTreeNode : public Usul::Base::Object
{
public:

  USUL_DECLARE_REF_POINTERS ( OctTreeNode );

  typedef Usul::Interfaces::IUnknown Unknown;
 
  typedef osg::Vec3f Point;
  typedef osg::ref_ptr< osg::Vec3Array > Points;
  typedef Usul::Base::Object BaseClass;
  typedef OctTreeNode::RefPtr OctTreeNodePtr;
  typedef std::vector< OctTreeNodePtr > Children;
  typedef std::vector< osg::BoundingBox > BoundingBoxVec;
  typedef Usul::Math::Vec3d Vec3d;
  typedef std::list< osg::Vec3f > LinkedList;
  typedef osg::ref_ptr< osg::PrimitiveSet > PrimitiveSetPtr;
  typedef osg::DrawElementsUShort Elements;
  typedef osg::ref_ptr < Elements > ElementsPtr;
  typedef std::vector<char> StreamBuffer;
  typedef boost::shared_ptr<StreamBuffer> StreamBufferPtr;
  typedef std::vector< Usul::Types::Uint16 > LodDefinitions;
  typedef osg::ref_ptr< osg::Group > GroupPtr;

   // Type information.
  USUL_DECLARE_TYPE_ID ( OctTreeNode );

  OctTreeNode ( StreamBufferPtr, const std::string &tempPath ); 
  virtual ~OctTreeNode();

  // Getters
  osg::BoundingBox                  boundingBox() const;
  Children                          children() const;
  unsigned int                      type() const;
  unsigned int                      capacity() const;
 
  // Setters
  void                              boundingBox( const osg::BoundingBox &bb );
  void                              type( unsigned int type );
  void                              capacity( unsigned int level );
  bool                              add( Point p );

  void                              buildVectors();

  osg::Node*                        buildScene( Unknown *caller = 0x0, Unknown *progress = 0x0 );
  void                              preBuildScene( Usul::Documents::Document* document, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  void                              useLOD( bool value );

  void                              distance( double d );
  double                            getBoundingRadius() const;

  void                              closeInputStream();
  void                              closeOutputStream();

  void                              initSplitProgress( unsigned long n, unsigned long d );
  Usul::Types::Uint64               getNumPoints();
  Usul::Types::Uint64               numLeafNodes() const;
  LodDefinitions                    getLodDefinitions() const;
  void                              numLeafNodes( Usul::Types::Uint64 num );
  void                              lodDefinitions( LodDefinitions lods );

  bool                              split( Usul::Documents::Document* document, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  void                              write( std::ofstream* ofs, unsigned int numerator, unsigned int denominator, Usul::Documents::Document* document, Unknown *caller = 0x0, Unknown *progress = 0x0 ) const;
  void                              read ( std::ifstream* ifs, Usul::Documents::Document* document, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  

protected:

  void                              _writePoints( std::ofstream* ofs, Usul::Documents::Document* document, Unknown *caller = 0x0, Unknown *progress = 0x0 ) const;
  void                              _writeNodeInfo( std::ofstream* ofs ) const;
  void                              _writeRecord( std::ofstream* ofs, Usul::Types::Uint32 type, Usul::Types::Uint64 size ) const;

  void                              _readNodeInfo( std::ifstream* ifs );
  void                              _readPoints( std::ifstream* ifs );
  Usul::Types::Uint64               _readToRecord( std::ifstream* ifs, Usul::Types::Uint32 type );
  void                              _readOctreeRecord ( std::ifstream* ifs, Usul::Documents::Document* document, Unknown *caller = 0x0, Unknown *progress = 0x0 );


  bool                              _contains( OctTreeNode::Point p );
  void                              _insertOrCreateChildren( OctTreeNode::Point p );
  void                              _reorder();
  void                              _createChildren();
  void                              _split();
  bool                              _addCellToChild( OctTreeNode::Point p );
  bool                              _rayQuadIntersect( Vec3d a, Vec3d b, Vec3d c, Vec3d d, Vec3d p );
  
  

  void                              _openTempFileForRead();
  void                              _openTempFileForWrite();

  std::ifstream&                    _getInputStream();
  std::ofstream&                    _getOutputStream();

  void                              _closeInputStream();
  void                              _closeOutputStream();

  void                              _closeChildrenStreams();
  Usul::Types::Uint32               _headerSize() const;

private:

  osg::BoundingBox                _bb;
  Children                        _children;
  Points                          _points;
  unsigned int                    _type;
  unsigned int                    _capacity;

  

  bool                            _useLOD;

  LodDefinitions                  _lodDefinitions;

  std::ifstream*                  _infile;
  std::ofstream*                  _outfile;
  std::string                     _tempFilename;
  Usul::Types::Uint64             _numPoints;
  StreamBufferPtr                 _streamBuffer;
  std::string                     _tempPath;
  double                          _distance;
  GroupPtr                        _root;
  osg::Vec4f                      _material;
  
  static long                     _streamCount;
  static unsigned long            _numerator;
  static unsigned long            _denominator;
  static Usul::Types::Uint64      _numLeafNodes;

  

}; // OctTreeNode
}; // namespace Points
}; // namespace OsgTools

#endif // __EXPERIMENTAL_OCTTREENODE_H__
