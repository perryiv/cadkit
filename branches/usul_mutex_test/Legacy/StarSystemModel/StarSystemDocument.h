
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for star-systems.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _STAR_SYSTEM_MODEL_DOCUMENT_H_
#define _STAR_SYSTEM_MODEL_DOCUMENT_H_

#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IDatabasePager.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Interfaces/IMenuAdd.h"
#include "Usul/Jobs/Manager.h"

#include "StarSystem/System.h"
#include "StarSystem/Hud.h"

#include "Serialize/XML/Macros.h"

#include <string>

namespace osg { class Node; }
namespace osgDB { class DatabasePager; }


class StarSystemDocument : public Usul::Documents::Document,
                           public Usul::Interfaces::IBuildScene,
                           public Usul::Interfaces::IDatabasePager,
                           public Usul::Interfaces::IUpdateListener,
                           public Usul::Interfaces::IMenuAdd
{
public:

  // Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef osgDB::DatabasePager Pager;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( StarSystemDocument );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Construction.
  StarSystemDocument();

  // Add the view to the document.
  virtual void                addView ( Usul::Interfaces::IView *view );

  // Build the scene.
  virtual osg::Node *         buildScene ( const BaseClass::Options &options, Unknown *caller = 0x0 );

  // Return true if this document can do it.
  virtual bool                canExport ( const std::string &file ) const;
  virtual bool                canInsert ( const std::string &file ) const;
  virtual bool                canOpen   ( const std::string &file ) const;
  virtual bool                canSave   ( const std::string &file ) const;

  // Clear any existing data.
  virtual void                clear ( Unknown *caller = 0x0 );

  // Get the filters that correspond to what this document can read and write.
  virtual Filters             filtersOpen()   const;
  virtual Filters             filtersSave()   const;
  virtual Filters             filtersInsert() const;
  virtual Filters             filtersExport() const;

  // Get the database pager.
  virtual Pager *             getDatabasePager();

  // Read the document.
  virtual void                read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  // Remove the view from the document.
  virtual void                removeView ( Usul::Interfaces::IView *view );

  /// Usul::Interfaces::IRenderListener inherited from base class.
  virtual void                postRenderNotify ( Unknown *caller );
  virtual void                preRenderNotify ( Unknown *caller );

  // Write the document to given file name.
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0  ) const;

  // Get/Set the scale.
  void                        scale ( double );
  bool                        isScale ( double ) const;
protected:

  // Do not copy.
  StarSystemDocument ( const StarSystemDocument & );
  StarSystemDocument &operator = ( const StarSystemDocument & );

  // Use reference counting.
  virtual ~StarSystemDocument();

  /// Update (Usul::Interfaces::IUpdateListener).
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );

  /// Add to the menu.
  virtual void                menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown* caller = 0x0 );
private:

  void                        _destroy();

  Usul::Jobs::Manager *       _getJobManager();

  void                        _makeSystem();

  StarSystem::System::RefPtr _system;
  StarSystem::Hud _hud;
  Usul::Jobs::Manager *_manager;
  double _scale;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS ( star_system_document );
};



#endif // _POLYGON_MODEL_DOCUMENT_H_
