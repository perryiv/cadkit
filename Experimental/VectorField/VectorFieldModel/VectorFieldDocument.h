
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for scene files.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VECTOR_FIELD_DOCUMENT_H_
#define _VECTOR_FIELD_DOCUMENT_H_

#include "VectorField.h"

#include "Usul/Documents/Document.h"

#include "osg/Array"
#include "osgManipulator/CommandManager"

#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IIntersectListener.h"
#include "Usul/Interfaces/IMouseEventListener.h"
#include "Usul/Interfaces/IMenuAdd.h"

#include <string>

namespace osg { class Group; }

class VectorFieldDocument : public Usul::Documents::Document,
                            public Usul::Interfaces::IBuildScene,
                            public Usul::Interfaces::IIntersectListener,
                            public Usul::Interfaces::IMouseEventListener,
                            public Usul::Interfaces::IMenuAdd
{
public:

  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( VectorFieldDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  VectorFieldDocument();

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
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 ) const;

  void                        allowAddParticle( bool b );
  bool                        isAllowAddParticle() const;

protected:

  /// Do not copy.
  VectorFieldDocument ( const VectorFieldDocument & );
  VectorFieldDocument &operator = ( const VectorFieldDocument & );

  /// Use reference counting.
  virtual ~VectorFieldDocument();

  void              _addParticle ( const osg::Vec3& position );

  // Notify the observer of the intersection.
  virtual void      intersectNotify ( float x, float y, const osgUtil::LineSegmentIntersector::Intersection &hit, Usul::Interfaces::IUnknown *caller );

  // Called when mouse event occurs.
  virtual void      mouseEventNotify ( osgGA::GUIEventAdapter&, Usul::Interfaces::IUnknown * );

  virtual void      menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown* caller = 0x0 );

private:
  VectorField _field;
  osg::ref_ptr<osgManipulator::CommandManager> _commandManager;
  osg::ref_ptr < osg::Group > _root;
  bool _allowAddParticles;
};



#endif // _VECTOR_FIELD_DOCUMENT_H_
