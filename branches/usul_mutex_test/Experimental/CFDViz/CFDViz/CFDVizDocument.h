
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for mpd files
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CFD_VIZ_DOCUMENT_H_
#define _CFD_VIZ_DOCUMENT_H_

#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IAnimatePath.h"
#include "Usul/Interfaces/ICFDCommands.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Interfaces/IMpdNavigator.h"
#include "Usul/Interfaces/IMenuAdd.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector4.h"
#include "Usul/Policies/Update.h"

#include "OsgTools/Triangles/TriangleSet.h"
#include "OsgTools/Builders/Arrow.h"

#include "XmlTree/Document.h"

#include "osg/Image"
#include "osg/LineSegment"

#include <string>
#include <memory>

namespace osg { class Node; }


class CFDVizDocument :            public Usul::Documents::Document,
                                  public Usul::Interfaces::IBuildScene,
                                  public Usul::Interfaces::IUpdateListener,
                                  public Usul::Interfaces::IMenuAdd,
                                  public Usul::Interfaces::ICFDCommands

                                  
{
public:
  // Structs
  struct CFDPoint
  {
    Usul::Math::Vec3d pos;
    Usul::Math::Vec3d dir;
    double value;
  };
  struct Particle
  {
    Usul::Math::Vec3d pos;
    Usul::Math::Vec3d dir;
    unsigned int ttl;
  };
  struct Partition
  {
    osg::BoundingBox bb;
    unsigned int xmin, xmax, ymin, ymax, zmin, zmax;
  };

  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef Usul::Documents::Document Document;
  typedef Usul::Documents::Document::RefPtr DocumentPtr;
  typedef Usul::Documents::Manager DocManager;
  typedef DocManager::DocumentInfo Info;
  typedef Usul::Policies::NumberBased UpdatePolicy;
  typedef std::auto_ptr< UpdatePolicy > UpdatePolicyPtr;
  typedef osg::ref_ptr< osg::Group > GroupPtr;
  typedef osg::ref_ptr< osg::Image > Image;
  typedef Usul::Math::Vec2d Vec2d;
  typedef Usul::Math::Vec3d Vec3d;
  typedef Usul::Math::Vec3f Vec3f;
  typedef Usul::Math::Vec4d Color4;
  typedef std::vector< Color4 > ColorTable;
  typedef std::vector< Color4 > Color4Vec;
  typedef std::vector< double > ColorValues;
  typedef std::vector< unsigned char > ColorVec;
  typedef std::vector< CFDPoint > Vertices;
  typedef osg::ref_ptr< osg::Vec3Array > Vec3Array;
  typedef std::vector < Vec3Array > Streamlines;
  typedef osg::ref_ptr< osg::LineSegment > LineSegmentPtr;
  typedef osg::BoundingBox BoundingBox;
  typedef osg::ref_ptr< osg::MatrixTransform > MatrixTransformPtr;
  typedef std::pair < unsigned int, unsigned int > Progress;
  typedef OsgTools::Builders::Arrow Arrow;
  typedef OsgTools::Ray Ray;
  typedef std::vector< Particle > Particles;
  typedef std::vector< Partition > Partitions;
  
  
  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( CFDVizDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  CFDVizDocument();
  
  //Usul::Interfaces::IMenuAdd
  void                        menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller = 0x0 );

  /// Build the scene.
  virtual osg::Node *         buildScene ( const BaseClass::Options &options, Unknown *caller = 0x0 );

  /// Return true if this document can do it.
  virtual bool                canExport ( const std::string &file ) const;
  virtual bool                canInsert ( const std::string &file ) const;
  virtual bool                canOpen   ( const std::string &file ) const;
  virtual bool                canSave  ( const std::string &file ) const;

  /// Clear any existing data.
  virtual void                clear ( Unknown *caller = 0x0 );

  /// Get the filters that correspond to what this document can read and write.
  virtual Filters  filtersOpen()   const;
  virtual Filters  filtersSave()   const;
  virtual Filters  filtersInsert() const;
  virtual Filters  filtersExport() const;
  
  /// Read the document.
  virtual void     read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  /// Write the document to given file name.
  virtual void     write ( const std::string &filename, Unknown *caller = 0x0  ) const;
  
 
  // Usul::Interfaces::ICFDCommand
  void              animation ( bool state );
  bool              animation ();

protected:

  /// Do not copy.
  CFDVizDocument ( const CFDVizDocument & );
  CFDVizDocument &operator = ( const CFDVizDocument & );

  /// Usul::Interfaces::IUpdateListener
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );

  void                        _openDocument ( const std::string &file, Usul::Documents::Document *document, Usul::Interfaces::IUnknown *caller );
  void                        _setStatusBar ( const std::string &text, Usul::Interfaces::IUnknown *caller );
  void                        _incrementProgress ( bool state, Unknown *progress = 0x0 );

  // Scene Builders
  void                        _buildScene( Unknown *caller );
  osg::Node*                  _buildVectorArrows( bool drawMirror, double arrowSize, Unknown *caller );
    
  //Reads
  void                        _readVertices( const std::string& filename, Unknown *progress = 0x0 );
  void                        _readColorTable( const std::string& filename, Unknown *progress = 0x0 );
  void                        _readGridFile( const std::string & filename, Unknown *progress = 0x0 );

  void                        _setupEnvironment();
  Color4                      _getInterpolatedColorValue( double value );
  osg::Vec4Array *            _getColorArray( bool drawMirror );
  CFDPoint                    _getVertexAt( unsigned int i, unsigned int j, unsigned int k );

  // CFD animation and particle movement
  void                        _moveParticles( Unknown *caller );
  Vec3d                       _getDirectionAt( Vec3d pos );
  Vec3d                       _getDirectionAt( unsigned int i, unsigned int j, unsigned int k, Vec3d pos );
  Vec3d                       _getDirectionAt2( Vec3d pos, Unknown *caller );
  bool                        _contains( unsigned int i, unsigned int j, unsigned int k, Vec3d pos );
  bool                        _getPartition( Vec2d &iRange, Vec2d &jRange, Vec2d &kRange, Vec3d pos );
  void                        _partition();
  osg::Node *                 _drawParticles( Unknown *caller );
  osg::Node *                 _drawGrid( Unknown *caller );
  void                        _initParticles();
  double                      _determinant( Vec3d a, Vec3d b, Vec3d c );
  bool                        _rayQuadIntersect( Vec3d a, Vec3d b, Vec3d c, Vec3d d, Vec3d p );

  /// Use reference counting.
  virtual ~CFDVizDocument();

private:
  GroupPtr                      _root;
  GroupPtr                      _particleScene;
  Vertices                      _vertices;
  Progress                      _progress;
  Vec2d                         _colorRange;
  ColorTable                    _colorTable;
  ColorValues                   _values;
  bool                          _firstPass;    
  Usul::Math::Vec3ui            _gridResolution;
  Particles                     _particles;    
  Usul::Policies::NumberBased   _update;
  bool                          _isAnimating;
  Vec3Array                     _streamLines;
  Partitions                    _partitions;
  
};



#endif // _CFD_VIZ_DOCUMENT_H_
