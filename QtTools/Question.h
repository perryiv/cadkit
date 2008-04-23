
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Qt implementation of Usul::Interfaces::IQuestion.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __QT_TOOLS_QUESTION_H__
#define __QT_TOOLS_QUESTION_H__

#include "QtTools/Export.h"

#include <string>

class QWidget;

namespace QtTools
{
  QT_TOOLS_EXPORT std::string question ( QWidget* parent, const std::string &buttons, const std::string &title, const std::string &text );
}

#endif // __QT_TOOLS_QUESTION_H__
