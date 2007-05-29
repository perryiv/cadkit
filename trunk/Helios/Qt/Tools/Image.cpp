
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

#include "Helios/Qt/Tools/Image.h"

#include "Usul/App/Application.h"
#include "Usul/File/Path.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Trace/Trace.h"

#include "QtGui/QAction"
#include "QtGui/QIcon"
#include "QtGui/QWidget"
#include "QtGui/QLabel"

using namespace CadKit::Helios::Tools;


///////////////////////////////////////////////////////////////////////////////
//
//  Helper struct to set the icon.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class T > struct SetIcon;

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

    QIcon icon ( path.c_str() );
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

void Image::pixmap ( const std::string &name, QLabel *label )
{
  USUL_TRACE_SCOPE_STATIC;

  if ( 0x0 == label )
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

  QPixmap pixmap;
  if ( false == pixmap.load ( path.c_str() ) )
  {
    return;
  }

  // If we have an image then set it.
  if ( ( false == pixmap.isNull() ) && ( 0x0 != label ) )
  {
    label->setPixmap ( pixmap );
  }
}
