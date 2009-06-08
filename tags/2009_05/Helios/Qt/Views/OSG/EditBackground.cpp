
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Views/OSG/EditBackground.h"

#include "Usul/Bits/Bits.h"

#include "QtTools/Color.h"
#include "QtTools/ColorButton.h"

#include "QtGui/QHBoxLayout"
#include "QtGui/QVBoxLayout"
#include "QtGui/QGridLayout"
#include "QtGui/QCheckBox"
#include "QtGui/QGroupBox"
#include "QtGui/QLabel"

using namespace CadKit::Helios::Views::OSG;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

EditBackground::EditBackground ( OsgTools::Render::Viewer* viewer, QWidget* parent ) :
  BaseClass ( parent ),
  _viewer      ( viewer ),
  _topLeft     ( new QCheckBox ( "Top Left"     ) ),
  _topRight    ( new QCheckBox ( "Top Right"    ) ),
  _bottomLeft  ( new QCheckBox ( "Bottom Left"  ) ),
  _bottomRight ( new QCheckBox ( "Bottom Right" ) )
{
  this->setWindowTitle ( "Edit Background Color" );
  QPushButton *ok ( new QPushButton ( "Ok" ) );
  QPushButton *cancel ( new QPushButton ( "Cancel" ) );

  connect ( ok,     SIGNAL ( clicked () ), this, SLOT ( accept () ) );
  connect ( cancel, SIGNAL ( clicked () ), this, SLOT ( reject () ) );

  QVBoxLayout* topLayout ( new QVBoxLayout );
  this->setLayout ( topLayout );

  // Add the corner checks.
  topLayout->addWidget ( this->_initChecks() );

  // Color button.
  {
    QtTools::ColorButton* color ( new QtTools::ColorButton );
    color->setEnabled ( _viewer.valid() );

    // Set the current color.
    if ( _viewer.valid() )
      color->color ( QtTools::Color < osg::Vec4f >::convert ( viewer->backgroundColor() ) );

    // Connec the slot.
    connect ( color, SIGNAL ( colorChangedF ( float, float, float, float ) ), this, SLOT ( _colorChanged ( float, float, float, float ) ) );

    QHBoxLayout *layout ( new QHBoxLayout );
    layout->addWidget ( new QLabel ( "Edit Color:" ) );
    layout->addWidget ( color );
    topLayout->addLayout ( layout );
  }

  // Ok and cancel Button.
  {
    QHBoxLayout *layout ( new QHBoxLayout );
    layout->addStretch();
    layout->addWidget ( ok );
    layout->addWidget ( cancel );
    topLayout->addLayout ( layout );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

EditBackground::~EditBackground()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the check boxes.
//
///////////////////////////////////////////////////////////////////////////////

QWidget* EditBackground::_initChecks()
{
  QGroupBox *group ( new QGroupBox ( "Corners" ) );
  
  QGridLayout *layout ( new QGridLayout );
  
  layout->addWidget ( _topLeft, 0, 0, Qt::AlignLeft );
  layout->addWidget ( _topRight, 0, 1, Qt::AlignLeft );
  layout->addWidget ( _bottomLeft, 1, 0, Qt::AlignLeft );
  layout->addWidget ( _bottomRight, 1, 1, Qt::AlignLeft );

  group->setLayout ( layout );
  
  bool valid ( _viewer.valid() );

  // Set the enabled state.
  _topLeft->setEnabled ( valid );
  _topRight->setEnabled ( valid );
  _bottomLeft->setEnabled ( valid );
  _bottomRight->setEnabled ( valid );

  // Set current check state. The static_cast is because of a g++ 4.1.2 bug...
  if ( _viewer.valid() )
  {
    _topLeft->setChecked     ( Usul::Bits::has ( _viewer->backgroundCorners(), static_cast<unsigned int> ( Corners::TOP_LEFT )     ) );
    _topRight->setChecked    ( Usul::Bits::has ( _viewer->backgroundCorners(), static_cast<unsigned int> ( Corners::TOP_RIGHT )    ) );
    _bottomLeft->setChecked  ( Usul::Bits::has ( _viewer->backgroundCorners(), static_cast<unsigned int> ( Corners::BOTTOM_LEFT )  ) );
    _bottomRight->setChecked ( Usul::Bits::has ( _viewer->backgroundCorners(), static_cast<unsigned int> ( Corners::BOTTOM_RIGHT ) ) );
  }

  // Connect slots.
  connect ( _topLeft,     SIGNAL ( stateChanged ( int ) ), this, SLOT ( _topLeftChanged     ( int ) ) );
  connect ( _topRight,    SIGNAL ( stateChanged ( int ) ), this, SLOT ( _topRightChanged    ( int ) ) );
  connect ( _bottomLeft,  SIGNAL ( stateChanged ( int ) ), this, SLOT ( _bottomLeftChanged  ( int ) ) );
  connect ( _bottomRight, SIGNAL ( stateChanged ( int ) ), this, SLOT ( _bottomRightChanged ( int ) ) );

  return group;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Top left check state changed.
//
///////////////////////////////////////////////////////////////////////////////

void EditBackground::_topLeftChanged ( int state )
{
  // The static_cast is because of a g++ 4.1.2 bug...
  if ( _viewer.valid() )
    _viewer->backgroundCorners ( Usul::Bits::set ( _viewer->backgroundCorners(), static_cast<unsigned int> ( Corners::TOP_LEFT ), ( state > 0 ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Top right check state changed.
//
///////////////////////////////////////////////////////////////////////////////

void EditBackground::_topRightChanged ( int state )
{
  // The static_cast is because of a g++ 4.1.2 bug...
  if ( _viewer.valid() )
    _viewer->backgroundCorners ( Usul::Bits::set ( _viewer->backgroundCorners(), static_cast<unsigned int> ( Corners::TOP_RIGHT ), ( state > 0 ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Bottom left check state changed.
//
///////////////////////////////////////////////////////////////////////////////

void EditBackground::_bottomLeftChanged ( int state )
{
  // The static_cast is because of a g++ 4.1.2 bug...
  if ( _viewer.valid() )
    _viewer->backgroundCorners ( Usul::Bits::set ( _viewer->backgroundCorners(), static_cast<unsigned int> ( Corners::BOTTOM_LEFT ), ( state > 0 ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Bottom right check state changed.
//
///////////////////////////////////////////////////////////////////////////////

void EditBackground::_bottomRightChanged ( int state )
{
  // The static_cast is because of a g++ 4.1.2 bug...
  if ( _viewer.valid() )
    _viewer->backgroundCorners ( Usul::Bits::set ( _viewer->backgroundCorners(), static_cast<unsigned int> ( Corners::BOTTOM_RIGHT ), ( state > 0 ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The color has changed.
//
///////////////////////////////////////////////////////////////////////////////

void EditBackground::_colorChanged ( float r, float g, float b, float a )
{
  if ( _viewer.valid() )
  {
    _viewer->backgroundColor ( osg::Vec4 ( r, g, b, a ) );
    _viewer->render();
  }
}
