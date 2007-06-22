
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Controller.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Types/Types.h"
#include "Usul/System/Memory.h"
#include "Usul/Resources/TextWindow.h"
#include "Usul/System/Clock.h"
#include "Usul/Cast/Cast.h"
#include "Usul/State/Busy.h"
#include "Usul/Shared/Preferences.h"
#include "Usul/Registry/Constants.h"
#include "Usul/Components/Manager.h"
#include "Usul/CommandLine/Arguments.h"

#include "Usul/Interfaces/IBoundingBox.h"
#include "Usul/Interfaces/IBoundingSphere.h"
#include "Usul/Interfaces/IOpenGLState.h"
#include "Usul/Interfaces/IAxes.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Interfaces/GUI/IAboutString.h"
#include "Usul/Interfaces/GUI/ICancelButton.h"
#include "Usul/Interfaces/IExport.h"
#include "Usul/Interfaces/IFlipNormals.h"
#include "Usul/Interfaces/IMode.h"
#include "Usul/Interfaces/IStereo.h"
#include "Usul/Interfaces/ILights.h"
#include "Usul/Interfaces/IBackground.h"
#include "Usul/Interfaces/ICamera.h"
#include "Usul/Interfaces/ISpin.h"
#include "Usul/Interfaces/IInitNewDocument.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/ISceneStage.h"
#include "Usul/Interfaces/ICenterOfRotation.h"
#include "Usul/Interfaces/IDocumentSelect.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>

using namespace Usul::App;

namespace Detail
{
  // Possible stereos
  Controller::StereoModes _stereoModes;


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Helper class to populate the stereo-modes.
  //
  ///////////////////////////////////////////////////////////////////////////////

  struct PopulateStereoModes
  {
    PopulateStereoModes()
    {
      _stereoModes.push_back ( Controller::StereoMode ( 0, "Active Stereo"         ) );
      _stereoModes.push_back ( Controller::StereoMode ( 1, "Anaglyphic (Red/Blue)" ) );
      _stereoModes.push_back ( Controller::StereoMode ( 2, "Horizontal Split"      ) );
      _stereoModes.push_back ( Controller::StereoMode ( 3, "Vertical Split"        ) );
      _stereoModes.push_back ( Controller::StereoMode ( 4, "Just Left Eye"         ) );
      _stereoModes.push_back ( Controller::StereoMode ( 5, "Just Right Eye"        ) );
    }
  } _populateStereoModes;
  
  inline const Controller::StereoModes& stereoModes()
  {
    return _stereoModes;
  }

}

///////////////////////////////////////////////////////////////////////////////
//
//  Static instance.
//
///////////////////////////////////////////////////////////////////////////////

