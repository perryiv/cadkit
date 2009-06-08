
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, 2008 Arizona State University
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

#ifndef _MODEL_PRESENTATION_DOCUMENT_H_
#define _MODEL_PRESENTATION_DOCUMENT_H_

#include "MpdJob.h"
#include "MpdDefinitions.h"
#include "../ModelPresentationLib/ModelPresentationLib.h"

#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IAnimatePath.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Interfaces/IMpdNavigator.h"
#include "Usul/Interfaces/IMenuAdd.h"
#include "Usul/Interfaces/ITextMatrix.h"
#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Interfaces/IViewport.h"
#include "Usul/Interfaces/IMpdWriter.h"
#include "Usul/Interfaces/ITimeVaryingData.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Policies/Update.h"
#include "Usul/Interfaces/IRenderListener.h"

#include "OsgTools/Triangles/TriangleSet.h"

#include "osg/Camera"

#include "XmlTree/Document.h"

#include <string>
#include <memory>

namespace osg { class Node; }


class ModelPresentationDocument : public Usul::Documents::Document,
                                  public Usul::Interfaces::IBuildScene,
                                  public Usul::Interfaces::IUpdateListener,
                                  public Usul::Interfaces::IMpdNavigator,
                                  public Usul::Interfaces::IMenuAdd,
                                  public Usul::Interfaces::IMpdWriter,
                                  public Usul::Interfaces::ITimeVaryingData
{
public:
 
  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef Usul::Documents::Document Document;
  typedef Usul::Documents::Document::RefPtr DocumentPtr;
  typedef Usul::Documents::Manager DocManager;
  typedef DocManager::DocumentInfo Info;
  typedef Usul::Jobs::Manager JobManager;
  typedef Usul::Interfaces::ITextMatrix ITextMatrix;
  typedef Usul::Interfaces::IViewMatrix IViewMatrix;
  typedef Usul::Interfaces::IViewport IViewport;
  typedef Usul::Policies::NumberBased UpdatePolicy;
  typedef std::auto_ptr< UpdatePolicy > UpdatePolicyPtr;
  typedef osg::Matrixf Matrixf;
  typedef XmlTree::Document::Attributes Attributes;
  typedef XmlTree::Document::Children Children;
 
  typedef osg::ref_ptr< osg::Group > GroupPtr;
  typedef std::vector < std::string > Files;
  typedef std::vector< MpdDefinitions::MpdSet > MpdSets;
  typedef std::vector< MpdDefinitions::MpdTimeSet > MpdTimeSets;
  typedef MpdDefinitions::MpdDynamicSets MpdDynamicSets;
  typedef std::vector< MpdJob::RefPtr > MpdJobs;
  typedef Usul::Interfaces::IAnimatePath IAnimatePath;
  typedef std::vector < osg::Matrixd > MatrixVec;
  typedef std::vector< std::string > modelMenuList;
  typedef std::vector< std::string > timelineMenuList;
 
  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ModelPresentationDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  ModelPresentationDocument();

  /// Build the scene.
  virtual osg::Node *         buildScene ( const BaseClass::Options &options, Unknown *caller = 0x0 );

  /// Usul::Interfaces::IRenderListener
  // Called after a render.
  virtual void                postRenderNotify ( Usul::Interfaces::IUnknown *caller );

  // Called before a render.
  virtual void                preRenderNotify ( Usul::Interfaces::IUnknown *caller );


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
  
  /// Usul::Interfaces::IMpdNavigator
  void              nextStep();
  void              prevStep();
  void              firstStep();
  void              setAnimationPath( const std::string& name );
  void              displayViewMatrix();
  unsigned int      getCurrentGroupFromSet( unsigned int index ) const;
  void              setGroup ( unsigned int set, unsigned int group );
  void              animate( bool state );
  bool              animate();
  void              setTimelineState( unsigned int i, bool state );  
  bool              getTimelineState( unsigned int i );  
  bool              isAnimating();
  void              isAnimating( bool value );
  void              dynamicModelState( unsigned int index, bool state );
  bool              dynamicModelState( unsigned int i );
  void              updateGlobalEndtime();
  void              firstSequenceStep();
  void              nextSequenceStep();
  void              prevSequenceStep();
  void              setAnimationSpeed( unsigned int speed );
  unsigned int      getAnimationSpeed();
  void              setToggleState ( unsigned int index, bool b );
  bool              getToggleState( unsigned int index ) const;

  void              setStep ( unsigned int time );

  /// Usul::Interfaces::IMpdWriter functions
  // Model Functions
  virtual void                    addModel( const std::string &name, const std::string &path );
  // Set functions
  virtual void                    addSet( const std::string &name, const std::string &menuName );
  virtual void                    addModelToSet( const std::string &modelName, const std::string &setName, const std::string &groupName );
  virtual void                    addGroupToSet( const std::string &setName, const std::string &groupName );
  // TimeSet functions
  virtual void                    addTimeSet( const std::string &name, const std::string &menuName, unsigned int endTime );
  virtual void                    addModelsToTimeSet( std::vector< std::string > modelList, const std::string &timeSetName, unsigned int startTime, unsigned int endTime );
  // Sequence Functions
  virtual void                    addSequence( const std::string &name, const std::string &menuName );
  virtual void                    addStepToSequence( const std::string &locationName, std::vector< std::string > modelList );
  // Location Functions
  virtual void                    addLocation( const std::string &name, const std::string &location );
  virtual void                    addLocation( const std::string &name, osg::Matrix location );
  // DynamicSet Functions
  virtual void                    addDynamicSet( const std::string &name, const std::string &menuName, const std::string &prefix,
                                                 const std::string &extension, const std::string &directory, unsigned int maxFiles );
  virtual void                    buildXMLString();
  virtual void                    write() const;
  virtual void                    write( const std::string &filename ) const;

  /// Usul::Interfaces::ITimeVaryingData
  virtual void                    setCurrentTimeStep ( unsigned int current );
  virtual unsigned int            getCurrentTimeStep() const;
  
  virtual unsigned int            getNumberOfTimeSteps() const;
 
protected:

  /// Do not copy.
  ModelPresentationDocument ( const ModelPresentationDocument & );
  ModelPresentationDocument &operator = ( const ModelPresentationDocument & );

  void                        _buildScene();
  bool                        _readParameterFile( XmlTree::Node &node, Unknown *caller, Unknown *progress );
  void                        _parseHeader( XmlTree::Node &node, Unknown *caller, Unknown *progress );
  void                        _parseSet( XmlTree::Node &node, Unknown *caller, Unknown *progress, unsigned int setnum );
  void                        _parseTimeSet( XmlTree::Node &node, Unknown *caller, Unknown *progress );
  void                        _parseSequence( XmlTree::Node &node, Unknown *caller, Unknown *progress );
  void                        _parseLocation( XmlTree::Node &node, Unknown *caller, Unknown *progress, MpdDefinitions::Location &location, MpdDefinitions::LocationNames &names );
  void                        _parseMatrix( XmlTree::Node &node, Unknown *caller, Unknown *progress, const std::string& name, MpdDefinitions::Location &location, MpdDefinitions::LocationNames &names );
  osg::Node*                  _parseGroup( XmlTree::Node &node, Unknown *caller, Unknown *progress, MpdDefinitions::MpdSet & set, MpdDefinitions::MpdGroup & grp );
  osg::Node*                  _parseTimeGroup( XmlTree::Node &node, Unknown *caller, Unknown *progress, unsigned int &currentTime, MpdDefinitions::MpdTimeSet &timeset );
  void                        _parseSequenceStep( XmlTree::Node &node, Unknown *caller, Unknown *progress, MpdDefinitions::MpdSequenceStep &step );
  void                        _parseModels( XmlTree::Node &node, Unknown *caller, Unknown *progress );
  osg::Node*                  _parseModel( XmlTree::Node &node, Unknown *caller, Unknown *progress, std::string &name );
  void                        _parseDynamic( XmlTree::Node &node, Unknown *caller, Unknown *progress ); 
 
  osg::Node*                  _loadFile( const std::string& filename, Unknown *caller, Unknown *progress );
  osg::Node*                  _loadDirectory( const std::string& dir, Unknown *caller, Unknown *progress );
  MatrixVec                   _getInterpolationMatrices ( const osg::Matrixd &m1, const osg::Matrixd &m2 ) const;
  bool                        _dynamic();

  bool                        _checkTime();
  void                        _checkTime( bool value );

  std::string                 _getWorkingDir();
  void                        _processJobData( unsigned int index, Usul::Interfaces::IUnknown *caller );

  // TODO: return a smart pointer instead
  MpdJob::RefPtr              _getJobAtIndex( unsigned int index );

  void                        _handleSequenceEvent();
  osg::Node*                  _createProxyGeometry( const std::string &message, Usul::Interfaces::IUnknown *caller );

  /// Usul::Interfaces::IUpdateListener
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );

  void                        _openDocument ( const std::string &file, Usul::Documents::Document *document, Usul::Interfaces::IUnknown *caller, Unknown *progress );
  void                        _setStatusBar ( const std::string &text, Usul::Interfaces::IUnknown *caller );
  void                        _checkTimeSteps( Usul::Interfaces::IUnknown *caller );
  void                        _incrementTimeStep();
  
  void                        _setMatrix( osg::Matrix * matrix, const std::string& values, const std::string& type );

  void                        _dynamicModelState  ( bool state, unsigned int index );

  //Usul::Interfaces::IMenuAdd
  void                        menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller = 0x0 );

  void                        _setStatusText( const std::string message, unsigned int &textXPos, unsigned int &textYPos, double xmult, double ymult, Usul::Interfaces::IUnknown *caller );
  
  osg::Vec3d                  _getPosition( const std::string &position, Usul::Interfaces::IUnknown *caller );

  void                        _write( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  // Sequence Animation Method
  void                        _animateSequence( bool value );
  bool                        _isAnimatingSequence();

  /// Use reference counting.
  virtual ~ModelPresentationDocument();

private:
  typedef std::set<unsigned int> CompiledDisplayLists;

  GroupPtr                      _root;
  MpdDefinitions::MpdModels     _mpdModels;
  MpdSets                       _sets;
  MpdTimeSets                   _timeSets;
  MpdDynamicSets                _dynamicSets;
  UpdatePolicyPtr               _update;
  UpdatePolicyPtr               _checkFileSystem;
  bool                          _useTimeLine;
  bool                          _useModels;
  bool                          _useDynamic;
  bool                          _useSequence;
  bool                          _isAnimating;
  bool                          _showTools;
  bool                          _userSpecifiedEndTime;
  bool                          _checkTimeStatus;

  MpdDefinitions::Locations     _locations;
  MpdDefinitions::LocationNames _locationNames;
  MpdDefinitions::MpdSequence   _sequence;
   

  std::string                   _workingDir;
  unsigned int                  _globalTimelineEnd;
  unsigned int                  _globalCurrentTime;

  MpdJobs                       _jobs;

  //Text variables for status
  unsigned int                  _textXPos;
  unsigned int                  _textYPos;
  unsigned int                  _dynamicNotLoadedTextXPos;
  unsigned int                  _dynamicNotLoadedTextYPos;

  osg::ref_ptr< osg::Camera >   _camera;
  ModelPresentationLib::RefPtr  _writer;

  unsigned int                  _animationSpeed;

  CompiledDisplayLists          _compileDisplayLists;

  bool                          _animatingSequence;

  
};



#endif // _MODEL_PRESENTATION_DOCUMENT_H_
