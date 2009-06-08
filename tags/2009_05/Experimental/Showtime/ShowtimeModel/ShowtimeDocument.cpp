
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

#include "ShowtimeModel/ShowtimeDocument.h"
#include "ShowtimeModel/Actions/Fade.h"

#include "Serialize/XML/Serialize.h"
#include "Serialize/XML/Deserialize.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
#include "MenuKit/ToggleButton.h"
#include "MenuKit/RadioButton.h"

#include "OsgTools/Group.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Components/Manager.h"
#include "Usul/Factory/BaseFactory.h"
#include "Usul/Factory/TypeCreator.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Interfaces/GUI/IProgressBarFactory.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Interfaces/ITimerService.h"
#include "Usul/Math/MinMax.h"
#include "Usul/MPL/SameType.h"
#include "Usul/Scope/CurrentDirectory.h"
#include "Usul/Strings/Case.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/Directory.h"
#include "Usul/System/Sleep.h"
#include "Usul/Trace/Trace.h"

#include "osg/Group"

#include <algorithm>

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ShowtimeDocument, ShowtimeDocument::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

ShowtimeDocument::ShowtimeDocument() : BaseClass ( "Showtime Document" ),
  _scene ( new osg::Group ),
  _models(),
  _actions(),
  _numSteps ( 0 ),
  _currentStep ( 0 ),
  _timer ( 0 )
{
  USUL_TRACE_SCOPE;

  // Check type compatability.
  typedef Showtime::Actions::Action::StepRange::value_type StepType;
  USUL_ASSERT_SAME_TYPE ( StepType, unsigned int );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

ShowtimeDocument::~ShowtimeDocument()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &ShowtimeDocument::_destroy ), "9218430010" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy
//
///////////////////////////////////////////////////////////////////////////////