Controller *Controller::_instance ( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Singleton.
//
///////////////////////////////////////////////////////////////////////////////

Controller &Controller::instance()
{
  if ( 0x0 == _instance )
    _instance = new Controller;
  return *_instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Controller::Controller(void)
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Controller::~Controller(void)
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the active document.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Documents::Document* Controller::activeDocument()
{
  return Usul::Documents::Manager::instance().active();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the active view, which may be null.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IViewer *Controller::activeView()
{
  return ( this->activeDocument() ? this->activeDocument()->activeView() : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the polygon mode active?
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::hasPolygonMode ( IPolygonMode::Mode mode )
{
  Usul::Interfaces::IPolygonMode::QueryPtr polygon ( this->activeView() );

  return ( polygon.valid() && mode == polygon->polygonMode ( ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when a polygon mode is selected.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::setPolygonMode ( IPolygonMode::Mode mode )
{
  Usul::Interfaces::IPolygonMode::QueryPtr polygon ( this->activeView() );

  if( polygon.valid() )
  {
    // If it already has the mode remove it.
    if( mode == polygon->polygonMode( ) )
      polygon->polygonMode ( IPolygonMode::NONE );

    // Set the mode.
    else
      polygon->polygonMode ( mode );

    // Redraw.
    this->activeView()->render();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the polygon mode button be enabled?
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::polygonModeEnable ()
{
  Usul::Interfaces::IPolygonMode::QueryPtr polygon ( this->activeView() );

  return polygon.valid();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the shade model active?
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::hasShadeModel ( IShadeModel::Mode mode )
{
  Usul::Interfaces::IShadeModel::QueryPtr shading ( this->activeView() );
  return ( shading.valid() && mode == shading->shadeModel() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when a shade model is selected.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::setShadeModel ( IShadeModel::Mode mode )
{
  Usul::Interfaces::IShadeModel::QueryPtr shading ( this->activeView() );

  // If there is a valid interface.
  if( shading.valid() )
  {
    shading->shadeModel( mode );
    this->activeView()->render();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the shade model button be enabled?
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::shadeModelEnable ()
{
  Usul::Interfaces::IShadeModel::QueryPtr shading ( this->activeView() );
  return shading.valid();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::boundingBoxToggle()
{
  Usul::Interfaces::IBoundingBox::QueryPtr bb ( this->activeView() );

  // Toggle the state.
  if ( bb.valid() )
  {
    bb->boundingBox ( !bb->boundingBox() );
    this->activeView()->render();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle the bounding sphere.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::boundingSphereToggle()
{
  Usul::Interfaces::IBoundingSphere::QueryPtr bs ( this->activeView() );

  // Toggle the state.
  if ( bs.valid() )
  {
    bs->boundingSphere ( !bs->boundingSphere() );
    this->activeView()->render();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Turn stereo on or off.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::stereoSet ( bool b )
{
  Usul::Interfaces::IOpenGLState::QueryPtr gl ( this->activeView( ) );

  // Is there an active view?
  if ( gl.valid() )
  {
    gl->stereo ( b );
    this->activeView()->render();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does the active view have the axes shown.
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::axesCheck ()
{
  Usul::Interfaces::IAxes::QueryPtr axes ( this->activeView() );

  // Set checks.
  return ( axes.valid() && axes->hasAxes() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Enable the axes button
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::axesEnable ()
{
  Usul::Interfaces::IAxes::QueryPtr axes ( this->activeView() );
  return axes.valid();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Enable the axes button
//
///////////////////////////////////////////////////////////////////////////////

void Controller::axesShow ( bool b )
{
  Usul::Interfaces::IAxes::QueryPtr axes ( this->activeView() );

  if ( axes.valid() )
    axes->setAxes( b );

  this->activeView()->render();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Open document.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::documentOpen ( const std::string& filename, Usul::Interfaces::IUnknown *caller )
{
  // Typedefs.
  typedef Usul::Documents::Manager   DocManager;
  typedef DocManager::Documents      Documents;

  // Feedback.
  const Usul::Types::Uint64 usedMemory ( Usul::System::Memory::usedPhysical() );
  std::cout << "Opening document: " << filename << " ... " << Usul::Resources::TextWindow::endl;

  // Show/hide the progress bar and cancel button.
  Usul::Interfaces::IProgressBar::ShowHide progress ( caller );
  Usul::Interfaces::ICancelButton::ShowHide cancel  ( caller );

  // Check to see if there is a document already open with this filename.
  {
    // Get the set of documents that are open.
    Documents &documents ( DocManager::instance().documents() );

    // Loop through the documents.
    for( Documents::iterator i = documents.begin(); i != documents.end(); ++ i )
    {
      // Is a document open with this file name?
      if( (*i)->fileName() == filename )
      {
        // Needed to make g++ happy
        Document::RefPtr d ( i->get() );

        // Bring this document's windows to the front.
        d->windowsForward();

        // Document already open, return.
        return;
      }
    }
  }

  // Create the document.
  Documents documents ( Usul::Documents::Manager::instance().create ( filename ) );

  // Make sure...
  if ( documents.empty() )
  {
    std::ostringstream message;
    message << "Error 2565795720: Failed to create document for file: " << filename;
    throw std::runtime_error ( message.str() );
  }

  // The document to open.
  Document::RefPtr document ( 0x0 );

  Usul::Interfaces::IDocumentSelect::QueryPtr select ( caller );

  if ( select.valid() && documents.size() > 1 )
  {
    // Get the document from the user.
    document = select->selectDocument( documents );
  }
  else
  {
    // Just use the front if that's all we have, or if we don't have an interface to select a document.
    document = documents.front();
  }

  // Initialize start time.
  Usul::Types::Uint64 start ( Usul::System::Clock::milliseconds() );

  // Open the document.
  document->open ( filename, caller );

  // Find a delegate for this document
  Usul::Documents::Manager::instance().delegate ( document.get() );

  // Open new window with the document.
  document->createDefaultGUI ( caller );

  // Add the document to the document managers list
  // Need to do this here instead of in the document manager,
  // because a cancel exception can be thrown and the document
  // will not be deleted because it's still in the manager.
  Usul::Documents::Manager::instance().add ( document.get() );

  // Feedback.
  ::printf ( "%8.4f seconds .... Total time to open new document.\n", static_cast < double > ( Usul::System::Clock::milliseconds() - start ) * 0.001 ); ::fflush ( stdout );
  std::cout << "Memory required: " << ( static_cast < double > ( Usul::System::Memory::usedPhysical() - usedMemory ) * 0.001 ) << " Kilobytes" << std::endl;
  std::cout << "Done opening document" << Usul::Resources::TextWindow::endl;

}


///////////////////////////////////////////////////////////////////////////////
//
//  Enable the bounding box button.
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::boundingBoxEnable ()
{
  Usul::Interfaces::IBoundingBox::QueryPtr bb ( this->activeView() );
  return bb.valid();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check the bounding box button.
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::boundingBoxCheck  ()
{
  Usul::Interfaces::IBoundingBox::QueryPtr bb ( this->activeView() );
  return bb.valid() && bb->boundingBox();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Enable the bounding sphere button.
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::boundingSphereEnable ()
{
  Usul::Interfaces::IBoundingSphere::QueryPtr bs ( this->activeView() );
  return bs.valid();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check the bounding sphere button.
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::boundingSphereCheck  ()
{
  Usul::Interfaces::IBoundingSphere::QueryPtr bs ( this->activeView() );
  return bs.valid() && bs->boundingSphere();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the about message
//
///////////////////////////////////////////////////////////////////////////////

std::string Controller::aboutMessage ( )
{
  typedef Usul::Components::Manager Manager;
  typedef Manager::UnknownSet Plugins;
  Plugins plugins ( Manager::instance().getInterfaces ( Usul::Interfaces::IAboutString::IID ) );

  std::string about;

  for ( Plugins::const_iterator iter = plugins.begin(); iter != plugins.end(); ++iter )
  {
    Usul::Interfaces::IAboutString::ValidQueryPtr ptr ( iter->get() );
    about += ptr->aboutString();
  }

  return about;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Insert document.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::documentInsert ( Usul::Interfaces::IUnknown* caller )
{
    // Set busy to true, make sure it resets
  Usul::State::Busy busy;

  // Insert some files.
  if ( this->activeDocument() )
    this->activeDocument()->insert ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Save document.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::documentSave   ( Usul::Interfaces::IUnknown* caller )
{
  // Prompt to save the file as another name.
  if ( this->activeDocument() )
    this->activeDocument()->save ( caller );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Document save as.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::documentSaveAs ( Usul::Interfaces::IUnknown* caller )
{
  // Prompt to save the file as another name.
  if ( this->activeDocument() )
    this->activeDocument()->saveAs ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters for exporting.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::filtersExport ( Filters &filters )
{
  // Return if there is no view or no document.
  if ( 0x0 == this->activeView() || 0x0 == this->activeDocument() )
    return;

  // Get the interface
  Usul::Interfaces::IExport::QueryPtr exportData ( this->activeView() );

  // Get the filters for the document
  const Filters df ( this->activeDocument()->filtersExport() );
  Filters all ( df.begin(), df.end() );

  // Get the filters for the view, if we have a valid interface
  if( exportData.valid() )
  {
    const Filters vi ( exportData->filtersExport() );
    all.insert ( all.end(), vi.begin(), vi.end() );
  }
  
  // Sort the filters and make sure we only have unique ones
  std::sort ( all.begin(), all.end() );
  Filters f ( all.begin(), std::unique ( all.begin(), all.end() ) );
  filters = f;

}


///////////////////////////////////////////////////////////////////////////////
//
//  Export document into filename.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::documentExport ( const std::string& filename, std::string filter, Usul::Interfaces::IUnknown* caller )
{
  // Return if there is no view or no document.
  if ( 0x0 == this->activeView() || 0x0 == this->activeDocument() )
    return;

  // Get the interface
  Usul::Interfaces::IExport::QueryPtr exportData ( this->activeView() );

    // See if the document can export this.
  if ( this->activeDocument()->canExport ( filename ) )
  {
    Usul::Documents::Document::Options &options ( this->activeDocument()->options() );
    
    std::transform ( filter.begin(), filter.end(), filter.begin(), ::tolower );
    std::string format ( ( std::string::npos != filter.find ( "binary" ) ) ? "binary" : "ascii" );
    
    options["format"] = format;

    this->activeDocument()->write ( filename, caller );
    return;
  }

  if( exportData.valid() && exportData->canExport ( filename ) )
  {
    // See if the view can export this.
    if ( exportData->exportFile ( filename ) )
      return;
  }

  // If we get here then we did not successfully export the file
  throw std::runtime_error ( "Error 2154590424: Failed to export file: " + filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Flip normals of active document.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::flipNormals ( Usul::Interfaces::IUnknown* caller )
{
  // See if active document supports flipping normals.
  Usul::Interfaces::IFlipNormals::QueryPtr flip ( this->activeDocument() );
  if ( flip.valid() )
  {
    // Show/hide the progress bar and cancel button.
    Usul::Interfaces::IProgressBar::ShowHide progress ( caller );
    Usul::Interfaces::ICancelButton::ShowHide cancel  ( caller );

    // Do the flipping.
    flip->flipNormalVectors();
    this->activeDocument()->sendMessage ( Document::ID_CLEAR_SCENE  );
    this->activeDocument()->sendMessage ( Document::ID_BUILD_SCENE  );
    this->activeDocument()->sendMessage ( Document::ID_RENDER_SCENE );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Enable flip normals button.
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::flipNormalsEnable ( )
{
  Usul::Interfaces::IFlipNormals::QueryPtr flip ( this->activeDocument() );
  return flip.valid();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the two sided lighting state.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::lightingTwoSidedSet ( bool b )
{
  Usul::Interfaces::IOpenGLState::QueryPtr gl ( this->activeView( ) );

  // Do we have a valid interface.
  if ( gl.valid() )
  {
    gl->twoSidedLighting ( b );
    this->activeView()->render();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the two sided lighting button be checked.
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::lightingTwoSidedCheck  ()
{
  Usul::Interfaces::IOpenGLState::QueryPtr gl ( this->activeView( ) );

  return gl.valid() && gl->twoSidedLighting();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the two sided lighting button be enabled.
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::lightingTwoSidedEnable ()
{
  Usul::Interfaces::IOpenGLState::QueryPtr gl ( this->activeView( ) );

  return gl.valid() && gl->lighting();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the lighting state.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::lightingSet ( bool b )
{
  Usul::Interfaces::IOpenGLState::QueryPtr gl ( this->activeView( ) );

  // If we have a valid interface.
  if ( gl.valid() )
  {
    gl->lighting ( b );
    this->activeView()->render();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the lighting button be checked.
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::lightingCheck ()
{
  Usul::Interfaces::IOpenGLState::QueryPtr gl ( this->activeView( ) );
  return gl.valid() && gl->lighting();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the lighting button be enabled.
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::lightingEnable ()
{
  Usul::Interfaces::IOpenGLState::QueryPtr gl ( this->activeView( ) );
  return gl.valid();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the stereo button be checked.
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::stereoCheck ()
{
  Usul::Interfaces::IOpenGLState::QueryPtr gl ( this->activeView( ) );
  return gl.valid() && gl->stereo();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the stereo button be enabled.
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::stereoEnable()
{
  Usul::Interfaces::IOpenGLState::QueryPtr gl ( this->activeView( ) );
  return gl.valid();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the navigation button be checked.
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::modeNavigatingCheck ( )
{
  // Enable if there is a view, set check if view is in correct state.
  Usul::Interfaces::IMode::QueryPtr mode ( this->activeView( ) );
  return mode.valid() && mode->navigating() && !mode->tool();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the navigation button be enabled.
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::modeNavigatingEnable ( )
{
  // Enable if there is a view, set check if view is in correct state.
  Usul::Interfaces::IMode::QueryPtr mode ( this->activeView( ) );
  return mode.valid() && !Usul::State::busy();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the mode to navigation.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::modeNavigatingSet ( )
{
  Usul::Interfaces::IMode::QueryPtr mode ( this->activeView( ) );

  // Set the mode to navigation if there is an active view.
  if ( mode.valid() )
    mode->navigating ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the picking button be checked.
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::modePickingCheck ( )
{
  // Enable if there is a view, set check if view is in correct state.
  Usul::Interfaces::IMode::QueryPtr mode ( this->activeView( ) );
  return mode.valid() && mode->picking() && !mode->tool();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the picking button be enabled.
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::modePickingEnable ( )
{
  // Enable if there is a view, set check if view is in correct state.
  Usul::Interfaces::IMode::QueryPtr mode ( this->activeView( ) );
  return mode.valid() && !Usul::State::busy();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the mode to picking.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::modePickingSet ( )
{
  Usul::Interfaces::IMode::QueryPtr mode ( this->activeView( ) );

  // Set the mode to pick if there is an active view
  if ( mode.valid() )
    mode->picking ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the possible stereo modes.
//
///////////////////////////////////////////////////////////////////////////////

const Controller::StereoModes& Controller::stereoModes()
{
  return Detail::stereoModes();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the stereo mode.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::stereoModeSet ( unsigned int mode )
{
  Usul::Interfaces::IStereo::QueryPtr stereo ( this->activeView() );
  if ( stereo.valid() )
  {
    stereo->stereoMode( mode );
    
    this->activeView()->render();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the given stereo mode be checked.
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::stereoCheck ( unsigned int mode )
{
  Usul::Interfaces::IStereo::QueryPtr stereo ( this->activeView() );
  return stereo.valid() && mode == stereo->stereoMode();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the stereo eye distance.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::stereoEyeDistanceSet ( float distance )
{
  Usul::Interfaces::IStereo::QueryPtr stereo ( this->activeView() );
  if ( stereo.valid() )
  {
    stereo->stereoEyeDistance( distance );
    this->activeView()->render();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the eye distance button be checked.
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::stereoEyeDistanceCheck ( float distance )
{
  Usul::Interfaces::IStereo::QueryPtr stereo ( this->activeView() );
  return stereo.valid() && distance == stereo->stereoEyeDistance();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the show lights state.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::lightsShow ( bool b )
{
  Usul::Interfaces::ILights::QueryPtr lights ( this->activeView() );

  if ( lights.valid() )
    lights->setLights( b );

  this->activeView()->render();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the lights button be chekced.
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::lightsCheck ()
{
  Usul::Interfaces::ILights::QueryPtr lights ( this->activeView() );
  return lights.valid() && lights->hasLights();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the lights button be enabled.
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::lightsEnable ()
{
  Usul::Interfaces::ILights::QueryPtr lights ( this->activeView() );
  return lights.valid();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the backgroud color to the default.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::backgroundDefault()
{
  Usul::Interfaces::IBackground::QueryPtr background ( this->activeView() );

  // If there is an active view
  if( background.valid() )
    background->defaultBackground();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Edit the backgroud color.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::backgroundEdit()
{
  Usul::Interfaces::IBackground::QueryPtr background ( this->activeView() );

  // If there is an active view
  if( background.valid() )
    background->editBackground();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the background button be enabled.
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::backgroundEnable()
{
  Usul::Interfaces::IBackground::QueryPtr background ( this->activeView() );
  return background.valid();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the camera view to the given option.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::camera ( unsigned int option )
{
  Usul::Interfaces::ICamera::QueryPtr camera ( this->activeView() );
  Usul::Interfaces::ISpin::QueryPtr   spin   ( this->activeView() );
  
  if ( camera.valid() )
  {
    // Stop any spinning
    if( spin.valid() )
      spin->spin ( false );

    // move the camera, animate the motion.
    camera->camera ( Usul::Interfaces::ICamera::CameraOption ( option ) );
    this->activeView()->render();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the camera button be enabled.
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::cameraEnable ()
{
  Usul::Interfaces::ICamera::QueryPtr camera ( this->activeView() );
  return camera.valid();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the display lists state.
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::displayLists()
{
  return Usul::Shared::Preferences::instance().getBool ( Usul::Registry::Keys::DISPLAY_LISTS );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle the display lists state.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::displayListsToggle()
{
  // Set the state and tell all documents to update.
  Usul::Shared::Preferences &prefs ( Usul::Shared::Preferences::instance() );
  const bool state ( prefs.getBool ( Usul::Registry::Keys::DISPLAY_LISTS ) );
  prefs.setBool ( Usul::Registry::Keys::DISPLAY_LISTS, !state );
  Usul::Documents::Manager::instance().sendMessage ( Usul::Interfaces::ISendMessage::ID_DISPLAY_LISTS_UPDATE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle the high lods state.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::highLods()
{
  typedef Usul::Shared::Preferences Prefs;
  const bool state ( Prefs::instance().getBool ( Usul::Registry::Keys::HIGH_LODS ) );
  Prefs::instance().setBool ( Usul::Registry::Keys::HIGH_LODS, !state );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the high lods button be checked.
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::highLodsCheck()
{
  return Usul::Shared::Preferences::instance().getBool ( Usul::Registry::Keys::HIGH_LODS );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle the low lods state.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::lowLods()
{
  typedef Usul::Shared::Preferences Prefs;
  const bool state ( Prefs::instance().getBool ( Usul::Registry::Keys::LOW_LODS ) );
  Prefs::instance().setBool ( Usul::Registry::Keys::LOW_LODS, !state );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the low lods button be checked.
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::lowLowsCheck()
{
  return Usul::Shared::Preferences::instance().getBool ( Usul::Registry::Keys::LOW_LODS );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open a new window for the active document.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::newWindow ( Usul::Interfaces::IUnknown* caller )
{
  // Show/hide the progress bar and cancel button.
  Usul::Interfaces::IProgressBar::ShowHide progress ( caller );
  Usul::Interfaces::ICancelButton::ShowHide cancel  ( caller );

  // Open new window for active document.
  Document::RefPtr doc ( this->activeDocument() );
  if ( doc.valid() )
    doc->createDefaultGUI ( caller );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle the allow spin state.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::spinToggle ()
{
  typedef Usul::Shared::Preferences Prefs;
  const bool state ( Prefs::instance().getBool ( Usul::Registry::Keys::ALLOW_SPIN ) );
  Prefs::instance().setBool ( Usul::Registry::Keys::ALLOW_SPIN, !state );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the allow spin button be checked.
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::spinCheck ()
{
  typedef Usul::Shared::Preferences Prefs;
  return Prefs::instance().getBool ( Usul::Registry::Keys::ALLOW_SPIN );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open a new document.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::documentNew ( Usul::Interfaces::IUnknown* component, Usul::Interfaces::IUnknown* caller )
{
    // Create an empty document
  Document::RefPtr doc ( Usul::Documents::Manager::instance().create( component, caller ) );

  // Return now if not valid
  if( !doc.valid() )
    return;

  // Find a delegate for this document
  Usul::Documents::Manager::instance().delegate ( doc.get() );

  // Get the query ptr
  Usul::Interfaces::IInitNewDocument::QueryPtr init ( doc.get() );

  // Initialize new document if it implements the interface
  if( init.valid() )
    init->initNewDocument( caller );

  // Create the default gui
  doc->createDefaultGUI( caller );

  // Add the document to the document managers list.
  // Need to do this here instead of in the document manager,
  // because a cancel exception can be thrown above and the document
  // will not be deleted because it's still in the manager.
  Usul::Documents::Manager::instance().add( doc.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load all plugins.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::loadPlugins()
{
  typedef Usul::Components::Manager PluginManager;

  // Get the directory where the application lives.
  const std::string dir ( Usul::CommandLine::Arguments::instance().directory() );
   
  PluginManager::instance().addDirectory( dir );

  // Load all plugins.
  PluginManager::instance().load ( Usul::Interfaces::IPlugin::IID );

  // Feedback about plugins.
  const PluginManager::Strings names ( PluginManager::instance().names() );
  std::cout << "Found " << names.size() << " plugins: ";
  std::copy ( names.begin(), names.end(), std::ostream_iterator<std::string> ( std::cout, "; " ) );
  std::cout << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the scene stage button be checked?
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::sceneStage()
{
  Usul::Interfaces::ISceneStage::QueryPtr stage ( this->activeView() );
  return stage.valid() && stage->sceneStage();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the scene stage state.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::sceneStage ( bool b )
{
  Usul::Interfaces::ISceneStage::QueryPtr stage ( this->activeView() );

  if ( stage.valid() )
  {
    stage->sceneStage( b );
    this->activeView()->render();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the center of rotation.
//
///////////////////////////////////////////////////////////////////////////////
  
void Controller::centerOfRotationSet ( bool b )
{
  Usul::Interfaces::ICenterOfRotation::QueryPtr cor ( this->activeView() );
  if ( cor.valid() )
  {
    cor->showCenterOfRotation ( b );
    this->activeView()->render();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the center of rotation checked?
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::centerOfRotationCheck ()
{
  Usul::Interfaces::ICenterOfRotation::QueryPtr cor ( this->activeView() );
  return cor.valid() && cor->showCenterOfRotation();
}
