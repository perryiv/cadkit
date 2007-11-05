
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "SnapShotWidget.h"

#include "Usul/File/Temp.h"
#include "Usul/File/Remove.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Interfaces/GUI/ISaveFileDialog.h"
#include "Usul/Interfaces/ISnapShot.h"

#include "boost/algorithm/string/replace.hpp"

#include "QtCore/QProcess"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SnapShotWidget::SnapShotWidget( QWidget *parent, Usul::Interfaces::IUnknown* caller ) : BaseClass ( parent ),
  _files (),
  _caller ( caller )
{
  this->setupUi ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SnapShotWidget::~SnapShotWidget()
{
  this->_deleteFiles();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a snap shot.
//
///////////////////////////////////////////////////////////////////////////////

void SnapShotWidget::_deleteFiles ()
{
  for ( Files::iterator iter = _files.begin(); iter != _files.end(); ++iter )
    Usul::File::remove ( *iter );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a snap shot.
//
///////////////////////////////////////////////////////////////////////////////

void SnapShotWidget::on__snapShotButton_clicked()
{
  try
  {
    Usul::Interfaces::ISnapShot::QueryPtr snapShot ( Usul::Documents::Manager::instance().activeView() );

    if( snapShot.valid () )
    {
      // Get the parameters for the snap shot.
      double frameScale ( _frameScale->value() );
      unsigned int numSamples ( _numSamples->value() );

      typedef Usul::Interfaces::ISaveFileDialog::Filter  Filter;
      typedef Usul::Interfaces::ISaveFileDialog::Filters Filters;
      typedef Usul::Interfaces::ISaveFileDialog::FileResult FileResult;
      
      Filters filters;
      filters.push_back ( Filter ( "Bitmap (*.bmp)", "(*.bmp)" ) );
      filters.push_back ( Filter ( "JPEG (*.jpg)", "(*.jpg)" ) );
      filters.push_back ( Filter ( "PNG (*.png)", "(*.png)" ) );

      Usul::Interfaces::ISaveFileDialog::QueryPtr dialog ( _caller );

      if ( _caller.valid() )
      {
        FileResult result ( dialog->getSaveFileName ( "Save Image", filters ) );

        std::string filename ( result.first );
        if ( false == filename.empty() )
        {
          // Create a temp file.
          //std::string filename ( Usul::File::Temp::file() );

          // Make it a bitmap.
          //boost::algorithm::replace_last ( filename, ".tmp", ".bmp" );

          // Save it to our list.
          //_files.push_back ( filename );

          // Some feedback...
          std::cout << "Creating image: " << filename << std::endl;

          // Take the picture.
          snapShot->takePicture ( filename, frameScale, numSamples );

          // Open it up.
          //QProcess::startDetached ( filename.c_str() );
        }
      }
    }
  }
  catch ( const std::exception& e )
  {
    std::cout << "Error 1415262500: Standard exception caught while trying to take snap shot: " << e.what() << std::endl;
  }
  catch ( ... )
  {
    std::cout << "Error 3216169248: Unknown exception caught while trying to take snap shot." << std::endl;
  }
}
