
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/WmsLayerQt/WmsLayerQtComponent.h"
#include "Minerva/Plugins/WmsLayerQt/EditWmsLayerWidget.h"
#include "Minerva/Qt/Widgets/AddNetworkLayerWidget.h"

#include "Minerva/Interfaces/IDirtyScene.h"
#include "Minerva/Interfaces/IFeature.h"

#include "Usul/Components/Factory.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Registry/Database.h"
#include "Usul/Registry/Qt.h"

#include "QtGui/QDialog"
#include "QtGui/QDialogButtonBox"
#include "QtGui/QVBoxLayout"

USUL_DECLARE_COMPONENT_FACTORY ( WmsLayerQtComponent );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( WmsLayerQtComponent, WmsLayerQtComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Registry constants.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  const std::string SECTION  ( "wms_qt_gui" );
  const std::string GEOMETRY ( "geometry" );
}


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

WmsLayerQtComponent::WmsLayerQtComponent() : BaseClass(),
  _widget ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

WmsLayerQtComponent::~WmsLayerQtComponent() 
{
//  delete _widget;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *WmsLayerQtComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  case Usul::Interfaces::ILayerAddGUIQt::IID:
    return static_cast < Usul::Interfaces::ILayerAddGUIQt * > ( this );
  case Usul::Interfaces::ILayerModifyGUIQt::IID:
    return static_cast < Usul::Interfaces::ILayerModifyGUIQt * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Usul::Interfaces::IPlugin implementation
//
///////////////////////////////////////////////////////////////////////////////

std::string WmsLayerQtComponent::getPluginName() const 
{
  return "WMS Layer Qt GUI";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Widget to add a gui for wms servers.
//
///////////////////////////////////////////////////////////////////////////////

QWidget* WmsLayerQtComponent::layerAddGUI ( Usul::Interfaces::IUnknown *caller )
{
  // TODO: Need to clean up if cancelled.
  _widget = new AddWmsLayerWidget;
  return _widget;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name of widget.  This will go on a tab.
//
///////////////////////////////////////////////////////////////////////////////

std::string WmsLayerQtComponent::name() const 
{
  return "WMS";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Apply any selections the user made.
//
///////////////////////////////////////////////////////////////////////////////

void WmsLayerQtComponent::apply ( Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown* caller )
{
  if ( 0x0 != _widget )
    _widget->apply ( parent, caller );

  // TODO: What about cancelled dialog?
  _widget = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Do we handle this layer.
//
///////////////////////////////////////////////////////////////////////////////

bool WmsLayerQtComponent::handle ( Usul::Interfaces::IUnknown* layer ) const
{
  Minerva::Interfaces::IFeature::QueryPtr f ( layer );
  Minerva::Core::Data::Feature::RefPtr feature ( f.valid() ? f->feature() : 0x0 );
  return 0x0 != dynamic_cast<Minerva::Core::Layers::RasterLayerWms*> ( feature.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show a dialog to modify properties.
//
///////////////////////////////////////////////////////////////////////////////

void WmsLayerQtComponent::showModifyGUI ( Usul::Interfaces::IUnknown* layer, Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown* caller )
{
	typedef Minerva::Core::Layers::RasterLayerWms RasterLayerWms;
	typedef RasterLayerWms::Options Options;
	typedef RasterLayerWms::Alphas Alphas;

  Minerva::Interfaces::IFeature::QueryPtr f ( layer );
  Minerva::Core::Data::Feature::RefPtr feature ( f.valid() ? f->feature() : 0x0 );
  RasterLayerWms::RefPtr wms ( dynamic_cast <RasterLayerWms*> ( feature.get() ) );
  
  // Return now if no layer.
  if ( 0x0 == wms )
    return;
  
  // Save current state.
	Alphas alphas ( wms->alphas() );
	Options options ( wms->options() );
  std::string name ( wms->name() );
  std::string url ( wms->urlBase() );
  
  EditWmsLayerWidget *page ( new EditWmsLayerWidget ( wms ) );
  
  QDialog dialog ( 0x0 );
  
  const QDialogButtonBox::StandardButtons buttons ( QDialogButtonBox::Cancel|QDialogButtonBox::NoButton|QDialogButtonBox::Ok );
  QDialogButtonBox *buttonBox ( new QDialogButtonBox ( buttons, Qt::Horizontal, &dialog ) );
  
  QObject::connect ( buttonBox, SIGNAL ( accepted() ), &dialog, SLOT ( accept() ) );
  QObject::connect ( buttonBox, SIGNAL ( rejected() ), &dialog, SLOT ( reject() ) );
  
  QVBoxLayout *topLayout ( new QVBoxLayout );
  dialog.setLayout ( topLayout );
  
  topLayout->addWidget ( page );
  topLayout->addWidget ( buttonBox );
  
  // Set the title.
  dialog.setWindowTitle( QString ( "Edit " ) + QString ( name.c_str() ) );
  
  // Set the window's properties.
  dialog.restoreGeometry ( Usul::Registry::Database::instance()[Detail::SECTION][Detail::GEOMETRY].get<QByteArray> ( QByteArray() ) );
  
	const int result ( dialog.exec() );

	if ( QDialog::Accepted == result )
	{
		// Query for needed interfaces.
		Minerva::Interfaces::IDirtyScene::QueryPtr dirty ( caller );
		Usul::Interfaces::IDocument::QueryPtr document ( caller );

		if ( dirty.valid() )
			dirty->dirtyScene ( true, Usul::Interfaces::IUnknown::QueryPtr ( wms ) );
  
		if ( document.valid() )
			document->requestRedraw();
	}
  else if ( QDialog::Rejected == result )
  {
    // Restore the state.
		wms->alphas ( alphas );
		wms->options ( options );
    wms->name ( name );
    wms->urlBase ( url );
  }  
  
  // Save the window's properties.
  Usul::Registry::Database::instance()[Detail::SECTION][Detail::GEOMETRY] = dialog.saveGeometry();
}
