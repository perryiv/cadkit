
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
//  Input form for files.
//
///////////////////////////////////////////////////////////////////////////////

#include "ModflowDelegate/FileInputForm.h"

#include "Usul/File/Path.h"
#include "Usul/Registry/Database.h"
#include "Usul/User/Directory.h"

#include "QtGui/QFileDialog"
#include "QtGui/QLineEdit"

using namespace Modflow;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FileInputForm::FileInputForm ( QWidget *parent  ) : 
  BaseClass ( parent )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

FileInputForm::~FileInputForm()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  The browse button has been clicked.
//
///////////////////////////////////////////////////////////////////////////////

void FileInputForm::_browse ( const std::string &filters, const std::string &regSection, QLineEdit *fileName )
{
  // Get the last directory.
  const std::string origDefaultDir ( Usul::User::Directory::documents ( true, false ) );
  const std::string lastDefaultDir ( Usul::Registry::Database::instance()["modflow"]["defaults"]["directory"].get ( origDefaultDir ) );
  const std::string directory ( Usul::Registry::Database::instance()["modflow"][regSection]["directory"].get ( lastDefaultDir ) );
  
  // Need to use this static function to get native file dialog.
  QString answer ( QFileDialog::getOpenFileName ( this, "Select File", directory.c_str(), filters.c_str(), 0x0 ) );
  
  if ( answer.size() > 0 )
  {
    std::string directory ( Usul::File::directory ( answer.toStdString(), false ) );
    Usul::Registry::Database::instance()["modflow"][regSection]["directory"] = directory;
    Usul::Registry::Database::instance()["modflow"]["defaults"]["directory"] = directory;
    if ( 0x0 != fileName )
    {
      fileName->setText ( answer );
    }
  }
}
