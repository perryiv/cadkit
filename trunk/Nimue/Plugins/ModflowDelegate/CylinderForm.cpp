
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "ModflowDelegate/CylinderForm.h"
#include "ModflowModel/Constants.h"

#include "XmlTree/ReplaceIllegalCharacters.h"

#include <limits>
#include <sstream>

using namespace Modflow;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

CylinderForm::CylinderForm ( const std::string& type, const std::string& section, const std::string& filters, QWidget *parent  ) : 
  BaseClass ( parent ),
  _type ( type ),
  _section ( section ),
  _filters ( filters )
{
  // Initialize code from Designer.
  this->setupUi ( this );

  // Default color.
  _colorButton->color ( QColor ( 226, 226, 0, 255 ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

CylinderForm::~CylinderForm()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build xml string.
//
///////////////////////////////////////////////////////////////////////////////

std::string CylinderForm::buildXml() const
{
  // Get the filename.
  std::string filename ( _filename->text().toStdString() );
  XmlTree::replaceIllegalCharacters ( filename );

  std::ostringstream os;
  os << "<file type=\"" << _type << "\"" << std::endl;
  os << "      name=\"" << filename << "\"" << std::endl;
  os << "      no_data=\"0\""  << std::endl;
  os << "      " << Modflow::Attributes::HEIGHT_SCALE << "=\"" << _heightScale->value() << "\"" << std::endl;
  os << "      " << Modflow::Attributes::RADIUS_SCALE << "=\"" << _radiusScale->value() << "\"" << std::endl;
  QColor color ( _colorButton->color() );
  os << "      " << Modflow::Attributes::COLOR << "=\"" << color.red() << " " << color.green() << " " << color.blue() <<  " 255\"" << std::endl;
  os << " />" << std::endl;
  return os.str();
}


///////////////////////////////////////////////////////////////////////////////
//
//  The browse button has been clicked.
//
///////////////////////////////////////////////////////////////////////////////

void CylinderForm::on_browseButton_clicked()
{
  this->_browse ( _filters, _section, _filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does this form only get added once?
//
///////////////////////////////////////////////////////////////////////////////

bool CylinderForm::addOnce() const
{
  return true;
}
