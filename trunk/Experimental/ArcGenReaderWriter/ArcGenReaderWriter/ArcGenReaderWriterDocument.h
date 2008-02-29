
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Polyline format example
//
//
//  Format:
//  -------------------
//  ID,FIELD,FIELD
//  id,value,value
//  x,y
//  x,y
//  END
//
//  Example:
//  --------------------
//  ID,Street,Streettype
//  1,Church,Street
//  34.5,-14.3
//  12.8,-19.6
//  END
//  
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for mpd files
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _ARC_GEN_READER_WRITER_DOCUMENT_H_
#define _ARC_GEN_READER_WRITER_DOCUMENT_H_

#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IAnimatePath.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Interfaces/IMpdNavigator.h"
#include "Usul/Interfaces/IMenuAdd.h"
#include "Usul/Interfaces/IPlanetCoordinates.h"
#include "Usul/Interfaces/IArcGenReaderWriter.h"
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


class ArcGenReaderWriterDocument : public Usul::Documents::Document,
                                  public Usul::Interfaces::IBuildScene,
                                  public Usul::Interfaces::IUpdateListener,
                                  public Usul::Interfaces::IArcGenReaderWriter
                                  
{
public:
  // Structs
  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef Usul::Documents::Document Document;
  typedef Usul::Documents::Document::RefPtr DocumentPtr;
  typedef Usul::Documents::Manager DocManager;
  typedef Usul::Interfaces::IPlanetCoordinates IPlanetCoordinates;
  typedef DocManager::DocumentInfo Info;
  typedef Usul::Policies::NumberBased UpdatePolicy;
  typedef std::auto_ptr< UpdatePolicy > UpdatePolicyPtr;
  typedef osg::ref_ptr< osg::Group > GroupPtr;
  typedef osg::ref_ptr< osg::Image > Image;
  typedef Usul::Math::Vec2d Vec2d;
  typedef Usul::Math::Vec3d Vec3d;
  typedef Usul::Math::Vec3f Vec3f;
  typedef std::vector< unsigned int > Vertices;
  typedef std::vector< osg::Vec3 > Positions;

 
  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ArcGenReaderWriterDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  ArcGenReaderWriterDocument();

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

  // set the positions vector
  virtual void             setPolyLineVertices ( Positions p );

  // set the recorded measurement value
  virtual void             measurement( double m );

protected:

  /// Do not copy.
  ArcGenReaderWriterDocument ( const ArcGenReaderWriterDocument & );
  ArcGenReaderWriterDocument &operator = ( const ArcGenReaderWriterDocument & );

  /// Usul::Interfaces::IUpdateListener
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );

  void                        _openDocument ( const std::string &file, Usul::Documents::Document *document, Usul::Interfaces::IUnknown *caller );

  // Scene Builders
  void                        _buildScene( Unknown *caller );

  // Writers
  void                        _writePolylineZ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 ) const;
    
  //Reads

  /// Use reference counting.
  virtual ~ArcGenReaderWriterDocument();

private:
  GroupPtr                    _root;
  Positions                   _positions;
  double                      _measurement;
    
  
};



#endif // _ARC_GEN_READER_WRITER_DOCUMENT_H_
