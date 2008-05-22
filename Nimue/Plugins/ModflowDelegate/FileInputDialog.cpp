
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Input dialog for files.
//
///////////////////////////////////////////////////////////////////////////////

#include "ModflowDelegate/FileInputDialog.h"
#include "ModflowDelegate/FileInputForm.h"
#include "ModflowDelegate/DiscretizationInputForm.h"
#include "ModflowDelegate/BasicPackageForm.h"
#include "ModflowDelegate/HeadLevelOutputForm.h"
#include "ModflowDelegate/RechargeForm.h"
#include "ModflowDelegate/PumpingForm.h"
#include "ModflowDelegate/AuxiliaryDataForm.h"
#include "ModflowDelegate/ProjectionDialog.h"

#include "ModflowModel/Constants.h"

#include "QtTools/ComboBox.h"

#include "Usul/Registry/Database.h"
#include "Usul/Registry/Qt.h"

#include "XmlTree/ReplaceIllegalCharacters.h"

#include "QtGui/QVBoxLayout"
#include "QtGui/QGridLayout"
#include "QtGui/QScrollArea"
#include "QtGui/QLabel"
#include "QtGui/QPushButton"

#include "ogr_spatialref.h"
#include "cpl_error.h"

#include <sstream>

using namespace Modflow;


