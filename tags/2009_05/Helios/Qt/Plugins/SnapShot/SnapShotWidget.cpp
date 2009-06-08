
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "SnapShotWidget.h"

#include "Usul/File/Path.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Interfaces/ISaveFileDialog.h"
#include "Usul/Interfaces/ISnapShot.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SnapShotWidget::SnapShotWidget( QWidget *parent, Usul::Interfaces::IUnknown* caller ) : BaseClass ( parent ),
  _caller ( caller ),
  _lastFilename(),
  _count ( 0 )
{
  this->setupUi ( this );

  _numSamplesLabel->setVisible ( false );
  _numSamples->setVisible ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SnapShotWidget::~SnapShotWidget()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a snap shot.
//
///////////////////////////////////////////////////////////////////////////////

void SnapShotWidget::on_snapShotButton_clicked()
{
  try
  {
    Usul::Interfaces::ISnapShot::QueryPtr snapShot ( Usul::Documents::Manager::instance().activeView() );

    if ( snapShot.valid () )
    {
      // Get the parameters for the snap shot.
      const double frameScale ( _frameScale->value() );
      const unsigned int numSamples ( _numSamples->value() );

      std::string filename ( this->_filename() );
      if ( false == filename.empty() )
      {
        // Force a redraw now to make sure the dialog's pixels are gone.
        Usul::Interfaces::IRedraw::QueryPtr redraw ( snapShot );
        if ( true == redraw.valid() )
        {
          redraw->redraw();
        }

        // Some feedback...
        std::cout << "Creating image: " << filename << std::endl;

        // Take the picture.
        snapShot->takePicture ( filename, frameScale, numSamples );
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


///////////////////////////////////////////////////////////////////////////////
//
//  Get a filename.
//
///////////////////////////////////////////////////////////////////////////////

std::string SnapShotWidget::_filename()
{
  typedef Usul::Interfaces::ISaveFileDialog::Filter  Filter;
  typedef Usul::Interfaces::ISaveFileDialog::Filters Filters;
  typedef Usul::Interfaces::ISaveFileDialog::FileResult FileResult;
  
  Filters filters;
  filters.push_back ( Filter ( "Bitmap (*.bmp)", "(*.bmp)" ) );
  filters.push_back ( Filter ( "JPEG (*.jpg)", "(*.jpg)" ) );
  filters.push_back ( Filter ( "PNG (*.png)", "(*.png)" ) );

  Usul::Interfaces::ISaveFileDialog::QueryPtr dialog ( _caller );

  // Filename.
  std::string filename ( "" );

  const bool increment ( Qt::Checked == _incrementFilename->checkState() );
  const bool prompt ( _lastFilename.empty() || false == increment );

  if ( dialog.valid() && prompt )
  {
    FileResult result ( dialog->getSaveFileName ( "Save Image", filters ) );

    filename = result.first;
    _lastFilename = filename;
  }
  
  if ( increment && false == _lastFilename.empty() )
  {
    // Make the zero-padded number.
    std::ostringstream out;
    out << std::setw ( 9 ) << _count++;
    std::string number ( out.str() );
    std::replace ( number.begin(), number.end(), ' ', '0' );

    const std::string directory ( Usul::File::directory ( _lastFilename, true ) );
    const std::string base ( Usul::File::base ( _lastFilename ) );
    const std::string ext ( Usul::File::extension ( _lastFilename ) );

    filename =  directory + base + number + "." + ext;
  }

  return filename;
}
