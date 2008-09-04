
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

#ifndef _VAPOR_INTRUSION_GUI_DOCUMENT_H_
#define _VAPOR_INTRUSION_GUI_DOCUMENT_H_

#include "Usul/Documents/Document.h"
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


#include "XmlTree/Document.h"

#include "osg/Image"
#include "osg/LineSegment"
#include "osg/Group"
#include "osg/Material"

#include <string>
#include <memory>

namespace osg { class Node; }


class VaporIntrusionGUIDocument : public Usul::Documents::Document,
                                  public Usul::Interfaces::IBuildScene,
                                  public Usul::Interfaces::IUpdateListener
                                  
{
public:

  
  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef Usul::Documents::Document Document;
  typedef Usul::Documents::Document::RefPtr DocumentPtr;
  typedef Usul::Documents::Manager DocManager;
  typedef DocManager::DocumentInfo Info;
  typedef osg::ref_ptr< osg::Group > GroupPtr;
  typedef osg::Vec4 Color;
  typedef osg::Vec4 ValueType;
  typedef std::pair< ValueType, GroupPtr > Cube;
  typedef std::vector< std::vector< std::vector< Cube > > > Cubes;


  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( VaporIntrusionGUIDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  VaporIntrusionGUIDocument();

  /// Build the scene.
  virtual osg::Node *         buildScene ( const BaseClass::Options &options, Unknown *caller = 0x0 );

  /// Return true if this document can do it.
  virtual bool                canExport ( const std::string &file ) const;
  virtual bool                canInsert ( const std::string &file ) const;
  virtual bool                canOpen   ( const std::string &file ) const;
  virtual bool                canSave  ( const std::string &file ) const;

  /// Get the filters that correspond to what this document can read and write.
  virtual Filters             filtersOpen()   const;
  virtual Filters             filtersSave()   const;
  virtual Filters             filtersInsert() const;
  virtual Filters             filtersExport() const;
  
  /// Read the document.
  virtual void                read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  /// Write the document to given file name.
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0  ) const;

  /// Clear any existing data.
  virtual void                clear ( Unknown *caller = 0x0 );

protected:

  /// Do not copy.
  VaporIntrusionGUIDocument ( const VaporIntrusionGUIDocument & );
  VaporIntrusionGUIDocument &operator = ( const VaporIntrusionGUIDocument & );

  /// Use reference counting.
  virtual ~VaporIntrusionGUIDocument();

  /// Usul::Interfaces::IUpdateListener
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );

  // Build the scene in the read
  void                        _buildScene( Usul::Interfaces::IUnknown *caller = 0x0 );

  // Test method for multiview verification
  osg::Node*                  _buildTestCube( osg::Vec3Array* points, Color c );

private:
    GroupPtr                  _root;
    Usul::Math::Vec3ui        _dimensions;
    Cubes                     _cubes;
    
  
};



#endif // _VAPOR_INTRUSION_GUI_DOCUMENT_H_
