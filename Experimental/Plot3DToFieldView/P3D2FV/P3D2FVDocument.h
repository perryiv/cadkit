
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


class P3D2FVDocument : public Usul::Documents::Document,
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
  typedef osg::ref_ptr< osg::Group > GroupPtr;

  typedef std::vector< Usul::Math::Vec3ui > Vec3uiArray;
  typedef std::vector< Usul::Math::Vec3f > Grid1D;
  typedef std::vector< Grid1D > Grid2D;
  typedef std::vector< Grid2D > Grid3D;
  typedef std::vector< Grid3D > Grid4D;

 
  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( P3D2FVDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  P3D2FVDocument();

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
  virtual Filters             filtersOpen()   const;
  virtual Filters             filtersSave()   const;
  virtual Filters             filtersInsert() const;
  virtual Filters             filtersExport() const;
  
  /// Read the document.
  virtual void                read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  /// Write the document to given file name.
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0  ) const;
  
 

protected:

  /// Do not copy.
  P3D2FVDocument ( const P3D2FVDocument & );
  P3D2FVDocument &operator = ( const P3D2FVDocument & );

  /// Usul::Interfaces::IUpdateListener
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );

 
  // Read Point3D file
  void                        _read( const std::string& filename, Unknown *caller = 0x0, Unknown *progress = 0x0);
  void                        _initializeGrid( Usul::Types::Uint32 n );
  void                        _initializeGridAt( Usul::Types::Uint32 index, Usul::Math::Vec3ui size );
  void                        _setValueAt( Usul::Types::Uint32 t, Usul::Types::Uint32 i, Usul::Types::Uint32 j, Usul::Types::Uint32 k, Usul::Math::Vec3f value );
  Usul::Math::Vec3f           _getValueAt( Usul::Types::Uint32 t, Usul::Types::Uint32 i, Usul::Types::Uint32 j, Usul::Types::Uint32 k );


  /// Use reference counting.
  virtual ~P3D2FVDocument();

private:

  Usul::Types::Uint32         _numBlocks;
  Vec3uiArray                 _dimensions;
  Grid2D                      _grid;
  
};



#endif // _FIELD_VIEW_READER_DOCUMENT_H_
