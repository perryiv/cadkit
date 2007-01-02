
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Panel.h"

#include "OsgTools/Render/Defaults.h"
#include "OsgTools/Render/EventAdapter.h"
#include "OsgTools/Render/ActionAdapter.h"

#include "osgDB/Registry"

#include "osgUtil/UpdateVisitor"

#include "ossimPlanet/ossimPlanetSceneView.h"
#include "ossimPlanet/ossimPlanetDatabasePager.h"
#include "ossimPlanet/ossimPlanetManipulator.h"

using namespace CadKit::Plugins::Documents::OSSIMPlanetDocument::Glue;


Panel::Panel() : BaseClass()
{
  this->_init();
}

Panel::~Panel()
{
  this->_clear();
}

Panel::!Panel()
{
  this->_clear();
}


void Panel::init()
{
  this->_init();
}

void Panel::clear()
{
}


void Panel::_init()
{
  this->initRenderingContext();

  if( 0x0 == _renderer )
  {
    _renderer = new OsgTools::Render::Renderer();
    _renderer->ref();

    osg::ref_ptr < ossimPlanetSceneView > sceneView ( new ossimPlanetSceneView );

    sceneView->setFrameStamp ( _renderer->framestamp() );

    // Set the update-visitor.
    sceneView->setUpdateVisitor ( new osgUtil::UpdateVisitor );

    sceneView->setCullVisitor( new osgUtil::CullVisitor );

    // Set the display settings.
    sceneView->setDisplaySettings ( new osg::DisplaySettings() );

    // Set the viewer's defaults.
    sceneView->setDefaults();

    _renderer->viewer( sceneView.get() );

    _renderer->init();
  }

  if( 0x0 == _databasePager )
  {
    _databasePager = new ossimPlanetDatabasePager;
    _databasePager->ref();
    _databasePager->setUseFrameBlock(true);
    _renderer->viewer()->getCullVisitor()->setDatabaseRequestHandler( _databasePager );
    _renderer->viewer()->getUpdateVisitor()->setDatabaseRequestHandler( _databasePager );
    osgDB::Registry::instance()->setDatabasePager( _databasePager );
  }

  if( 0x0 == _matrixManipulator )
  {
    _matrixManipulator = new ossimPlanetManipulator;
    _matrixManipulator->ref();
  }
}

void Panel::_clear()
{
  if( 0x0 != _renderer )
  {
    _renderer->unref();
    _renderer = 0x0;
  }

  if( 0x0 != _databasePager )
  {
    _databasePager->unref();
    _databasePager = 0x0;
  }

  if( 0x0 != _matrixManipulator )
  {
    _matrixManipulator->unref();
    _matrixManipulator = 0x0;
  }
}

void Panel::_paintOpenGL()
{
  this->_makeCurrent();

  _renderer->render();

  this->_swapBuffers();
}

