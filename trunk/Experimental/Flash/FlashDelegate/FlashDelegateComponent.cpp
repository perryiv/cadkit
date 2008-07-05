
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

#include "Flash/FlashDelegate/FlashDelegateComponent.h"
#include "Flash/FlashModel/IFlashDocument.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"

#include "Usul/Commands/GenericCommand.h"
#include "Usul/Components/Factory.h"
#include "Usul/Documents/Manager.h"

#include "QtGui/QDialog"
#include "QtGui/QDoubleSpinBox"
#include "QtGui/QHBoxLayout"
#include "QtGui/QVBoxLayout"
#include "QtGui/QLabel"
#include "QtGui/QPushButton"

USUL_DECLARE_COMPONENT_FACTORY ( FlashDelegateComponent );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( FlashDelegateComponent, FlashDelegateComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FlashDelegateComponent::FlashDelegateComponent() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

FlashDelegateComponent::~FlashDelegateComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *FlashDelegateComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin*>(this);
  case Usul::Interfaces::IMenuAdd::IID:
    return static_cast<Usul::Interfaces::IMenuAdd*> ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Does this delegate handle the given token
//
/////////////////////////////////////////////////////////////////////////////

bool FlashDelegateComponent::doesHandle ( const std::string& token ) const
{
  return ( token == "Flash Document"  );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the min/max of the flash document.
//
/////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  static void setMinMax()
  {
    // See if the active document is a flash document.
    Flash::IFlashDocument::QueryPtr fd ( Usul::Documents::Manager::instance().activeDocument() );
    if ( fd.valid() )
    {
      QDialog dialog;
      dialog.setWindowTitle ( "Enter Min/Max" );
      
      QVBoxLayout *topLayout ( new QVBoxLayout );
      dialog.setLayout ( topLayout );
      
      QDoubleSpinBox *min ( new QDoubleSpinBox );
      QDoubleSpinBox *max ( new QDoubleSpinBox );

      min->setRange( -std::numeric_limits<double>::max(), std::numeric_limits<double>::max() );
      max->setRange( -std::numeric_limits<double>::max(), std::numeric_limits<double>::max() );
      
      min->setValue ( fd->minimumGet() );
      max->setValue ( fd->maximumGet() );
      
      {
        QHBoxLayout *layout ( new QHBoxLayout );
        layout->addWidget ( new QLabel ( "Min:" ) );
        layout->addWidget ( min );
        layout->addWidget ( new QLabel ( "Max:" ) );
        layout->addWidget ( max );
        topLayout->addLayout ( layout );
      }
      
      {
        QPushButton *ok ( new QPushButton ( "Ok" ) );
        QPushButton *cancel ( new QPushButton ( "Cancel" ) );
        
        QObject::connect ( ok, SIGNAL ( clicked() ), &dialog, SLOT ( accept() ) );
        QObject::connect ( cancel, SIGNAL ( clicked() ), &dialog, SLOT ( reject() ) );
        
        QHBoxLayout *layout ( new QHBoxLayout );
        layout->addStretch();
        layout->addWidget( ok );
        layout->addWidget( cancel );
        topLayout->addLayout ( layout );
      }
      
      if ( QDialog::Accepted == dialog.exec() )
      {
        const double minValue ( min->value() );
        const double maxValue ( max->value() );
        
        fd->minimumSet ( minValue );
        fd->maximumSet ( maxValue );
      }
    }
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Add to the menu. (IMenuAdd).
//
/////////////////////////////////////////////////////////////////////////////

void FlashDelegateComponent::menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown* caller )
{
  // See if the active document is a flash document.
  Flash::IFlashDocument::QueryPtr fd ( Usul::Documents::Manager::instance().activeDocument() );
  if ( fd.valid() )
  {
    MenuKit::Menu::RefPtr view ( menu.find ( "&View", true ) );
    
    if ( view.valid() )
    {
      view->append ( new MenuKit::Button ( Usul::Commands::genericCommand ( "Set Min/Max", Helper::setMinMax, Usul::Commands::TrueFunctor() ) ) );
    }
  }
}
