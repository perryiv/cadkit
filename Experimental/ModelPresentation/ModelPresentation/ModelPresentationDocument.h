
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

#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Interfaces/IMpdNavigator.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Policies/Update.h"
#include "Usul/Interfaces/ICommandList.h"

#include "OsgTools/Triangles/TriangleSet.h"

#include "XmlTree/Document.h"

#include <string>

namespace osg { class Node; }


class ModelPresentationDocument : public Usul::Documents::Document,
                                  public Usul::Interfaces::IBuildScene,
                                  public Usul::Interfaces::IUpdateListener,
			                            public Usul::Interfaces::ICommandList,
                                  public Usul::Interfaces::IMpdNavigator
{
public:
  // Structs
  struct MpdSet
  {
    unsigned int index;
    std::string name;
  };
  struct MpdGroup
  {
    unsigned int setIndex;
    osg::ref_ptr< osg::Group > group;
  };
  struct MpdModel
  {
     unsigned int setIndex;
     unsigned int groupIndex;
     osg::ref_ptr< osg::Group > model;
  };
  struct MpdTimeGroup
  {
    unsigned int startTime;
    unsigned int endTime;
  };
  struct MpdTimeSet
  {
    osg::ref_ptr< osg::Switch > timeline;
    unsigned int currentTime;
    unsigned int endTime;
    unsigned int interval;
    std::vector< MpdTimeGroup > groups;
  };
  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef Usul::Documents::Document Document;
  typedef Usul::Documents::Document::RefPtr DocumentPtr;
  typedef Usul::Documents::Manager DocManager;
  typedef DocManager::DocumentInfo Info;
  typedef Usul::Policies::TimeBased TimeBased;


  typedef std::vector< osg::ref_ptr< osg::Switch > > MpdScene;
  typedef osg::ref_ptr< osg::Group > GroupPtr;
  typedef std::vector < std::string > Files;
  typedef std::vector< MpdGroup > MpdGroups;
  typedef std::vector< MpdModel > MpdModels;
  typedef std::vector< MpdSet > MpdSets;
 
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
  virtual Filters             filtersOpen()   const;
  virtual Filters             filtersSave()   const;
  virtual Filters             filtersInsert() const;
  virtual Filters             filtersExport() const;

  
  /// Read the document.
  virtual void                read ( const std::string &filename, Unknown *caller = 0x0 );

  /// Write the document to given file name.
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0  ) const;
  
  /// Usul::Interfaces::IMpdNavigator
  
  void              nextGroup( unsigned int index );
  void              startAnimation();
  void              stopAnimation();
  void              nextStep();
  void              prevStep();
  void              firstStep();

  bool              isAnimating();
  void              isAnimating( bool value );

                

protected:

  /// Do not copy.
  ModelPresentationDocument ( const ModelPresentationDocument & );
  ModelPresentationDocument &operator = ( const ModelPresentationDocument & );

  void                        _buildScene();
  bool                        _readParameterFile( XmlTree::Node &node, Unknown *caller );
  void                        _parseHeader( XmlTree::Node &node, Unknown *caller );
  void                        _parseStatic( XmlTree::Node &node, Unknown *caller );
  void                        _parseSet( XmlTree::Node &node, Unknown *caller );
  void                        _parseTimeSet( XmlTree::Node &node, Unknown *caller );
  osg::Node*                  _parseGroup( XmlTree::Node &node, Unknown *caller );
  osg::Node*                  _parseTimeGroup( XmlTree::Node &node, Unknown *caller );
  osg::Node*                  _parseModel( XmlTree::Node &node, Unknown *caller );
  osg::Node*                  _loadFile( const std::string& filename, Unknown *caller );
  osg::Node*                  _loadDirectory( const std::string& dir, Unknown *caller );
  /// Usul::Interfaces::IUpdateListener
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );

  void                        _openDocument ( const std::string &file, Usul::Documents::Document *document, Usul::Interfaces::IUnknown *caller );
  void                        _setStatusBar ( const std::string &text, Usul::Interfaces::IUnknown *caller );
  void                        _checkTimeSteps();
  


  CommandList                 getCommandList();

  /// Use reference counting.
  virtual ~ModelPresentationDocument();

private:
  GroupPtr                    _root;
  GroupPtr                    _static;
  MpdScene                    _sceneTree;
  MpdGroups                   _groups;
  MpdModels                   _models;
  MpdSets                     _sets;
  MpdTimeSet                  _timeSet;
  TimeBased                   _update;
  unsigned int                _updateInterval;
  bool                        _useTimeLine;
  bool                        _isAnimating;



};



#endif // _MODEL_PRESENTATION_DOCUMENT_H_
