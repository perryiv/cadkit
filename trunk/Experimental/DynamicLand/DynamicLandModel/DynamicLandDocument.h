
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for dld files
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DYNAMIC_LAND_MODEL_DOCUMENT_H_
#define _DYNAMIC_LAND_MODEL_DOCUMENT_H_

#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Interfaces/IDldNavigator.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Policies/Update.h"
#include "Usul/Interfaces/ICommandList.h"

#include "OsgTools/Triangles/TriangleSet.h"

#include "XmlTree/Document.h"

#include <string>

namespace osg { class Node; }


class DynamicLandDocument : public Usul::Documents::Document,
                            public Usul::Interfaces::IBuildScene,
                            public Usul::Interfaces::IUpdateListener,
                            public Usul::Interfaces::IDldNavigator,
			                      public Usul::Interfaces::ICommandList
{
public:

  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef Usul::Documents::Document Document;
  typedef Usul::Documents::Manager DocManager;
  typedef DocManager::DocumentInfo Info;
  typedef OsgTools::Triangles::TriangleSet::RefPtr TriangleSetPtr;
  typedef std::vector < std::string > Files;
  typedef std::vector < std::string > StringVector;
  typedef std::vector < osg::ref_ptr< osg::Group > > TimeStepPool;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( DynamicLandDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  DynamicLandDocument();

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

  // Load a model file
  bool                        load ( const std::string& filename, Usul::Interfaces::IUnknown *caller );

  // Set whether or not to load the current model


  /// Read the document.
  virtual void                read ( const std::string &filename, Unknown *caller = 0x0 );

  /// Write the document to given file name.
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0  ) const;
  
  // Write a TDF file
  bool                        writeTDF ( const std::string& filename, Usul::Interfaces::IUnknown *caller );

  // Usul::Interfaces::IDldNavigator
  bool                        decrementFilePosition ();
  bool                        incrementFilePosition ();
  unsigned int                currentFilePosition ();
  bool                        loadCurrentFile( bool loadFile );


protected:

  /// Do not copy.
  DynamicLandDocument ( const DynamicLandDocument & );
  DynamicLandDocument &operator = ( const DynamicLandDocument & );

  bool                        _load ( const std::string& filename, Usul::Interfaces::IUnknown *caller );
  bool                        _loadTexture ( const std::string& filename, Usul::Interfaces::IUnknown *caller );
  void                        _openDocument ( const std::string &file, Usul::Documents::Document *document, Usul::Interfaces::IUnknown *caller );
  void                        _buildScene();
  bool                        _readParameterFile( XmlTree::Node &node, Unknown *caller );
  void                        _parseHeader( XmlTree::Node &node, Unknown *caller );

  // Navigation and loading functions
  void                        _loadCurrentFileFromDocument( Usul::Interfaces::IUnknown *caller );
  void                        _loadNextTimeStep();
  void                        _loadPrevTimeStep();

  /// Usul::Interfaces::IUpdateListener
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );

  CommandList                 getCommandList();

  /// Use reference counting.
  virtual ~DynamicLandDocument();

private:
  
  float                           _cellSize;
  float                           _noDataValue;
  Usul::Math::Vec2ui              _gridSize;
  Usul::Math::Vec2f               _ll;
  Document::RefPtr                _currentDocument;
  Document::RefPtr                _newDocument;
  osg::ref_ptr< osg::Group >      _terrain;

  // Update variables
  Usul::Policies::TimeBased       _tb;
  unsigned int                    _currFileNum;
  unsigned int                    _numFilesInDirectory; 
  Files                           _files;
  bool                            _loadNewMap;
  std::string                     _dir;
  std::string                     _ext;
  std::string                     _prefix;

  // Navigation Variables
  bool                            _stepForward;
  bool                            _stepBackward;
  bool                            _animateForward;
  bool                            _animateBackward;
  TimeStepPool                    _timeStepPool;
  unsigned int                    _timeStepWindowSize;

  class LoadDataJob : public Usul::Jobs::Job
  {
  public:
    typedef Usul::Jobs::Job                           BaseClass;

    USUL_DECLARE_REF_POINTERS ( LoadDataJob );

    LoadDataJob ( DynamicLandDocument* document, const std::string& filename, Usul::Interfaces::IUnknown *caller );

  protected:

    virtual void _started ();
    DynamicLandDocument::RefPtr           _document;
    std::string                           _filename;
    Usul::Interfaces::IUnknown::QueryPtr  _caller;
  };


};



#endif // _DYNAMIC_LAND_MODEL_DOCUMENT_H_
