
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Input dialog for files.
//
///////////////////////////////////////////////////////////////////////////////

#include "AddModelDialog.h"

#include "QtTools/FileDialog.h"


#include "Usul/File/Path.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AddModelDialog::AddModelDialog ( QWidget *parent ) : BaseClass ( parent )
{
  // Initialize code from Designer.
  this->setupUi ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

AddModelDialog::~AddModelDialog()
{

}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void AddModelDialog::on_browseButton_clicked()
{
// Useful typedefs.
  typedef QtTools::FileDialog             FileDialog;
  typedef FileDialog::FilesResult         FilesResult;
  typedef FileDialog::Filter              Filter;
  typedef FileDialog::Filters             Filters;
  
  Filters filters;
  
  filters.push_back ( Filter ( "All Files (*.*)", "*.*" ) );
  
#if 0 // Single result case
  // Prompt the user.
  FileResult result ( FileDialog::getLoadFileName ( this, "Browse to model file", filters ) );
  _pathText->setText( result.first.c_str() );

  std::string nameText = Usul::File::base( result.first.c_str() );
  _nameText->setText( nameText.c_str() );
#else
    // Prompt the user.
  FilesResult results ( FileDialog::getLoadFileNames ( this, "Browse to model file", filters ) );

  for( unsigned int i = 0; i < results.first.size(); ++i )
  {

    std::string result = results.first.at( i ).c_str();
    std::string name = Usul::File::base( result );
    _modelsList->addItem( name, result );
  }
  //_pathText->setText( result.first.c_str() );

  //std::string nameText = Usul::File::base( result.first.c_str() );
  //_nameText->setText( nameText.c_str() );
#endif

}



///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

QtTools::StringsView::Items AddModelDialog::getItems() const
{

  //return _pathText->text().toStdString();
  return _modelsList->items();
}

