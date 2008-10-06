///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.htm
//
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __EXPERIMENTAL_POINT_DOCUMENT_OCTTREENODE_H__
#define __EXPERIMENTAL_POINT_DOCUMENT_OCTTREENODE_H__

#include "PointSetRecords.h"

#include "Usul/Pointers/Pointers.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Documents/Document.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include "Usul/Math/Vector3.h"
#include "Usul/Types/Types.h"

#include "osg/BoundingBox"
#include "osg/Vec3"
#include "osg/Vec4"
#include "osg/Node"
#include "osg/PrimitiveSet"
#include "osg/Group"

#include "boost/shared_ptr.hpp"

#include <vector>
#include <list>
#include <iosfwd>

#define UNDEFINED_NODE 0
#define NODE_HOLDER 1
#define POINT_HOLDER 2

namespace Usul { namespace Jobs { class Manager; } }
namespace osgUtil { class CullVisitor; }

class OctTreeNode : public osg::Group
{
public:

  USUL_DECLARE_REF_POINTERS ( OctTreeNode );

  typedef Usul::Interfaces::IUnknown Unknown;
 
  typedef osg::Vec3f Point;
  typedef osg::ref_ptr< osg::Vec3Array > Points;
  typedef osg::Group BaseClass;
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

  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;

   // Type information.
  USUL_DECLARE_TYPE_ID ( OctTreeNode );

  OctTreeNode ( Usul::Jobs::Manager *, StreamBufferPtr, const std::string &tempPath ); 
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
  void                              createLodLevels();
  

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

  void                              workingDir( const std::string& dir );
  std::string                       workingDir();

  void                              baseName( const std::string& name );
  std::string                       baseName();

  // Traverse the children.
  virtual void                      traverse ( osg::NodeVisitor & );

  std::string                       name();
  void                              name( const std::string& n );

  // Return the mutex. Use with caution.
  Mutex &                           mutex() const;

  // Get/Set Node Depth
  Usul::Types::Uint32               getNodeDepth();
  void                              setNodeDepth( Usul::Types::Uint32 d );

  // Get/Set Tree Depth
  Usul::Types::Uint32               getTreeDepth();
  void                              setTreeDepth( Usul::Types::Uint32 d );



protected:

  void                              _writePoints( std::ofstream* ofs, Usul::Documents::Document* document, Unknown *caller = 0x0, Unknown *progress = 0x0 ) const;
  void                              _writeNodeInfo( std::ofstream* ofs ) const;
  void                              _writeRecord( std::ofstream* ofs, Usul::Types::Uint32 type, Usul::Types::Uint64 size ) const;
  void                              _writeOOCNodeInfo( std::ofstream* ofs ) const;

  void                              _readNodeInfo( std::ifstream* ifs );
  void                              _readPoints( std::ifstream* ifs );
  Usul::Types::Uint64               _readToRecord( std::ifstream* ifs, Usul::Types::Uint32 type );
  void                              _readOctreeRecord ( std::ifstream* ifs, Usul::Documents::Document* document, Unknown *caller = 0x0, Unknown *progress = 0x0 );
  std::string                       _readOOCNodeInfo( std::ifstream* ifs );
  

  bool                              _contains( OctTreeNode::Point p );
  void                              _insertOrCreateChildren( OctTreeNode::Point p );
  void                              _reorder();
  void                              _createChildren();
  void                              _split();
  bool                              _addCellToChild( OctTreeNode::Point p );
  
  void                              _openTempFileForRead();
  void                              _openTempFileForWrite();

  std::ifstream&                    _getInputStream();
  std::ofstream&                    _getOutputStream();

  void                              _closeInputStream();
  void                              _closeOutputStream();

  void                              _closeChildrenStreams();
  Usul::Types::Uint32               _headerSize() const;

  void                              _createLodLevels();
  unsigned int                      _getLodLevel( const osg::Vec3d& eye );
  osg::Node*                        _buildNode( Unknown *caller = 0x0, Unknown *progress = 0x0 );

  osg::LOD*                         _getLod();


private:

  // No copying or assignment.
  OctTreeNode ( const OctTreeNode & );
  OctTreeNode &operator = ( const OctTreeNode & );

  osg::BoundingBox                _bb;
  Children                        _children;
  Points                          _points;
  unsigned int                    _type;
  unsigned int                    _capacity;
  std::string                     _name;
  unsigned int                    _currentLodLevel;

  bool                            _useLOD;

  LodDefinitions                  _lodDefinitions;

  std::ifstream*                  _infile;
  std::ofstream*                  _outfile;
  std::string                     _tempFilename;
  Usul::Types::Uint64             _numPoints;
  StreamBufferPtr                 _streamBuffer;
  std::string                     _tempPath;
  double                          _distance;
  osg::Vec4f                      _material;
  
  static long                     _streamCount;
  static unsigned long            _numerator;
  static unsigned long            _denominator;
  static Usul::Types::Uint64      _numLeafNodes;

  mutable Mutex                   _mutex;
  std::string                     _workingDir;
  std::string                     _baseName;
  Usul::Jobs::Manager *           _jobManager;

  static Usul::Types::Uint32      _treeDepth;
  static Usul::Types::Uint32      _depthCount;
  Usul::Types::Uint32             _nodeDepth;

};
#endif // __EXPERIMENTAL_POINT_DOCUMENT_OCTTREENODE_H__

