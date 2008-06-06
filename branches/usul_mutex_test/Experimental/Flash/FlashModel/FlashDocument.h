
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for flash files
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FLASH_MODEL_DOCUMENT_H_
#define _FLASH_MODEL_DOCUMENT_H_

#include "Experimental/Flash/FlashModel/Timestep.h"

#include "Usul/Documents/Document.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Jobs/Manager.h"

#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/ITimeVaryingData.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Interfaces/IMenuAdd.h"

#include "Serialize/XML/Macros.h"

#include "OsgTools/Volume/GPURayCasting.h"
#include "OsgTools/Volume/Texture3DVolume.h"
#include "OsgTools/Volume/TransferFunction.h"

#include "osg/BoundingBox"
#include "osg/Group"

#include <string>
#include <vector>
#include <list>

// Uncomment this line to use GPU ray casting.  There are still some issues to work out.
#define USE_RAY_CASTING 0

class FlashDocument : public Usul::Documents::Document,
                      public Usul::Interfaces::IBuildScene,
                      public Usul::Interfaces::ITimeVaryingData,
                      public Usul::Interfaces::IUpdateListener,
                      public Usul::Interfaces::IMenuAdd
{
public:

  /// Useful typedefs.
  typedef Usul::Documents::Document      BaseClass;
  
#if USE_RAY_CASTING
  typedef OsgTools::Volume::GPURayCasting Volume;
#else
  typedef OsgTools::Volume::Texture3DVolume Volume;
#endif

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( FlashDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  FlashDocument();
  
  /// Build the scene.
  virtual osg::Node *         buildScene ( const BaseClass::Options &options, Unknown *caller = 0x0 );

  /// Return true if this document can do it.
  virtual bool                canExport ( const std::string &file ) const;
  virtual bool                canInsert ( const std::string &file ) const;
  virtual bool                canOpen   ( const std::string &file ) const;
  virtual bool                canSave  ( const std::string &file ) const;

  /// Clear any existing data.
  virtual void                clear ( Unknown *caller = 0x0 );
  
  // Deserialize.
  virtual void                deserialize ( const XmlTree::Node &node );
  
  /// Get/Set the dirty flag.
  void                        dirty ( bool b );
  bool                        dirty () const;

  /// Set/get the draw bounding box flag.
  void                        drawBBox ( bool b );
  bool                        isDrawBBox() const;
  
  /// Set/get the draw points flag.
  void                        drawPoints ( bool b );
  bool                        isDrawPoints() const;
  
  /// Set/get the draw volume flag.
  void                        drawVolume ( bool b );
  bool                        isDrawVolume() const;

  
  /// Get the filters that correspond to what this document can read and write.
  virtual Filters             filtersOpen()   const;
  virtual Filters             filtersSave()   const;
  virtual Filters             filtersInsert() const;
  virtual Filters             filtersExport() const;

  /// Read the document.
  virtual void                read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  /// Write the document to given file name.
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0  ) const;
  
  /// Usul::Interfaces::IUpdateListener
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );

protected:

  /// Do not copy.
  FlashDocument ( const FlashDocument & );
  FlashDocument &operator = ( const FlashDocument & );

  /// Use reference counting.
  virtual ~FlashDocument();
  
  /// Build the scene.
  void                        _buildScene();

  /// Build the default transfer functions.
  void                        _buildDefaultTransferFunctions ();
  
  /// Is the i'th timestep loaded?
  bool                        _hasTimestep ( unsigned int i ) const;
  
  /// Load the i'th timestep.
  void                        _loadTimestep ( unsigned int i );

  /// Usul::Interfaces::ITimeVaryingData
  virtual void                setCurrentTimeStep ( unsigned int current );
  virtual unsigned int        getCurrentTimeStep () const;
  
  virtual unsigned int        getNumberOfTimeSteps () const;
  
  /// Add to the menu (IMenuAdd).
  virtual void                menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller = 0x0 );

private:
  
  typedef std::vector<std::string> Filenames;
  typedef OsgTools::Volume::TransferFunction                    TransferFunction;
  typedef TransferFunction::RefPtr                              TransferFunctionPtr;
  typedef std::vector < TransferFunctionPtr >                   TransferFunctions;
  typedef std::map <unsigned int, Timestep::RefPtr>             Timesteps;
  
  Filenames _filenames;
  double _scale;
  unsigned int _currentTimestep;
  osg::ref_ptr < osg::Group > _root;
  bool _dirty;
  bool _drawBBox;
  bool _drawPoints;
  bool _drawVolume;
  
  unsigned int _currentTransferFunction;
  TransferFunctions _transferFunctions;
  Timesteps _timesteps;
  osg::ref_ptr<osg::Program> _program;
  
  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_CLASS_NAME ( FlashDocument );
  SERIALIZE_XML_SERIALIZE_FUNCTION;
  SERIALIZE_XML_ADD_MEMBER_FUNCTION;
};



#endif // _FLASH_MODEL_DOCUMENT_H_
