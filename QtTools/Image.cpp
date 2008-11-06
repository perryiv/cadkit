
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper class for creating images.
//
///////////////////////////////////////////////////////////////////////////////

#include "QtTools/Image.h"

#include "Usul/App/Application.h"
#include "Usul/File/Path.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Trace/Trace.h"

#include "QtGui/QAbstractButton"
#include "QtGui/QAction"
#include "QtGui/QIcon"
#include "QtGui/QImageReader"
#include "QtGui/QLabel"
#include "QtGui/QSplashScreen"
#include "QtGui/QWidget"

using namespace QtTools;


///////////////////////////////////////////////////////////////////////////////
//
//  Helper struct to set the icon.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class T > struct SetIcon;

  template <> struct SetIcon < QAbstractButton >
  {
    static void set ( QAbstractButton *b, QIcon icon )
    {
      USUL_TRACE_SCOPE_STATIC;
      if ( 0x0 != b )
      {
        b->setIcon ( icon );
      }
    }
  };

  template <> struct SetIcon < QWidget >
  {
    static void set ( QWidget *w, QIcon icon )
    {
      USUL_TRACE_SCOPE_STATIC;
      if ( 0x0 != w )
      {
        w->setWindowIcon ( icon );
      }
    }
  };

  template <> struct SetIcon < QAction >
  {
    static void set ( QAction *a, QIcon icon )
    {
      USUL_TRACE_SCOPE_STATIC;
      if ( 0x0 != a )
      {
        a->setIcon ( icon );
      }
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to set the icon.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class T > void set ( const std::string &name, T *t )
  {
    USUL_TRACE_SCOPE_STATIC;

    if ( 0x0 == t )
    {
      return;
    }

    std::string path ( name );
    if ( false == Usul::Predicates::FileExists::test ( path ) )
    {
      const std::string file ( Usul::File::base ( name ) + "." + Usul::File::extension ( name ) );
      const std::string dir ( Usul::App::Application::instance().iconDirectory() );
      path = dir + file;
      if ( false == Usul::Predicates::FileExists::test ( path ) )
      {
        return;
      }
    }
    
    // Make the reader.
    QImageReader reader ( path.c_str() );

    // Read the image.
    QImage image ( reader.read() );
    
    // Check to see if the image was read correctly.
    if ( image.isNull() )
    {
      std::cout << "Warning 3786892950: Could not read file " << path << std::endl;
      std::cout << "Reason: " << reader.errorString().toStdString() << std::endl;
      return;
    }

    // Make the icon.
    QIcon icon ( QPixmap::fromImage ( image ) );
    if ( icon.isNull() )
    {
      return;
    }

    Detail::SetIcon<T>::set ( t, icon );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the icon.
//
///////////////////////////////////////////////////////////////////////////////

void Image::icon ( const std::string &name, QAbstractButton *button )
{
  USUL_TRACE_SCOPE_STATIC;
  Detail::set ( name, button );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the icon.
//
///////////////////////////////////////////////////////////////////////////////

void Image::icon ( const std::string &name, QWidget *widget )
{
  USUL_TRACE_SCOPE_STATIC;
  Detail::set ( name, widget );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the icon.
//
///////////////////////////////////////////////////////////////////////////////

void Image::icon ( const std::string &name, QAction *action )
{
  USUL_TRACE_SCOPE_STATIC;
  Detail::set ( name, action );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the pixmap.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class T > void pixmap ( const std::string &name, T *t )
  {
    USUL_TRACE_SCOPE_STATIC;

    if ( 0x0 == t )
    {
      return;
    }

    std::string path ( name );
    if ( false == Usul::Predicates::FileExists::test ( path ) )
    {
      const std::string file ( Usul::File::base ( name ) + "." + Usul::File::extension ( name ) );
      const std::string dir ( Usul::App::Application::instance().iconDirectory() );
      path = dir + file;
      if ( false == Usul::Predicates::FileExists::test ( path ) )
      {
        return;
      }
    }

    QPixmap p;
    if ( false == p.load ( path.c_str() ) )
    {
      return;
    }

    // If we have an image then set it.
    if ( ( false == p.isNull() ) && ( 0x0 != t ) )
    {
      t->setPixmap ( p );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the pixmap.
//
///////////////////////////////////////////////////////////////////////////////

void Image::pixmap ( const std::string &name, QLabel *label )
{
  USUL_TRACE_SCOPE_STATIC;
  Detail::pixmap ( name, label );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the pixmap.
//
///////////////////////////////////////////////////////////////////////////////

void Image::pixmap ( const std::string &name, QSplashScreen *splash )
{
  USUL_TRACE_SCOPE_STATIC;
  Detail::pixmap ( name, splash );
}
