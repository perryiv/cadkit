
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "ModflowDelegate/DiscretizationInputForm.h"
#include "ModflowDelegate/ProjectionDialog.h"

#include "ModflowModel/Constants.h"

#include "Usul/Convert/Convert.h"

#include "XmlTree/ReplaceIllegalCharacters.h"

#include "QtTools/ComboBox.h"

#include "ogr_srs_api.h"
#include "ogr_spatialref.h"
#include "cpl_error.h"

using namespace Modflow;

///////////////////////////////////////////////////////////////////////////////
//
//  Constants for the registry.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  const std::string SECTION ( "discretization_input_form" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DiscretizationInputForm::DiscretizationInputForm ( QWidget *parent  ) : 
  BaseClass ( parent ),
  _conversions(),
  _coordinateSystem ( 0x0 )
{
  // Initialize code from Designer.
  this->setupUi ( this );

  _x->setRange ( std::numeric_limits<double>::max() * -1.0, std::numeric_limits<double>::max() );
  _y->setRange ( std::numeric_limits<double>::max() * -1.0, std::numeric_limits<double>::max() );
  _z->setRange ( std::numeric_limits<double>::max() * -1.0, std::numeric_limits<double>::max() );
  
  _conversions["None"] = 1.0;
  _conversions["Feet"] = Usul::Convert::Type<std::string,double>::convert ( SRS_UL_FOOT_CONV );
  _conversions["Feet (U.S. Survey)"] = Usul::Convert::Type<std::string,double>::convert ( SRS_UL_US_FOOT_CONV );

  _unitsComboBox->addItem ( "None" );
  _unitsComboBox->addItem ( "Feet" );
  _unitsComboBox->addItem ( "Feet (U.S. Survey)" );
  _unitsComboBox->setCurrentIndex ( 0 );

  QObject::connect ( _unitsComboBox, SIGNAL ( currentIndexChanged ( int ) ), this, SLOT ( _unitsComboBoxChanged ( int ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DiscretizationInputForm::~DiscretizationInputForm()
{
  _coordinateSystem.reset ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build xml string.
//
///////////////////////////////////////////////////////////////////////////////

std::string DiscretizationInputForm::buildXml() const
{
  // Get the filename.
  std::string filename ( _filename->text().toStdString() );
  XmlTree::replaceIllegalCharacters ( filename );

  std::ostringstream os;
  os << "<file type=\"" << Modflow::Readers::DISCRETIZATION << "\"" << std::endl;
  os << "      name=\"" << filename << "\"" << std::endl;
  os << "      " << Modflow::Attributes::CELL_GRID_ORIGIN << "=\"" << _x->value() << " " << _y->value() << " " << _z->value() << "\"" << std::endl;
  os << "      " << Modflow::Attributes::UNIT_CONVERSION << "=\"" << _unitConversion->value() << "\"" << std::endl;

  if ( 0x0 != _coordinateSystem.get() )
  {
    char *wkt ( 0x0 );
    if ( CE_None == _coordinateSystem->exportToWkt ( &wkt ) )
    {
      std::string text ( wkt );
      XmlTree::replaceIllegalCharacters ( text );
      os << "      " << Modflow::Attributes::WELL_KNOWN_TEXT << "=\"" << text << "\"" << std::endl;
      ::OGRFree ( wkt );
    }
  }
  
  os << " />" << std::endl;

  return os.str();
}


///////////////////////////////////////////////////////////////////////////////
//
//  The browse button has been clicked.
//
///////////////////////////////////////////////////////////////////////////////

void DiscretizationInputForm::on_browseButton_clicked()
{
  std::string filters ( "Discretization File (*.dis);;All Files (*.*)" );
  this->_browse ( filters, Detail::SECTION, _filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The choose coordinate system clicked.
//
///////////////////////////////////////////////////////////////////////////////

void DiscretizationInputForm::on_coordinateSystem_clicked()
{
  ProjectionDialog dialog ( this );

  if ( QDialog::Accepted == dialog.exec() )
  {
    OGRSpatialReference *srs ( dialog.coordinateSystem() );
    if ( 0x0 != srs )
    {
      _coordinateSystem.reset ( srs->Clone() );
      _coordinateSystemName->setText ( srs->GetAttrValue( "PROJCS" ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does this form only get added once?
//
///////////////////////////////////////////////////////////////////////////////

bool DiscretizationInputForm::addOnce() const
{
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Units combo box has changed.
//
///////////////////////////////////////////////////////////////////////////////

void DiscretizationInputForm::_unitsComboBoxChanged ( int )
{
  // Get the conversion.
  const std::string name ( _unitsComboBox->currentText().toStdString() );
  Conversions::const_iterator iter ( _conversions.find ( name ) );
  const double conversion ( iter != _conversions.end() ? iter->second : 1.0 );
  _unitConversion->setValue ( conversion );
}
