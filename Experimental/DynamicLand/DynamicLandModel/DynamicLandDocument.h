
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
#include "Usul/Jobs/Manager.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Policies/Update.h"
#include "Usul/Interfaces/ICommandList.h"

#include "OsgTools/Triangles/TriangleSet.h"

#include "XmlTree/Document.h"

#include "boost/shared_ptr.hpp"

#include <string>

namespace osg { class Node; }


class DynamicLandDocument : public Usul::Documents::Document,
                            public Usul::Interfaces::IBuildScene,
                            public Usul::Interfaces::IUpdateListener,
                            public Usul::Interfaces::IDldNavigator,
			                      public Usul::Interfaces::ICommandList
{
public:
  // Structs
  struct HeaderInfo
  {
    float                           cellSize;
    float                           noDataValue;
    Usul::Math::Vec2ui              gridSize;
    Usul::Math::Vec2f               ll;
  };
  
  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef Usul::Documents::Document Document;
  typedef Usul::Documents::Document::RefPtr DocumentPtr;
  typedef Usul::Documents::Manager DocManager;
  typedef Usul::Jobs::Manager JobManager;
  typedef boost::shared_ptr<JobManager> JobManagerPtr;
  typedef DocManager::DocumentInfo Info;
  typedef OsgTools::Triangles::TriangleSet::RefPtr TriangleSetPtr;
  typedef std::vector < std::string > Files;
  typedef std::vector < std::string > StringVector;
  typedef osg::ref_ptr< osg::Group > GroupPtr;
  struct TimeStep
  {
    bool isValid, isLoading, isLoaded;
    //DocumentPtr document;
    GroupPtr group;
  };
  
  typedef std::vector < TimeStep > TimeStepPool;

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

  
  // toggle to denote that loading is taking place
  void                        newFrameLoaded( bool l );  

  /// Read the document.
  virtual void                read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  /// Write the document to given file name.
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0  ) const;
  
  // Write a TDF file
  bool                        writeTDF ( const std::string& filename, Usul::Interfaces::IUnknown *caller, Document* document );

  // Usul::Interfaces::IDldNavigator
  bool                        decrementFilePosition ();
  bool                        incrementFilePosition ();
  unsigned int                currentFilePosition ();
  int                         currentFileLoaded();
  bool                        loadCurrentFile( bool loadFile );
  void                        animate( bool a );
  void                        setCurrentFilePosition( unsigned int pos );
  
  // are we currently animating
  bool                        isAnimating(){ return _isAnimating; };

  DocumentPtr                 document();  

  void                        setTimeStepFrame( unsigned int i, osg::Group * group );

  HeaderInfo                  getHeaderInfo(){ return _header; };
  bool                        isLoading( unsigned int pos );
  void                        isLoading( unsigned int pos, bool loading );
  bool                        isLoaded( unsigned int pos );
  void                        isLoaded( unsigned int pos, bool loaded );
  bool                        isValid( unsigned int pos );
  void                        isValid( unsigned int pos, bool valid );
  std::string                 dir();
  bool                        loadCurrentFile();
  unsigned int                numFiles();
  std::string                 getFilenameAtIndex( unsigned int index );
  void                        removeTimeStepAtIndex( unsigned int index );
  unsigned int                timeStepWindowSize();

protected:

  /// Do not copy.
  DynamicLandDocument ( const DynamicLandDocument & );
  DynamicLandDocument &operator = ( const DynamicLandDocument & );

  void                        _buildScene();
  bool                        _readParameterFile( XmlTree::Node &node, Unknown *caller );
  void                        _parseHeader( XmlTree::Node &node, Unknown *caller );

  // Navigation and loading functions
  void                        _loadCurrentFileFromDocument( Usul::Interfaces::IUnknown *caller );
  void                        _loadNextTimeStep();
  void                        _loadPrevTimeStep();


  // Animation Functions
  bool                        _updateAnimationFrame();
  void                        _updateAnimationFrame( bool u );
 

  /// Usul::Interfaces::IUpdateListener
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );

  CommandList                 getCommandList();

  void                        _setThreadPoolSize( unsigned int size );

  void                        _findFiles();
  void                        _loadJobs( Usul::Interfaces::IUnknown *caller );

  Usul::Jobs::Manager *       _getJobManager();

  void                        _setStatusBar ( const std::string &text, Usul::Interfaces::IUnknown *caller );
  void                        _currentFileLoaded( int index);

  /// Use reference counting.
  virtual ~DynamicLandDocument();

private:
  
  HeaderInfo                      _header;
  Document::RefPtr                _currentDocument;
  Document::RefPtr                _newDocument;
  osg::ref_ptr< osg::Group >      _terrain;

  // Update variables
  Usul::Policies::TimeBased       _fileQueryDelay;
  Usul::Policies::TimeBased       _animationDelay;
  unsigned int                    _currFileNum;
  int                             _currFileLoaded;
  unsigned int                    _numFilesInDirectory; 
  Files                           _files;
  bool                            _loadNewMap;
  std::string                     _dir;
  std::string                     _ext;
  std::string                     _prefix;

  // Navigation Variables 
  bool                            _stepForward;
  bool                            _stepBackward;
  TimeStepPool                    _timeStepPool;
  unsigned int                    _timeStepWindowSize;
  unsigned int                    _numTimeSteps;

  //Animation Variables
  bool                            _isAnimating;
  bool                            _updateAnimation;

  bool                            _setPoolSize;
  JobManagerPtr                   _jobManager;

  class KillJob : public Usul::Jobs::Job
  {
    public:
      typedef Usul::Jobs::Job                           BaseClass;

      USUL_DECLARE_REF_POINTERS ( KillJob );

      KillJob ( DynamicLandDocument* document, Usul::Interfaces::IUnknown *caller, unsigned int i );
      void                                  removeTimeStep();
      unsigned int                          index(){ return _index; };

    protected:
      virtual void                          _started ();

    private:
      DynamicLandDocument::RefPtr           _document;
      unsigned int                          _index;
  };

  class LoadDataJob : public Usul::Jobs::Job
  {
  public:
    typedef Usul::Jobs::Job                           BaseClass;

    USUL_DECLARE_REF_POINTERS ( LoadDataJob );

    LoadDataJob ( DynamicLandDocument* document, const std::string& filename, Usul::Interfaces::IUnknown *caller, unsigned int i );
    osg::Node* _buildScene( Usul::Documents::Document* document );
    // Load a model file
    Usul::Documents::Document *       load ( const std::string& filename, Usul::Interfaces::IUnknown *caller );
  

  protected:

    virtual void                          _started ();
    Usul::Documents::Document *           _load ( const std::string& filename, Usul::Interfaces::IUnknown *caller );
    void                                  _openDocument ( const std::string &file, Usul::Documents::Document *document, Usul::Interfaces::IUnknown *caller );
    bool                                  _loadTexture ( Usul::Documents::Document *document, const std::string& filename, Usul::Interfaces::IUnknown *caller );
  
  private:
  
    DynamicLandDocument::RefPtr           _document;
    std::string                           _filename;
    Usul::Interfaces::IUnknown::QueryPtr  _caller;
    unsigned int                          _index;
    HeaderInfo                            _header;

  };


};



#endif // _DYNAMIC_LAND_MODEL_DOCUMENT_H_