///////////////////////////////////////////////////////////////////////////////
//
//  Registry constants.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  const std::string SECTION  ( "modflow_file_input_dialog" );
  const std::string GEOMETRY ( "geometry" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FileInputDialog::FileInputDialog ( QWidget *parent ) : BaseClass ( parent ),
  _forms(),
  _layout ( new QVBoxLayout ),
  _widget ( new QWidget ),
  _coordinateSystem ( false, "" )
{
  // Initialize code from Designer.
  this->setupUi ( this );

  // Strings for the combo box.
  typedef std::vector<std::string> Strings;
  Strings strings;
  strings.push_back ( Modflow::FileTypes::DISCRETIZATION );
  strings.push_back ( Modflow::FileTypes::BASIC_PACKAGE );
  //strings.push_back ( Modflow::FileTypes::BLOCK_CENTERED_FLOW );
  strings.push_back ( Modflow::FileTypes::HEAD_LEVEL_OUTPUT );
  strings.push_back ( Modflow::FileTypes::RECHARGE );
  strings.push_back ( Modflow::FileTypes::PUMPING );
  strings.push_back ( Modflow::FileTypes::AUXILIARY_DATA );
  
  // Populate the combo box.
  QtTools::ComboBox::populate ( *_comboBox, strings );
  _comboBox->setCurrentIndex ( 0 );
  
  QScrollArea *scrollArea ( new QScrollArea );
  _scrollWidget->setLayout ( new QVBoxLayout );
  _scrollWidget->layout()->addWidget ( scrollArea );
  scrollArea->setWidget ( _widget );
  scrollArea->setWidgetResizable ( true );
  
  _layout->addStretch();
  
  // Set the layout.
  _widget->setLayout ( _layout );
  
  // Set the window's properties.
  this->restoreGeometry ( Usul::Registry::Database::instance()[Detail::SECTION][Detail::GEOMETRY].get<QByteArray> ( QByteArray() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

FileInputDialog::~FileInputDialog()
{
  // Set the window's properties.
  Usul::Registry::Database::instance()[Detail::SECTION][Detail::GEOMETRY] = this->saveGeometry();
  
  for ( Forms::iterator iter = _forms.begin(); iter != _forms.end(); ++iter )
    delete *iter;
  _forms.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add button was clicked.
//
///////////////////////////////////////////////////////////////////////////////

void FileInputDialog::on_addButton_clicked()
{
  // The form.
  Modflow::FileInputForm *form ( 0x0 );
  
  // Get the name of the form to use.
  std::string name ( _comboBox->currentText().toStdString() );
  
  // Make the form
  if ( Modflow::FileTypes::DISCRETIZATION == name )
    form = new Modflow::DiscretizationInputForm();
  else if ( Modflow::FileTypes::BASIC_PACKAGE == name )
    form = new Modflow::BasicPackageForm();
  else if ( Modflow::FileTypes::HEAD_LEVEL_OUTPUT == name )
    form = new Modflow::HeadLevelOutputForm();
  else if ( Modflow::FileTypes::RECHARGE == name )
    form = new Modflow::RechargeForm();
  else if ( Modflow::FileTypes::PUMPING == name )
    form = new Modflow::PumpingForm();
  else if ( Modflow::FileTypes::AUXILIARY_DATA == name )
    form = new Modflow::AuxiliaryDataForm();

  // Add the form to the widget if we have a valid one.
  if ( 0x0 != form )
  {
    // Remove from list if form can only be added once.
    if ( form->addOnce() )
      _comboBox->removeItem ( _comboBox->findText ( name.c_str() ) );

    // Add the widget.
    if ( 0x0 != _layout )
    {
      QFrame *frame ( new QFrame );
      frame->setFrameStyle ( QFrame::Sunken | QFrame::StyledPanel );

      // This doesn't appear to have any effect...
#if 0
      QPalette::ColorRole role ( ( _forms.size() % 2 == 0 ) ? QPalette::Base : QPalette::AlternateBase );
      frame->setBackgroundRole ( role );
      frame->setForegroundRole ( role );
#endif
      
      // Make a button for remove.
      QPushButton *remove ( new QPushButton ( "Remove" ) );
      
      // Make a label.
      QLabel *label ( new QLabel ( name.c_str() ) );
      label->setAlignment ( Qt::AlignHCenter | Qt::AlignVCenter );

      QGridLayout *grid ( new QGridLayout );
      grid->addWidget ( label, 0, 0, 1, 2 );
      grid->addWidget ( form, 1, 0 );
      grid->addWidget ( remove, 1, 1 );

      // Give all columns for name the same minimum width.
      //grid->setColumnMinimumWidth ( 0, 120 );

      frame->setLayout ( grid );
      
      // Hide form with remove button is pressed.
      QObject::connect ( remove, SIGNAL ( pressed() ), frame, SLOT ( hide() ) );
      QObject::connect ( remove, SIGNAL ( pressed() ), form,  SLOT ( hide() ) );

      _layout->insertWidget( 0, frame );
    }
    
    // Add to our list.
    _forms.push_back ( form );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the xml string.
//
///////////////////////////////////////////////////////////////////////////////

std::string FileInputDialog::buildXml() const
{
  std::ostringstream os;
  
  // Write header.
  os << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n" << std::endl;

  os << "<modflow>" << std::endl;

  // Get the wkt for coordinate system.
  std::string wkt ( true == _coordinateSystem.first ? _coordinateSystem.second : "" );
  XmlTree::replaceIllegalCharacters ( wkt );

  // Write coordinate system.
  os << "<coordinate_system well_known_text=\"" << wkt << "\" />" << std::endl;

  // Write files.
  os << "<files>" << std::endl;
  for ( Forms::const_iterator iter = _forms.begin(); iter != _forms.end(); ++iter )
  {
    FileInputForm *form ( *iter );
    
    // Don't write out forms that aren't visible (removed).
    if ( 0x0 != form && false == form->isHidden() )
      os << form->buildXml();
  }
  
  os << "</files>" << std::endl;

  // Write footer.
  os << "</modflow>" << std::endl;

  return os.str();
}


///////////////////////////////////////////////////////////////////////////////
//
//  The choose coordinate system clicked.
//
///////////////////////////////////////////////////////////////////////////////

void FileInputDialog::on_coordinateSystem_clicked()
{
  ProjectionDialog dialog ( this );

  if ( QDialog::Accepted == dialog.exec() )
  {
    OGRSpatialReference *srs ( dialog.coordinateSystem() );
    char *wkt ( 0x0 );
    if ( 0x0 != srs && CE_None == srs->exportToWkt ( &wkt ) )
    {
      _coordinateSystem.first = true;
      _coordinateSystem.second = std::string ( wkt );
      _coordinateSystemName->setText ( srs->GetAttrValue( "PROJCS" ) );
      ::OGRFree ( wkt );
    }
  }
}
