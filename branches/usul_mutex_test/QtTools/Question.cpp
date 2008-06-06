
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "QtTools/Question.h"

#include "Usul/Strings/Split.h"

#include "QtGui/QPushButton"
#include "QtGui/QMessageBox"

#include <map>

///////////////////////////////////////////////////////////////////////////////
//
//  Qt implementation of Usul::Interfaces::IQuestion.
//
///////////////////////////////////////////////////////////////////////////////

std::string QtTools::question ( QWidget* parent, const std::string &buttons, const std::string &title, const std::string &text )
{
  // The message box.
  QMessageBox message ( QMessageBox::Question, title.c_str(), text.c_str(), 0, parent );

  // Typedefs.
  typedef std::map<QAbstractButton*, std::string> ButtonMap;
  typedef std::vector<std::string> Strings;
  
  // Get the requested buttons.
  Strings strings;
  Usul::Strings::split ( buttons, "|", false, strings );

  // Save button to text mapping.
  ButtonMap buttonMap;

  // Add buttons to dialog.
  for ( Strings::const_iterator iter = strings.begin(); iter != strings.end(); ++iter )
  {
    QAbstractButton *button ( 0x0 );

    std::string value ( *iter );
    std::transform ( value.begin(), value.end(), value.begin(), ::tolower );

    // See what button is requested.
    if ( "yes" == value )
      button = message.addButton ( QMessageBox::Yes );
    else if ( "no" == value )
      button = message.addButton ( QMessageBox::No );
    else if ( "cancel" == value )
      button = message.addButton ( QMessageBox::Cancel );

    // Save the button to text mapping.
    if ( 0x0 != button )
      buttonMap[button] = *iter;
  }

  // Show the message box.
  message.exec();

  // Get the button that was pressed.
  QAbstractButton *button ( message.clickedButton() );
  return buttonMap[button];
}
