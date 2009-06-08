
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

#ifndef __EXPERIMENTAL_POINT_DOCUMENT_OCTTREE_H__
#define __EXPERIMENTAL_POINT_DOCUMENT_OCTTREE_H__

#include "OctTreeNode.h"
#include "PointSetRecords.h"

#include "OsgTools/Configure/OSG.h"

#include "Usul/Base/Object.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Documents/Document.h"

#include "osg/BoundingBox"
#include "osg/Group"

#include "boost/shared_ptr.hpp"

#include <vector>

namespace Usul { namespace Jobs { class Manager; } }


class OctTree : public Usul::Base::Object
{
public:
  
  typedef Usul::Base::Object BaseClass;
  typedef osg::Vec3 Point;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef std::vector<char> StreamBuffer;
  typedef boost::shared_ptr<StreamBuffer> StreamBufferPtr;

  // Type information.
  USUL_DECLARE_TYPE_ID ( OctTree );

  USUL_DECLARE_REF_POINTERS ( OctTree );

  OctTree ( Usul::Jobs::Manager *jm );
  virtual ~OctTree();

  void                          add( OctTreeNode * node );
  bool                          insert( Point p );
  
  osg::Node*                    buildScene( Unknown *caller = 0x0, Unknown *progress = 0x0 );

  void                          buildVectors();

  void                          bounds( osg::BoundingBox bb );
  
  // Get/Set capacity level
  void                          capacity( unsigned int level );
  unsigned int                  capacity();

  void                          split( Usul::Documents::Document* document, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  void                          write( std::ofstream* ofs, Usul::Documents::Document* document, Unknown *caller = 0x0, Unknown *progress = 0x0 ) const;
  void                          read ( std::ifstream* ifs, Usul::Documents::Document* document, Unknown *caller = 0x0, Unknown *progress = 0x0 );
  void                          preBuildScene( Usul::Documents::Document* document, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  void                          workingDir( const std::string& dir, bool setNodes );
  std::string                   workingDir();

  void                          baseName( const std::string& name );
  std::string                   baseName();

protected:
  
  void                          _partition();
  osg::Node*                    _buildTransparentPlane();
  
private:

  // No copying or assignment.
  OctTree ( const OctTree & );
  OctTree &operator = ( const OctTree & );
  
  osg::ref_ptr< OctTreeNode >   _tree;
  unsigned int                  _capacity;
  StreamBufferPtr               _buffer;
  std::string                   _tempPath;
  std::string                   _workingDir;
  std::string                   _baseName;
  Usul::Jobs::Manager *         _jobManager;
};
#endif // __EXPERIMENTAL_POINT_DOCUMENT_OCTTREE_H__

