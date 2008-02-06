
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
//  Document for mpd files
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODEL_PRESENTATION_DOCUMENT_H_
#define _MODEL_PRESENTATION_DOCUMENT_H_

#include "Experimental/ModelPresentation/ModelPresentation/MpdJob.h"
#include "Experimental/ModelPresentation/ModelPresentation/MpdDefinitions.h"

#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IAnimatePath.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Interfaces/IMpdNavigator.h"
#include "Usul/Interfaces/IMenuAdd.h"
#include "Usul/Interfaces/IDataSync.h"
#include "Usul/Interfaces/ITextMatrix.h"
#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Interfaces/IViewport.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Policies/Update.h"

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
                                  public Usul::Interfaces::IMenuAdd
{
public:
 
  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef Usul::Documents::Document Document;
  typedef Usul::Documents::Document::RefPtr DocumentPtr;
  typedef Usul::Documents::Manager DocManager;
  typedef DocManager::DocumentInfo Info;
  typedef Usul::Jobs::Manager JobManager;
  typedef Usul::Interfaces::IDataSync IDataSync;
  typedef Usul::Interfaces::ITextMatrix ITextMatrix;
  typedef Usul::Interfaces::IViewMatrix IViewMatrix;
  typedef Usul::Interfaces::IViewport IViewport;
  typedef Usul::Policies::NumberBased UpdatePolicy;
  typedef std::auto_ptr< UpdatePolicy > UpdatePolicyPtr;
  typedef osg::Matrixf Matrixf;
  typedef XmlTree::Document::Attributes Attributes;
  typedef XmlTree::Document::Children Children;
  typedef std::vector < std::string > Files;

  typedef std::vector< osg::ref_ptr< osg::Switch > > MpdScene;
  typedef osg::ref_ptr< osg::Group > GroupPtr;
  typedef std::vector < std::string > Files;
  typedef std::vector< MpdDefinitions::MpdGroup > MpdGroups;
  typedef std::vector< MpdDefinitions::MpdModel > MpdModels;
  typedef std::vector< MpdDefinitions::MpdSet > MpdSets;
  
  
  typedef std::vector< MpdDefinitions::MpdTimeSet > MpdTimeSets;
  typedef MpdDefinitions::MpdDynamicSets MpdDynamicSets;

  typedef std::vector< MpdJob::RefPtr > MpdJobs;
  

  typedef Usul::Interfaces::IAnimatePath IAnimatePath;
  typedef std::vector < osg::Matrixd > MatrixVec;

  typedef std::vector< std::string > modelMenuList;
  typedef std::vector< std::string > timelineMenuList;
  //typedef std::vector< MpdDefinitions::MpdSequence > MpdSequences;
 
  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ModelPresentationDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  ModelPresentationDocument();

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
  
  /// Usul::Interfaces::IMpdNavigator
  void              nextGroup( unsigned int index );
  void              nextStep();
  void              prevStep();
  void              firstStep();
  void              setAnimationPath( const std::string& name );
  void              displayViewMatrix();
  unsigned int      getCurrentGroupFromSet( unsigned int index ) const;
  void              setGroup ( unsigned int set, unsigned int group );
  void              animate( bool state );
  bool              animate();
  void              timelineModelState( unsigned int i, bool state );  
  bool              timelineModelState( unsigned int i );  
  bool              isAnimating();
  void              isAnimating( bool value );
  void              dynamicModelState( unsigned int index, bool state );
  bool              dynamicModelState( unsigned int i );
  void              validateDynamicSets();
  void              updateGlobalEndtime();
  void              firstSequenceStep();
  void              nextSequenceStep();
  void              prevSequenceStep();

 
protected:

  /// Do not copy.
  ModelPresentationDocument ( const ModelPresentationDocument & );
  ModelPresentationDocument &operator = ( const ModelPresentationDocument & );

  void                        _buildScene();
  bool                        _readParameterFile( XmlTree::Node &node, Unknown *caller, Unknown *progress );
  void                        _parseHeader( XmlTree::Node &node, Unknown *caller, Unknown *progress );
  void                        _parseStatic( XmlTree::Node &node, Unknown *caller, Unknown *progress );
  void                        _parseSet( XmlTree::Node &node, Unknown *caller, Unknown *progress, unsigned int setnum );
  void                        _parseTimeSet( XmlTree::Node &node, Unknown *caller, Unknown *progress );
  void                        _parseSequence( XmlTree::Node &node, Unknown *caller, Unknown *progress );
  void                        _parseLocation( XmlTree::Node &node, Unknown *caller, Unknown *progress, MpdDefinitions::Location &location, MpdDefinitions::LocationNames &names );
  void                        _parseMatrix( XmlTree::Node &node, Unknown *caller, Unknown *progress, const std::string& name, MpdDefinitions::Location &location, MpdDefinitions::LocationNames &names );
  osg::Node*                  _parseGroup( XmlTree::Node &node, Unknown *caller, Unknown *progress, MpdDefinitions::MpdSet & set );
  osg::Node*                  _parseTimeGroup( XmlTree::Node &node, Unknown *caller, Unknown *progress, unsigned int &currentTime, MpdDefinitions::MpdTimeSet &timeset );
  osg::Node*                  _parseSequenceStep( XmlTree::Node &node, Unknown *caller, Unknown *progress, MpdDefinitions::MpdSequenceStep &step );
  osg::Node*                  _parseModel( XmlTree::Node &node, Unknown *caller, Unknown *progress );
  void                        _parseDynamic( XmlTree::Node &node, Unknown *caller, Unknown *progress ); 
  osg::Node*                  _loadFile( const std::string& filename, Unknown *caller, Unknown *progress );
  osg::Node*                  _loadDirectory( const std::string& dir, Unknown *caller, Unknown *progress );
  MatrixVec                   _getInterpolationMatrices ( const osg::Matrixd &m1, const osg::Matrixd &m2 ) const;
  bool                        _dynamic();

  std::string                 _getWorkingDir();
  void                        _processJobData( unsigned int index, Usul::Interfaces::IUnknown *caller );
  MpdJob*                     _getJobAtIndex( unsigned int index );

  void                        _handleSequenceEvent();
  osg::Node*                  _createProxyGeometry( const std::string &message, Usul::Interfaces::IUnknown *caller );
  


  /// Usul::Interfaces::IUpdateListener
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );
  void                        _updateCamera( Usul::Interfaces::IUnknown *caller );

  void                        _openDocument ( const std::string &file, Usul::Documents::Document *document, Usul::Interfaces::IUnknown *caller, Unknown *progress );
  void                        _setStatusBar ( const std::string &text, Usul::Interfaces::IUnknown *caller );
  void                        _checkTimeSteps( Usul::Interfaces::IUnknown *caller );
  void                        _incrementTimeStep();
  
  void                        _setMatrix( osg::Matrix * matrix, const std::string& values, const std::string& type );

  void                        _dynamicModelState  ( bool state, unsigned int index );

  //Usul::Interfaces::IMenuAdd
  void                        menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller = 0x0 );

  void                        _setStatusText( const std::string message, Usul::Interfaces::IUnknown *caller );

  

  /// Use reference counting.
  virtual ~ModelPresentationDocument();

private:
  GroupPtr                      _root;
  GroupPtr                      _static;
  MpdScene                      _sceneTree;
  MpdGroups                     _groups;
  MpdModels                     _models;
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

  osg::ref_ptr< osg::Camera >   _camera;
  
};



#endif // _MODEL_PRESENTATION_DOCUMENT_H_
