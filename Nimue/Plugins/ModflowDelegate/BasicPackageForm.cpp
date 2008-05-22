
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "ModflowDelegate/BasicPackageForm.h"

#include "Usul/File/Path.h"
#include "Usul/Registry/Database.h"

#include "XmlTree/ReplaceIllegalCharacters.h"

#include "QtGui/QFileDialog"

#include <limits>

using namespace Modflow;

///////////////////////////////////////////////////////////////////////////////
//
//  Constants for the registry.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  const std::string SECTION ( "basic_package_form" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

BasicPackageForm::BasicPackageForm ( QWidget *parent  ) : 
  BaseClass ( parent )
{
  // Initialize code from Designer.
  this->setupUi ( this );

  _noDataValue->setRange ( std::numeric_limits<double>::max() * -1.0, std::numeric_limits<double>::max() );
  
  // Turn these off for now.
  _noDataValue->setVisible( false );
  _noDataValueLabel->setVisible ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

BasicPackageForm::~BasicPackageForm()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build xml string.
//
///////////////////////////////////////////////////////////////////////////////

std::string BasicPackageForm::buildXml() const
{
  // Get the filename.
  std::string filename ( _filename->text().toStdString() );
  XmlTree::replaceIllegalCharacters ( filename );

  std::ostringstream os;
  os << "<file type=\"basic_package\"" << std::endl;
  os << "      name=\"" << filename << "\"" << std::endl;
  os << "      no_data=\"" << _noDataValue->value() << "\"" << std::endl;
  os << " />" << std::endl;
  return os.str();
}


///////////////////////////////////////////////////////////////////////////////
//
//  The browse button has been clicked.
//
///////////////////////////////////////////////////////////////////////////////

void BasicPackageForm::on_browseButton_clicked()
{
  std::string filters ( "Basic Package File (*.ba6);;All Files (*.*)" );
  this->_browse ( filters, Detail::SECTION, _filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does this form only get added once?
//
///////////////////////////////////////////////////////////////////////////////

bool BasicPackageForm::addOnce() const
{
  return true;
}
