
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Glue code for OsgTools::Render::Viewer
//
///////////////////////////////////////////////////////////////////////////////

#include "Viewer.h"

#include "Threads/OpenThreads/Mutex.h"

#include "OsgTools/Jitter.h"

#include "Usul/Threads/Mutex.h"
#include "Usul/Math/Absolute.h"
#include "Usul/Math/MinMax.h"

using namespace CadKit::Viewer::Glue;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Viewer() : _viewer( 0x0 )
{
  Usul::Threads::SetMutexFactory factory ( &Threads::OT::newOpenThreadsMutex );
  _viewer = new OsgTools::Render::Viewer( 0x0, 0x0, 0x0 );
  Usul::Pointers::reference( _viewer );
  _viewer->axes( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::~Viewer()
{
  Usul::Pointers::unreference( _viewer );
  _viewer = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Finalizer.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::!Viewer()
{
  Usul::Pointers::unreference( _viewer );
  _viewer = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::render()
{
  if ( _viewer )
    _viewer->render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Resize the viewport.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::resize( int w, int h )
{
  if ( _viewer )
    _viewer->resize( w, h );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the background color.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::backgroundColor ( System::Drawing::Color^ color )
{
  if ( nullptr == color || 0x0 == _viewer )
    return;

  // Normalize to [0,1].
  const float r ( Usul::Math::minimum ( 1.0f, Usul::Math::maximum ( 0.0f, static_cast<float> ( color->R ) / 255 ) ) );
  const float g ( Usul::Math::minimum ( 1.0f, Usul::Math::maximum ( 0.0f, static_cast<float> ( color->G ) / 255 ) ) );
  const float b ( Usul::Math::minimum ( 1.0f, Usul::Math::maximum ( 0.0f, static_cast<float> ( color->B ) / 255 ) ) );
  const float a ( Usul::Math::minimum ( 1.0f, Usul::Math::maximum ( 0.0f, static_cast<float> ( color->A ) / 255 ) ) );

  // Pass along to the viewer.
  _viewer->backgroundColor ( osg::Vec4 ( r, g, b, a ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the background color.
//
///////////////////////////////////////////////////////////////////////////////

System::Drawing::Color^ Viewer::backgroundColor()
{
  if ( 0x0 == _viewer )
    return System::Drawing::Color::Black;

  // Get viewer's color.
  osg::Vec4 color ( _viewer->backgroundColor() );

  // Normalize to [0,255].
  const int r ( Usul::Math::minimum ( 255, Usul::Math::maximum ( 0, static_cast<int> ( color[0] * 255 ) ) ) );
  const int g ( Usul::Math::minimum ( 255, Usul::Math::maximum ( 0, static_cast<int> ( color[1] * 255 ) ) ) );
  const int b ( Usul::Math::minimum ( 255, Usul::Math::maximum ( 0, static_cast<int> ( color[2] * 255 ) ) ) );
  const int a ( Usul::Math::minimum ( 255, Usul::Math::maximum ( 0, static_cast<int> ( color[3] * 255 ) ) ) );

  // Return new color.
  return System::Drawing::Color::FromArgb ( a, r, g, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the background corners.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::backgroundCorners ( Viewer::Corners corners )
{
  if ( 0x0 != _viewer )
    _viewer->backgroundCorners ( static_cast<unsigned int> ( corners ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the background corners.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Corners Viewer::backgroundCorners()
{
  return ( ( 0x0 == _viewer ) ? Viewer::Corners::ALL : ( static_cast<Corners> ( _viewer->backgroundCorners() ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  A button was pressed.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::buttonPress ( float x, float y, bool left, bool middle, bool right )
{
  if ( _viewer )
    _viewer->buttonPress ( x, y, left, middle, right );
}


///////////////////////////////////////////////////////////////////////////////
//
//  A button was released
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::buttonRelease ( float x, float y, bool left, bool middle, bool right )
{
  if ( _viewer )
    _viewer->buttonRelease ( x, y, left, middle, right );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the viewer.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::clear()
{
  if ( _viewer )
    _viewer->clear();
  Usul::Pointers::unreference( _viewer );
  _viewer = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the viewer.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::create()
{
  if ( _viewer )
    _viewer->create();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle Navigation.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::handleNavigation ( float x, float y, bool left, bool middle, bool right, Type type )
{
  if ( _viewer )
    _viewer->handleNavigation ( x, y, left, middle, right, static_cast < osgGA::GUIEventAdapter::EventType > ( type ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle Seek.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::handleSeek ( float x, float y, bool left )
{
  if ( _viewer )
    _viewer->handleSeek ( x, y, left );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Move the camera.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::camera ( CadKit::Interfaces::CameraOption option )
{
  _viewer->camera( static_cast < OsgTools::Render::Viewer::CameraOption > ( option ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the mode.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setMode ( ViewMode mode )
{
  _viewer->setMode( static_cast < OsgTools::Render::Viewer::ViewMode > ( mode ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the mode.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::ViewMode Viewer::getMode ()
{
  return static_cast < CadKit::Viewer::Glue::Viewer::ViewMode > ( _viewer->getMode() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewer.
//
///////////////////////////////////////////////////////////////////////////////

System::IntPtr Viewer::viewer()
{
  return System::IntPtr ( _viewer );
}


///////////////////////////////////////////////////////////////////////////////
//
// Write the current frame to an image file.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::writeImageFile ( System::String^ filename )
{
  return _viewer->writeImageFile ( this->_toString ( filename ) );
}


///////////////////////////////////////////////////////////////////////////////
//
// Write the current model to file.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::writeModelFile ( System::String^ filename )
{
  return _viewer->writeModelFile ( this->_toString ( filename ) );
}


///////////////////////////////////////////////////////////////////////////////
//
// Write the current scene to file.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::writeSceneFile ( System::String^ filename )
{
  return _viewer->writeSceneFile ( this->_toString ( filename ) );
}


///////////////////////////////////////////////////////////////////////////////
//
// Return the number of render passes.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Viewer::numRenderPasses()
{
  return ( ( 0x0 == _viewer ) ? 0 : _viewer->numRenderPasses() );
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the number of render passes.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::numRenderPasses ( unsigned int num )
{
  if ( 0x0 != _viewer )
  {
    _viewer->numRenderPasses ( num );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scatter scale.
//
///////////////////////////////////////////////////////////////////////////////

double Viewer::scatterScale()
{
  return ( ( 0x0 == _viewer ) ? 0 : _viewer->scatterScale() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the scatter scale.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::scatterScale ( double scale )
{
  if ( 0x0 != _viewer )
  {
    _viewer->scatterScale ( scale );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the frame-dump scale.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::frameDumpScale ( float scale )
{
  if ( 0x0 != _viewer )
  {
    _viewer->frameDump().scale ( scale );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the frame-dump size.
//
///////////////////////////////////////////////////////////////////////////////

float Viewer::frameDumpScale()
{
  return ( 0x0 != _viewer ) ? _viewer->frameDump().scale() : 1.0f;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert managed string to namtive.
//
///////////////////////////////////////////////////////////////////////////////

std::string Viewer::_toString( System::String^ source )
{
  std::string answer;
  System::IntPtr ptr ( 0 );
  try
  {
    ptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( source );
    char* s = (char*)(void*) ptr;
    answer.assign ( s );
  }
  finally
  {
    System::Runtime::InteropServices::Marshal::FreeHGlobal( ptr );
  }
  return answer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get list of available render passes.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::RenderPasses^ Viewer::availableRenderPasses()
{
  OsgTools::Jitter::Available available = OsgTools::Jitter::instance().available();
  Viewer::RenderPasses^ passes = gcnew Viewer::RenderPasses;
  for ( OsgTools::Jitter::Available::const_iterator i = available.begin(); i != available.end(); ++i )
    passes->Add ( i->first );
  return passes;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Use display lists.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::useDisplayLists()
{
  return ( 0x0 != _viewer ) ? _viewer->useDisplayLists() : true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Use display lists.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::useDisplayLists ( bool state )
{
  if ( 0x0 != _viewer )
    _viewer->useDisplayLists ( state );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Use display lists.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::updateDisplayListUse()
{
  if ( 0x0 != _viewer )
    _viewer->updateDisplayListUse();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct Convert
  {
    static osg::PolygonMode::Face face ( CadKit::Interfaces::PolygonMode::Face f )
    {
      switch ( f )
      {
      case CadKit::Interfaces::PolygonMode::Face::FRONT:
        return osg::PolygonMode::FRONT;
      case CadKit::Interfaces::PolygonMode::Face::BACK:
        return osg::PolygonMode::BACK;
      default:
        return osg::PolygonMode::FRONT_AND_BACK;
      }
    }
    static CadKit::Interfaces::PolygonMode::Face face ( osg::PolygonMode::Face f )
    {
      switch ( f )
      {
      case osg::PolygonMode::FRONT:
        return CadKit::Interfaces::PolygonMode::Face::FRONT;
      case osg::PolygonMode::BACK:
        return CadKit::Interfaces::PolygonMode::Face::BACK;
      case osg::PolygonMode::FRONT_AND_BACK:
        return CadKit::Interfaces::PolygonMode::Face::FRONT_AND_BACK;
      default:
        return CadKit::Interfaces::PolygonMode::Face::NONE;
      }
    }
    static osg::PolygonMode::Mode mode ( CadKit::Interfaces::PolygonMode::Mode m )
    {
      switch ( m )
      {
      case CadKit::Interfaces::PolygonMode::Mode::POINT:
        return osg::PolygonMode::POINT;
      case CadKit::Interfaces::PolygonMode::Mode::LINE:
        return osg::PolygonMode::LINE;
      default:
        return osg::PolygonMode::FILL;
      }
    }
    static CadKit::Interfaces::PolygonMode::Mode mode ( osg::PolygonMode::Mode m )
    {
      switch ( m )
      {
      case osg::PolygonMode::POINT:
        return CadKit::Interfaces::PolygonMode::Mode::POINT;
      case osg::PolygonMode::LINE:
        return CadKit::Interfaces::PolygonMode::Mode::LINE;
      case osg::PolygonMode::FILL:
        return CadKit::Interfaces::PolygonMode::Mode::FILL;
      default:
        return CadKit::Interfaces::PolygonMode::Mode::NONE;
      }
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::PolygonMode::Mode Viewer::getPolygonMode ( PolygonMode::Face face )
{
  if ( 0x0 == _viewer )
    return PolygonMode::Mode::NONE;

  if ( _viewer->hasPolygonMode ( Detail::Convert::face ( face ), osg::PolygonMode::FILL ) )
    return CadKit::Interfaces::PolygonMode::Mode::FILL;

  if ( _viewer->hasPolygonMode ( Detail::Convert::face ( face ), osg::PolygonMode::LINE ) )
    return CadKit::Interfaces::PolygonMode::Mode::LINE;

  if ( _viewer->hasPolygonMode ( Detail::Convert::face ( face ), osg::PolygonMode::POINT ) )
    return CadKit::Interfaces::PolygonMode::Mode::POINT;

  return CadKit::Interfaces::PolygonMode::Mode::NONE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setPolygonMode ( PolygonMode::Face face, PolygonMode::Mode mode )
{
  if ( 0x0 != _viewer )
    _viewer->setPolygonMode ( Detail::Convert::face ( face ), Detail::Convert::mode ( mode ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Do we have the polygon mode?
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::hasPolygonMode ( PolygonMode::Face face, PolygonMode::Mode mode )
{
  return ( 0x0 == _viewer ) ? false : _viewer->hasPolygonMode ( Detail::Convert::face ( face ), Detail::Convert::mode ( mode ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Do we have the polygon mode?
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::hasPolygonMode ( PolygonMode::Face face )
{
  return ( 0x0 == _viewer ) ? false : _viewer->hasPolygonMode ( Detail::Convert::face ( face ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Do we have the polygon mode?
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::hasPolygonMode()
{
  return ( 0x0 == _viewer ) ? false : _viewer->hasPolygonMode();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::removePolygonMode()
{
  if ( 0x0 != _viewer )
    _viewer->removePolygonMode();
}