void ShowtimeDocument::_destroy()
{
  USUL_TRACE_SCOPE;
  this->clear();
  _scene = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *ShowtimeDocument::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;

  switch ( iid )
  {
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  case Usul::Interfaces::IUpdateListener::IID:
    return static_cast < Usul::Interfaces::IUpdateListener* > ( this );
  case Usul::Interfaces::IMenuAdd::IID:
    return static_cast < Usul::Interfaces::IMenuAdd* > ( this );
  case Usul::Interfaces::ITimerNotify::IID:
    return static_cast < Usul::Interfaces::ITimerNotify* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document exports given extension
//
///////////////////////////////////////////////////////////////////////////////

bool ShowtimeDocument::canExport ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document inserts given extension
//
///////////////////////////////////////////////////////////////////////////////

bool ShowtimeDocument::canInsert ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document writes given extension
//
///////////////////////////////////////////////////////////////////////////////

bool ShowtimeDocument::canSave ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document reads given extension
//
///////////////////////////////////////////////////////////////////////////////

bool ShowtimeDocument::canOpen ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "show" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void ShowtimeDocument::read ( const std::string &name, Unknown *caller, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( name ) ) );
  if ( "show" != ext )
    return;

  Usul::Interfaces::IStatusBar::UpdateStatusBar status ( progress );
  status ( "Loading " + name + " ... " );

  // Deserialize the xml tree.
  XmlTree::XercesLife life;
  XmlTree::Document::ValidRefPtr document ( new XmlTree::Document );
  document->load ( name );

  typedef XmlTree::Node::Children Children;
  typedef Children::const_iterator ConstItr;

  // Alway restore the current working directory.
  const std::string dir ( Usul::System::Directory::cwd() );
  Usul::Scope::CurrentDirectory cwd ( dir, false );

  // Find all model files.
  Files files ( this->_findModelFiles ( dir, document.get() ) );

  // Make a second progress bar.
  Usul::Interfaces::IProgressBarFactory::QueryPtr progressFactory ( caller );
  Usul::Interfaces::IProgressBar::QueryPtr fileProgress ( ( progressFactory.valid() ) ? progressFactory->createProgressBar ( true ) : 0x0 );
  Usul::Interfaces::IProgressBar::ShowHide showHideFileProgressBar ( fileProgress.get() );

  // Load all the models.
  this->_loadModels ( files, progress, fileProgress.get() );

  // Load the actions.
  this->_loadActions ( document );

  // Set the timer.
  this->_setTimer();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document.
//
///////////////////////////////////////////////////////////////////////////////

void ShowtimeDocument::write ( const std::string &name, Unknown *caller, Unknown *progress ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void ShowtimeDocument::clear ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  OsgTools::Group::removeAllChildren ( _scene.get() );
  _models.clear();
  _actions.clear();

  this->_removeTimer();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

ShowtimeDocument::Filters ShowtimeDocument::filtersOpen() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  filters.push_back ( Filter ( "Showtime (*.show)", "*.show" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

ShowtimeDocument::Filters ShowtimeDocument::filtersSave() const
{
  USUL_TRACE_SCOPE;
  return this->filtersOpen();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

ShowtimeDocument::Filters ShowtimeDocument::filtersExport() const
{
  USUL_TRACE_SCOPE;
  return Filters();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

ShowtimeDocument::Filters ShowtimeDocument::filtersInsert() const
{
  USUL_TRACE_SCOPE;
  return Filters();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *ShowtimeDocument::buildScene ( const BaseClass::Options &, Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Clear the scene.
  OsgTools::Group::removeAllChildren ( _scene.get() );

  // Add models to the scene.
  for ( Models::iterator i = _models.begin(); i != _models.end(); ++i )
  {
    Showtime::Model::RefPtr model ( i->second );
    if ( true == model.valid() )
    {
      _scene->addChild ( model->root().get() );
    }
  }

  // Return the scene.
  return _scene.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void ShowtimeDocument::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Do nothing if we're out of range.
  if ( _currentStep >= _numSteps )
  {
    this->_removeTimer();
    return;
  }

  std::cout << Usul::Strings::format ( "Processing step ", _currentStep, " of ", _numSteps ) << std::endl;

  // Find all actions that intersect the current step.
  Actions actions ( this->_findActions ( _currentStep ) );

  // Execute the actions.
  std::for_each ( actions.begin(), actions.end(), std::bind2nd 
    ( std::mem_fun ( &Showtime::Actions::Action::execute ), _currentStep ) );

  // Increment the step.
  ++_currentStep;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add to the menu.
//
///////////////////////////////////////////////////////////////////////////////

void ShowtimeDocument::menuAdd ( MenuKit::Menu& menuBar, Usul::Interfaces::IUnknown* caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the model.
//
///////////////////////////////////////////////////////////////////////////////

void ShowtimeDocument::addModel ( const std::string &name, Showtime::Model *model )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( 0x0 != model )
  {
    _models[name] = Showtime::Model::RefPtr ( model );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the model files in the xml document.
//
///////////////////////////////////////////////////////////////////////////////

ShowtimeDocument::Files ShowtimeDocument::_findModelFiles ( const std::string &initialDir, const XmlTree::Node *root ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Initialize.
  std::string dir ( initialDir );
  Files files;

  // Check input.
  if ( 0x0 == root )
    return files;

  typedef XmlTree::Node::Children Children;

  // Search for the models.
  Children kids ( root->find ( "models", false ) );
  for ( Children::iterator i = kids.begin(); i != kids.end(); ++i )
  {
    XmlTree::Node::RefPtr node ( i->get() );

    if ( true == node.valid() )
    {
      // Get the directory, if any.
      if ( false == node->attribute ( "directory" ).empty() )
      {
        // Switch the current directory.
        dir = node->attribute ( "directory" );
      }

      Children models ( node->find ( "model", false ) );
      for ( unsigned int j = 0; j < models.size(); ++j )
      {
        node = models.at(j).get();
        if ( true == node.valid() )
        {
          // Prepend the directory.
          const std::string path ( Usul::Strings::format ( dir, '/', node->attribute ( "file" ) ) );

          // Associate the node with the path.
          files.push_back ( FileData ( path, node ) );
        }
      }
    }
  }

  // Return the files.
  return files;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the models.
//
///////////////////////////////////////////////////////////////////////////////

void ShowtimeDocument::_loadModels ( const Files &files, Usul::Interfaces::IUnknown *progress, Usul::Interfaces::IUnknown *fileProgress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // For the progress bar.
  unsigned int count ( 0 );
  const unsigned int total ( files.size() );

  // Loop through the files.
  for ( Files::const_iterator i = files.begin(); i != files.end(); ++i )
  {
    // Get the path and node.
    const std::string path ( i->first );
    const XmlTree::Node::RefPtr node ( i->second );

    // Load the model.
    Usul::Functions::safeCallV1V2V3 ( Usul::Adaptors::memberFunction ( this, &ShowtimeDocument::_loadModel ), path, node.get(), fileProgress, "3170079787" );

    // Overall progress bar.
    this->setProgressBar ( true, ++count, total, progress );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the model.
//
///////////////////////////////////////////////////////////////////////////////

void ShowtimeDocument::_loadModel ( const std::string &path, const XmlTree::Node *node, Usul::Interfaces::IUnknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Make the model.
  Showtime::Model::RefPtr model ( new Showtime::Model ( path, node ) );

  // Should we load now?
  if ( "now" == node->attribute ( "load" ) )
  {
    model->load ( progress );
  }

  // Add the model.
  this->addModel ( model->name(), model.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the actions.
//
///////////////////////////////////////////////////////////////////////////////

void ShowtimeDocument::_loadActions ( const XmlTree::Node *root )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Check input.
  if ( 0x0 == root )
    return;

  typedef XmlTree::Node::Children Children;

  // Register factories.
  typedef Usul::Factory::BaseFactory<Usul::Base::Referenced> Factory;
  Factory factory;
  factory.add ( new Usul::Factory::TypeCreator<Showtime::Actions::Fade,Factory::BaseType> ( "fade" ) );

  // The last step.
  unsigned int lastStep ( 0 );

  // Search for the actions.
  Children kids ( root->find ( "actions", false ) );
  for ( Children::iterator i = kids.begin(); i != kids.end(); ++i )
  {
    XmlTree::Node::RefPtr node ( i->get() );

    if ( true == node.valid() )
    {
      Children actions ( node->children() );
      for ( unsigned int j = 0; j < actions.size(); ++j )
      {
        node = actions.at(j).get();
        if ( true == node.valid() )
        {
          const std::string name ( node->name() );
          Showtime::Actions::Action::RefPtr action ( dynamic_cast < Showtime::Actions::Action * > ( factory.create ( name ) ) );
          if ( true == action.valid() )
          {
            // Initialize the action.
            action->init ( this, node.get() );

            // Extend the last step.
            lastStep = Usul::Math::maximum ( action->stepRange()[1], lastStep );

            // Add the action.
            _actions.push_back ( action );
          }
        }
      }
    }
  }

  // Set the number of steps. There is always at least one.
  _numSteps = lastStep + 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the actions.
//
///////////////////////////////////////////////////////////////////////////////

ShowtimeDocument::Actions ShowtimeDocument::_findActions ( unsigned int step )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // The list of actions we return.
  Actions actions;

  // Loop through the actions.
  for ( Actions::iterator i = _actions.begin(); i != _actions.end(); ++i )
  {
    Showtime::Actions::Action::RefPtr action ( i->get() );
    if ( true == action.valid() )
    {
      if ( true == action->isInRange ( step ) )
      {
        actions.push_back ( action );
      }
    }
  }

  // Return the list.
  return actions;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the timer callback.
//
///////////////////////////////////////////////////////////////////////////////

void ShowtimeDocument::_setTimer()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  typedef Usul::Interfaces::ITimerService ITimerService;
  typedef Usul::Components::Manager PluginManager;
  
  this->_removeTimer();

  ITimerService::QueryPtr service ( PluginManager::instance().getInterface ( ITimerService::IID ) );
  if ( false == service.valid() )
    return;

  // Make a new timer.
  Usul::Interfaces::IUnknown::RefPtr me ( this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
  _timer = service->timerAdd ( 100, me );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove any existing timer.
//
///////////////////////////////////////////////////////////////////////////////

void ShowtimeDocument::_removeTimer()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  typedef Usul::Interfaces::ITimerService ITimerService;
  typedef Usul::Components::Manager PluginManager;
  
  ITimerService::QueryPtr service ( PluginManager::instance().getInterface ( ITimerService::IID ) );
  if ( false == service.valid() )
    return;

  // Remove any existing timer.
  service->timerRemove ( _timer );
  _timer = 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the timer fires.
//
///////////////////////////////////////////////////////////////////////////////

void ShowtimeDocument::timerNotify ( TimerID )
{
  USUL_TRACE_SCOPE;
  this->requestRedraw();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the model.
//
///////////////////////////////////////////////////////////////////////////////

Showtime::Model::RefPtr ShowtimeDocument::model ( const std::string &name )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  Models::iterator i ( _models.find ( name ) );
  return ( ( i == _models.end() ) ? Showtime::Model::RefPtr ( 0x0 ) : i->second );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current step.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int ShowtimeDocument::currentStep() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _currentStep;
}
