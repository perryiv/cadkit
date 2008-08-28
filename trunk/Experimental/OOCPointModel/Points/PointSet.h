
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
//  Point Tool Class
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _POINT_DOCUMENT_POINTS_POINT_SET_H_
#define _POINT_DOCUMENT_POINTS_POINT_SET_H_

#include "OctTree.h"
#include "PointSetRecords.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Documents/Document.h"

#include "osg/Vec3"
#include "osg/Group"
#include "osg/Geometry"
#include "osg/Geode"

#include <string>

namespace Usul { namespace Jobs { class Manager; } }
namespace osg { class Node; class Group; }

class PointSet : public Usul::Base::Object
{
public:
 
  /// Useful typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  

  // Data Structor Typedefs
  
  // OSG typedefs
  typedef osg::ref_ptr< osg::Group > GroupPtr;
  typedef osg::ref_ptr< osg::Geode > GeodePtr;
  typedef osg::ref_ptr< osg::Geometry > GeometryPtr;
  typedef osg::ref_ptr< osg::Vec3Array > Points;

  // Type information.
  USUL_DECLARE_TYPE_ID ( PointSet );
 
  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( PointSet );

  /// Construction.
  PointSet ( Usul::Jobs::Manager * );

  /// Build the scene.
  osg::Node *             buildScene ( Unknown *caller = 0x0 );
  void                    preBuildScene( Usul::Documents::Document* document, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  /// Clear any existing data.
  virtual void            clear ( Unknown *caller = 0x0 );
  
  bool                    addPoint( float x, float y, float z );
  bool                    addPoint( double x, double y, double z );
  bool                    addPoint( osg::Vec3 p );

  void                    allocate( unsigned int num );
  void                    bounds( osg::Vec3f min, osg::Vec3f max );

  void                    capacity( unsigned int t );

  void                    buildVectors();

  void                    split( Usul::Documents::Document* document, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  void                    write( std::ofstream* ofs, Usul::Types::Uint64  numPoints, Usul::Documents::Document* document = 0x0, Unknown *caller = 0x0, Unknown *progress = 0x0 ) const;
  void                    read ( std::ifstream* ifs, Usul::Types::Uint64 &numPoints,Usul::Documents::Document* document, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  void                    workingDir( const std::string& dir );
  std::string             workingDir();

  void                    baseName( const std::string& name );
  std::string             baseName();

 
protected:

  /// Use reference counting.
  virtual ~PointSet();

private:
  
  /// Do not copy.
  PointSet ( const PointSet & );
  PointSet &operator = ( const PointSet & );
  
  GroupPtr                _root;
  Points                  _points;
  OctTree::RefPtr         _tree;
  std::string             _workingDir;
  std::string             _baseName;
  Usul::Jobs::Manager *   _jobManager;
};


#endif // _OSGTOOLS_POINTS_POINT_SET_H_
