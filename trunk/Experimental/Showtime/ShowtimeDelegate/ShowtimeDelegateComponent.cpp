
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Showtime/ShowtimeDelegate/ShowtimeDelegateComponent.h"
#include "Showtime/ShowtimeDelegate/Viewer.h"

#include "Usul/Components/Factory.h"
#include "Usul/Components/Manager.h"
#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IAnimateMatrices.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/Qt/IWorkspace.h"
#include "Usul/Interfaces/ISceneGraph.h"
#include "Usul/Math/Constants.h"
#include "Usul/Math/Functions.h"
#include "Usul/Registry/Constants.h"
#include "Usul/Registry/Database.h"

#include "QtGui/QWorkspace"

#include "osg/Node"
#include "osg/ref_ptr"

namespace Sections = Usul::Registry::Sections;
namespace Keys = Usul::Registry::Keys;
typedef Usul::Registry::Database Reg;

USUL_DECLARE_COMPONENT_FACTORY ( ShowtimeDelegateComponent );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ShowtimeDelegateComponent, ShowtimeDelegateComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ShowtimeDelegateComponent::ShowtimeDelegateComponent() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ShowtimeDelegateComponent::~ShowtimeDelegateComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *ShowtimeDelegateComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin*>(this);
  case Usul::Interfaces::IDefaultGUIDelegate::IID:
    return static_cast < Usul::Interfaces::IDefaultGUIDelegate * > ( this );
  case Usul::Interfaces::IGUIDelegate::IID:
    return static_cast < Usul::Interfaces::IGUIDelegate * > ( this );
  default:
    return 0x0;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Does this delegate handle the given token?
//
/////////////////////////////////////////////////////////////////////////////

bool ShowtimeDelegateComponent::doesHandle ( const std::string& token ) const
{
  return ( ( token == "Showtime Document" ) || ( token == "Scene Document" ) );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Build the default GUI
//
/////////////////////////////////////////////////////////////////////////////

void ShowtimeDelegateComponent::createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller )
{
  Usul::Interfaces::Qt::IWorkspace::QueryPtr ws ( caller );
  QWorkspace *workspace ( ws.valid() ? ws->workspace() : 0x0 );

  Showtime::Viewer::RefPtr viewer ( new Showtime::Viewer ( Usul::Interfaces::IUnknown::QueryPtr ( document ), QGLFormat::defaultFormat(), workspace, caller ) );
  if ( 0x0 != workspace )
  {
    workspace->addWindow ( viewer.get() );
  }

  // We want mouse-move events even when there are no mouse buttons pressed.
  viewer->setMouseTracking ( true );

  // Get the viewer on the screen.
  viewer->stateLoad();
  viewer->show();
  viewer->setFocus();

  // Need the internal viewer too.
  Showtime::Viewer::ViewerPtr internalViewer ( viewer->viewer() );
  if ( false == internalViewer.valid() )
    return;

  // Need this interface.
  Usul::Interfaces::IBuildScene::QueryPtr build ( document );
  if ( false == build.valid() )
    return;

  // Get the node factory.
  typedef Usul::Interfaces::SceneGraph::IFactory IFactory;
  IFactory::QueryPtr factory ( Usul::Components::Manager::instance().getInterface ( IFactory::IID ) );
  if ( false == factory.valid() )
    return;

  // Build the scene and add it to the internal viewer.
  Usul::Documents::Document::Options options ( ( 0x0 != document ) ? document->options() : Usul::Documents::Document::Options() );
  osg::ref_ptr<osg::Node> node ( build->buildScene ( options, caller ) );

  // Wrap the node.
  Need to query for Usul::Interfaces::SceneGraph::OSG::IFactory and call "createWrapperObject ( node )"

  IUnkown::RefPtr wrapper ( factory->createObject (
  internalViewer->modelAdd ( node );

  // See if we can animate the view-all operation.
  typedef Usul::Interfaces::IAnimateMatrices IAnimateMatrices;
  typedef IAnimateMatrices::Matrices Matrices;
  IAnimateMatrices::QueryPtr animate ( Usul::Components::Manager::instance().getInterface ( IAnimateMatrices::IID ) );
  if ( true == animate.valid() )
  {
    const unsigned int milliSeconds ( Reg::instance()[Sections::PATH_ANIMATION]["view_all"]["milliseconds"].get<unsigned int> ( 15, true ) );
    const unsigned int numFrames    ( Reg::instance()[Sections::PATH_ANIMATION]["view_all"]["num_frames"].get<unsigned int>   ( 20, true ) );
    Matrices matrices;
    matrices.reserve ( numFrames );
    const Matrices::value_type va ( internalViewer->navigationViewAll() );
    const Matrices::value_type m0 ( va * va * va * va );
    const Matrices::value_type m1 ( va );
    const Usul::Math::Vec3d p0 ( m0.getTranslation<Usul::Math::Vec3d>() );
    const Usul::Math::Vec3d p1 ( m1.getTranslation<Usul::Math::Vec3d>() );
    for ( unsigned int i = 0; i < numFrames; ++i )
    {
      double u ( static_cast<double> ( i ) / static_cast<double> ( numFrames - 1 ) );
      u *= Usul::Math::PIE;
      u += Usul::Math::PIE;
      u  = Usul::Math::cos ( u );
      u += 1.0;
      u /= 2.0;
      const Usul::Math::Vec3d p ( p0 + u * ( p1 - p0 ) );
      matrices.push_back ( Matrices::value_type::translation ( p ) );
    }
    animate->animateMatrices ( matrices, milliSeconds, false );
  }
  else
  {
    internalViewer->navigationMatrix ( internalViewer->navigationViewAll() );
  }
}
