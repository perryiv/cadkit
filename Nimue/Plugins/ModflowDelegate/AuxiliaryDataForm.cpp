
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "ModflowDelegate/AuxiliaryDataForm.h"
#include "ModflowDelegate/ProjectionDialog.h"

#include "ModflowModel/Constants.h"

#include "Usul/Strings/Format.h"

#include "XmlTree/ReplaceIllegalCharacters.h"

#include "ogr_spatialref.h"
#include "cpl_error.h"

#include <limits>
#include <sstream>

using namespace Modflow;

///////////////////////////////////////////////////////////////////////////////
//
//  Constants for the registry.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  const std::string SECTION ( "auxiliary_data_form" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AuxiliaryDataForm::AuxiliaryDataForm ( QWidget *parent  ) : 
  BaseClass ( parent ),
  _coordinateSystem( 0x0 )
{
  // Initialize code from Designer.
  this->setupUi ( this );
  
  _translateX->setRange ( std::numeric_limits<double>::max() * -1.0, std::numeric_limits<double>::max() );
  _translateY->setRange ( std::numeric_limits<double>::max() * -1.0, std::numeric_limits<double>::max() );
  _translateZ->setRange ( std::numeric_limits<double>::max() * -1.0, std::numeric_limits<double>::max() );
  
  QObject::connect ( _overrideColor, SIGNAL ( toggled ( bool ) ), _colorLabel,          SLOT ( setEnabled ( bool ) ) );
  QObject::connect ( _overrideColor, SIGNAL ( toggled ( bool ) ), _colorButton,         SLOT ( setEnabled ( bool ) ) );
  QObject::connect ( _overrideColor, SIGNAL ( toggled ( bool ) ), _transparencyLabel,   SLOT ( setEnabled ( bool ) ) );
  QObject::connect ( _overrideColor, SIGNAL ( toggled ( bool ) ), _transparencySpinBox, SLOT ( setEnabled ( bool ) ) );
  
  const bool enable ( _overrideColor->isChecked() );
  _colorLabel->setEnabled( enable );
  _colorButton->setEnabled( enable );
  _transparencyLabel->setEnabled( enable );
  _transparencySpinBox->setEnabled( enable );

  QObject::connect ( _usePointSize, SIGNAL ( toggled ( bool ) ), _pointSize, SLOT ( setEnabled ( bool ) ) );
  QObject::connect ( _useLineWidth, SIGNAL ( toggled ( bool ) ), _lineWidth, SLOT ( setEnabled ( bool ) ) );
  _pointSize->setEnabled ( _usePointSize->isChecked() );
  _lineWidth->setEnabled ( _useLineWidth->isChecked() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

AuxiliaryDataForm::~AuxiliaryDataForm()
{
  _coordinateSystem.reset ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build xml string.
//
///////////////////////////////////////////////////////////////////////////////

std::string AuxiliaryDataForm::buildXml() const
{
  // Get the filename.
  std::string filename ( _filename->text().toStdString() );
  XmlTree::replaceIllegalCharacters ( filename );
  
  const std::string rotate    ( Usul::Strings::format ( _rotateAngle->value(), " ", _rotateX->value(), " ", _rotateY->value(), " ", _rotateZ->value() ) );
  const std::string translate ( Usul::Strings::format ( _translateX->value(), " ", _translateY->value(), " ", _translateZ->value() ) );

  std::ostringstream os;
  os << "<file type=\"" << Modflow::Readers::AUXILIARY_DATA << "\"" << std::endl;
  os << "      name=\"" << filename << "\"" << std::endl;
  os << "      " << Modflow::Attributes::LABEL << "=\"" << _labelEdit->text().toStdString() << "\"" << std::endl;
  os << "      " << Modflow::Attributes::ROTATE << "=\"" << rotate << "\"" << std::endl;
  os << "      " << Modflow::Attributes::TRANSLATE << "=\"" << translate << "\"" << std::endl;
  
  // Only write out color if we are suppose to.
  if ( _overrideColor->isChecked() )
  {
    QColor color ( _colorButton->color() );
    os << "      " << Modflow::Attributes::COLOR << "=\"" << color.red() << " " << color.green() << " " << color.blue() << " " << _transparencySpinBox->value() << "\"" << std::endl;
  }
  
  // Only write out texture if we have a filename.
  std::string texture ( _textureFilename->text().toStdString() );
  if ( false == texture.empty() )
    os << "      " << Modflow::Attributes::TEXTURE_IMAGES << "=\"" << texture << "\"" << std::endl;
  
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

  if ( _usePointSize->isChecked() )
  {
    os << "      " << Modflow::Attributes::POINT_SIZE << "=\"" << _pointSize->value() << "\"" << std::endl;
  }

  if ( _useLineWidth->isChecked() )
  {
    os << "      " << Modflow::Attributes::LINE_WIDTH << "=\"" << _lineWidth->value() << "\"" << std::endl;
  }
  
  os << " />" << std::endl;
  return os.str();
}


///////////////////////////////////////////////////////////////////////////////
//
//  The browse button has been clicked.
//
///////////////////////////////////////////////////////////////////////////////

void AuxiliaryDataForm::on_browseButton_clicked()
{
  std::string filters ( "All Files (*.*)" );
  this->_browse ( filters, Detail::SECTION, _filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Browse for a texture.
//
///////////////////////////////////////////////////////////////////////////////

void AuxiliaryDataForm::on_textureBrowse_clicked()
{
  std::string filters ( "JPEG Files (*.jpg);;PNG Files (*.jpg);;TIFF Files (*.jpg);;All Files (*.*)" );
  this->_browse ( filters, Detail::SECTION, _textureFilename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does this form only get added once?
//
///////////////////////////////////////////////////////////////////////////////

bool AuxiliaryDataForm::addOnce() const
{
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pick the coordinate system.
//
///////////////////////////////////////////////////////////////////////////////

void AuxiliaryDataForm::on_coordinateSystem_clicked()
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