void Panel::_resizeOpenGL()
{
  int w ( this->Width );
  int h ( this->Height );

  // Set the viewport.
  _renderer->viewport ( 0, 0, (int) w, (int) h );

  // Set the viewer's projection matrix.
  double fovy  ( OsgTools::Render::Defaults::CAMERA_FOV_Y );
  double zNear ( OsgTools::Render::Defaults::CAMERA_Z_NEAR );
  double zFar  ( OsgTools::Render::Defaults::CAMERA_Z_FAR );
  double width ( w ), height ( h );
  double aspect ( width / height );
  
  _renderer->viewer()->setProjectionMatrixAsPerspective ( fovy, aspect, zNear, zFar );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the background color.
//
///////////////////////////////////////////////////////////////////////////////

System::Drawing::Color^ Panel::backgroundColor()
{
  osg::Vec4 c ( _renderer->backgroundColor() );

  return System::Drawing::Color::FromArgb( c.a(), c.r(), c.g(), c.b() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the background color.
//
///////////////////////////////////////////////////////////////////////////////

void Panel::backgroundColor ( System::Drawing::Color color )
{
  this->ClearColor = color;

  osg::Vec4 c ( color.R, color.G, color.B, color.A );

  _renderer->backgroundColor( c );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene.
//
///////////////////////////////////////////////////////////////////////////////

CadKit::OSG::Glue::Node^ Panel::scene()
{
  return gcnew CadKit::OSG::Glue::Node( System::IntPtr ( _renderer->scene() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Panel::scene( CadKit::OSG::Glue::Node^ node )
{
  osg::Node* n ( static_cast < osg::Node* > ( node->nodePtr().ToPointer() ) );
  _renderer->scene( n );
  _matrixManipulator->setNode( n );

  // Make the adapters.
  OsgTools::Render::EventAdapter::Ptr ea ( new OsgTools::Render::EventAdapter );
  ea->setWindowSize ( Usul::Math::Vec2ui ( this->Width, this->Height ) );
  OsgTools::Render::ActionAdapter aa ( 0x0 );

  _matrixManipulator->home( *ea, aa );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Mouse has moved.
//
///////////////////////////////////////////////////////////////////////////////

void Panel::OnMouseMove(System::Windows::Forms::MouseEventArgs ^e)
{
  try
  {
    //lock (this->Mutex)
    {
      bool left = (e->Button == System::Windows::Forms::MouseButtons::Left);
      bool middle = (e->Button == System::Windows::Forms::MouseButtons::Middle);
      bool right = (e->Button == System::Windows::Forms::MouseButtons::Right);

      float x = e->Location.X;
      float y = this->Size.Height - e->Location.Y;

      bool mouse = left || middle || right;
      
      if (false == mouse)
        return;

      osgGA::GUIEventAdapter::EventType type = mouse ? osgGA::GUIEventAdapter::DRAG : osgGA::GUIEventAdapter::MOVE;

      // Declare the adapters.
      OsgTools::Render::EventAdapter::Ptr ea ( new OsgTools::Render::EventAdapter );
      OsgTools::Render::ActionAdapter aa ( 0x0 );

      // Set the event-adapter. Order is important.
      ea->setSeconds ( _renderer->framestamp()->getReferenceTime() );
      ea->setWindowSize ( Usul::Math::Vec2ui( this->Width, this->Height ) );
      ea->setMouse ( Usul::Math::Vec2f ( x, y ) );
      ea->setButtonMask ( left, middle, right );
      ea->setEventType ( type );

      // Ask the manipulator to handle the event.
      if ( _matrixManipulator->handle ( *ea, aa ) )
      {
        // Set the scene's matrix.
        _renderer->viewer()->setViewMatrix ( _matrixManipulator->getInverseMatrix() );
        this->render();
      }
    }
  }
  catch (System::Exception ^ex)
  {
    System::Console::WriteLine("Error 3325853270: {0}", ex->Message);
  }
}


void Panel::OnMouseWheel(System::Windows::Forms::MouseEventArgs ^e)
{
  /*try
  {
    lock (this.Mutex)
    {
      base.OnMouseWheel(e);
    }
  }
  catch (System.Exception ex)
  {
    System.Console.WriteLine("Error 2951730899: {0}", ex.Message);
  }*/
}


void Panel::OnMouseUp(System::Windows::Forms::MouseEventArgs ^e)
{
  /*try
  {
    lock (this.Mutex)
    {
      bool left = (e.Button == System.Windows.Forms.MouseButtons.Left);
      bool middle = (e.Button == System.Windows.Forms.MouseButtons.Middle);
      bool right = (e.Button == System.Windows.Forms.MouseButtons.Right);

      float x = e.Location.X;
      float y = this.Size.Height - e.Location.Y;

      if (null != this.Viewer)
      {
        this.Viewer.buttonRelease(x, y, left, middle, right);
      }
    }
  }
  catch (System.Exception ex)
  {
    System.Console.WriteLine("Error 2213011651: {0}", ex.Message);
  }*/
}


void Panel::OnMouseDown(System::Windows::Forms::MouseEventArgs ^e)
{
  /*try
  {
    lock (this.Mutex)
    {
      bool left = (e.Button == System.Windows.Forms.MouseButtons.Left);
      bool middle = (e.Button == System.Windows.Forms.MouseButtons.Middle);
      bool right = (e.Button == System.Windows.Forms.MouseButtons.Right);

      float x = e.Location.X;
      float y = this.Size.Height - e.Location.Y;

      if (null != this.Viewer)
      {
        this.Viewer.buttonPress(x, y, left, middle, right);
        this.Viewer.handleSeek(x, y, left);
      }
    }
  }
  catch (System.Exception ex)
  {
    System.Console.WriteLine("Error 3544395587: {0}", ex.Message);
  }*/
}
