
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
//  Document for mpd files
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FIELD_VIEW_READER_DOCUMENT_H_
#define _FIELD_VIEW_READER_DOCUMENT_H_

#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IAnimatePath.h"
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

#include "XmlTree/Document.h"

#include "osg/Image"
#include "osg/LineSegment"

#include <string>
#include <memory>

namespace osg { class Node; }


class FieldViewReaderDocument : public Usul::Documents::Document,
                                  public Usul::Interfaces::IBuildScene,
                                  public Usul::Interfaces::IUpdateListener
                                  
{
public:
  // Structs
    struct CFDPoint
  {
    Usul::Math::Vec3d pos;
    Usul::Math::Vec3d dir;
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
  typedef std::vector< unsigned char > ColorVec;
  typedef std::vector< double > ColorValues;
  typedef std::vector< CFDPoint > Vertices;
  typedef osg::ref_ptr< osg::LineSegment > LineSegmentPtr;
  typedef osg::BoundingBox BoundingBox;
  typedef osg::ref_ptr< osg::MatrixTransform > MatrixTransformPtr;
  typedef std::pair < unsigned int, unsigned int > Progress;

 
  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( FieldViewReaderDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  FieldViewReaderDocument();

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
  virtual void     write ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0  ) const;
  
 

protected:

  /// Do not copy.
  FieldViewReaderDocument ( const FieldViewReaderDocument & );
  FieldViewReaderDocument &operator = ( const FieldViewReaderDocument & );

  /// Usul::Interfaces::IUpdateListener
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );

  void                        _openDocument ( const std::string &file, Usul::Documents::Document *document, Usul::Interfaces::IUnknown *caller );
  void                        _setStatusBar ( const std::string &text, Usul::Interfaces::IUnknown *caller );
  void                        _incrementProgress ( bool state, Unknown *progress = 0x0 );

  osg::Vec4Array *            _getColorArray( bool drawMirror );
  Color4                      _getInterpolatedColorValue( double value );

  // Scene Builders
  void                        _buildScene( Unknown *caller );
  osg::Node*                  _buildWindow( Unknown *caller );
  osg::Node*                  _buildVectorArrows( bool drawMirror, double arrowSize, Unknown *caller );
    
  //Reads
  void                        _readVertices( const std::string& filename, Unknown *progress = 0x0 );
  void                        _readColorTable( const std::string& filename, Unknown *progress = 0x0 );
  void                        _init();
  void                        _setupEnvironment();

  /// Use reference counting.
  virtual ~FieldViewReaderDocument();

private:
  GroupPtr                    _root;
  Vertices                    _vertices;
  Progress                    _progress;

  Vec2d                       _colorRange;
  ColorTable                  _colorTable;
  ColorValues                 _values;

  std::string                 _filename;
  bool                        _firstPass; 
    
  
};



#endif // _FIELD_VIEW_READER_DOCUMENT_H_
