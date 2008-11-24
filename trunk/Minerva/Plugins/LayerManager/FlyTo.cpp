
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/LayerManager/FlyTo.h"
#include "Minerva/Core/Utilities/GeoCode.h"

#include "Minerva/Interfaces/ILookAtLayer.h"

#include "Usul/Documents/Manager.h"

#include "QtGui/QLineEdit"
#include "QtGui/QPushButton"
#include "QtGui/QVBoxLayout"

#include <iostream>

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FlyTo::FlyTo ( QWidget* parent ) : BaseClass ( parent ),
  _lineEdit ( new QLineEdit )
{
  QPushButton *go ( new QPushButton ( "Go" ) );
  
  connect ( go,        SIGNAL ( clicked() ),       this, SLOT ( _onFlyToClicked() ) );
  connect ( _lineEdit, SIGNAL ( returnPressed() ), this, SLOT ( _onFlyToClicked() ) );
  
  QHBoxLayout *topLayout ( new QHBoxLayout );
  this->setLayout ( topLayout );
  
  topLayout->addWidget ( _lineEdit );
  topLayout->addWidget ( go );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

FlyTo::~FlyTo()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Fly to location.
//
///////////////////////////////////////////////////////////////////////////////

void FlyTo::_onFlyToClicked()
{
  const std::string location ( _lineEdit->text().toStdString() );
  Minerva::Interfaces::ILookAtLayer::QueryPtr lookAt ( Usul::Documents::Manager::instance().activeDocument() );
  
  if ( false == location.empty() && true == lookAt.valid() )
  {
    typedef Minerva::Core::Utilities::GeoCode::Result GeoCodeResult;

    Minerva::Core::Utilities::GeoCode geoCode;
    
    GeoCodeResult result ( geoCode ( location ) );

    if ( result.success )
    {
      std::cout << "Geocoded " << location << " to " << result.location[1] << ", " << result.location[0] << std::endl;
      
      lookAt->lookAtPoint ( result.location );
    }
    else
    {
      std::cout << "Could not geocode " << location << std::endl;
    }
  }
}