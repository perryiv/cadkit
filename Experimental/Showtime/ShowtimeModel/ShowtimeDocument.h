
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for showtime.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SHOW_TIME_MODEL_DOCUMENT_H_
#define _SHOW_TIME_MODEL_DOCUMENT_H_

#include "ShowtimeModel/CompileGuard.h"
#include "ShowtimeModel/Actions/Action.h"
#include "ShowtimeModel/Model.h"

#include "XmlTree/Node.h"

#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IMenuAdd.h"
#include "Usul/Interfaces/ITimerNotify.h"
#include "Usul/Interfaces/IUpdateListener.h"

#include "osg/Group"
#include "osg/ref_ptr"

#include <map>
#include <vector>


class ShowtimeDocument : public Usul::Documents::Document,
                         public Usul::Interfaces::IBuildScene,
                         public Usul::Interfaces::IUpdateListener,
                         public Usul::Interfaces::IMenuAdd,
                         public Usul::Interfaces::ITimerNotify
{
public:

  // Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef std::map < std::string, Showtime::Model::RefPtr > Models;
  typedef std::pair < std::string, XmlTree::Node::RefPtr > FileData;
  typedef std::vector < FileData > Files;
  typedef std::vector < Showtime::Actions::Action::RefPtr > Actions;
  typedef Usul::Interfaces::ITimerNotify::TimerID TimerID;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ShowtimeDocument );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Construction.
  ShowtimeDocument();

  // Add the model
  void                        addModel ( const std::string &name, Showtime::Model *model );

  // Build the scene.
  virtual osg::Node *         buildScene ( const BaseClass::Options &options, Unknown *caller = 0x0 );

  // Return true if this document can do it.
  virtual bool                canExport ( const std::string &file ) const;
  virtual bool                canInsert ( const std::string &file ) const;
  virtual bool                canOpen   ( const std::string &file ) const;
  virtual bool                canSave   ( const std::string &file ) const;

  // Clear any existing data.
  virtual void                clear ( Unknown *caller = 0x0 );

  // Get the current step.
  unsigned int                currentStep() const;

  // Get the filters that correspond to what this document can read and write.
  virtual Filters             filtersOpen()   const;
  virtual Filters             filtersSave()   const;
  virtual Filters             filtersInsert() const;
  virtual Filters             filtersExport() const;

  // Add to the menu.
  virtual void                menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown* caller = 0x0 );

  // Get the model. Returns null if name is not found.
  Showtime::Model::RefPtr     model ( const std::string &name );

  // Read the document.
  virtual void                read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  // Called when the timer fires (Usul::Interfaces::ITimerNotify).
  virtual void                timerNotify ( TimerID );

  // Update (Usul::Interfaces::IUpdateListener).
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );

  // Write the document to given file name.
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0  ) const;

protected:

  // Do not copy.
  ShowtimeDocument ( const ShowtimeDocument & );
  ShowtimeDocument &operator = ( const ShowtimeDocument & );

  // Use reference counting.
  virtual ~ShowtimeDocument();

  Actions                     _findActions ( unsigned int step );
  Files                       _findModelFiles ( const std::string &initialDir, const XmlTree::Node *node ) const;

  void                        _loadActions ( const XmlTree::Node *node );
  void                        _loadModel ( const std::string &path, const XmlTree::Node *node, Usul::Interfaces::IUnknown * );
  void                        _loadModels ( const Files &files, Usul::Interfaces::IUnknown *, Usul::Interfaces::IUnknown * );

  void                        _removeTimer();

  void                        _setTimer();

private:

  void                        _destroy();

private:

  osg::ref_ptr<osg::Group> _scene;
  Models _models;
  Actions _actions;
  unsigned int _numSteps;
  unsigned int _currentStep;
  TimerID _timer;
};



#endif // _POLYGON_MODEL_DOCUMENT_H_
