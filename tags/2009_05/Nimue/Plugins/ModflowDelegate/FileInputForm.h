
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

#ifndef __MODFLOW_FILE_INPUT_FORM_H__
#define __MODFLOW_FILE_INPUT_FORM_H__

#include "QtGui/QWidget"

#include <string>

class QLineEdit;


namespace Modflow {


class FileInputForm : public QWidget
{
public:

  typedef QWidget BaseClass;
  
  FileInputForm ( QWidget *parent = 0x0 );
  virtual ~FileInputForm();
  
  virtual std::string buildXml() const = 0;
  
  // Does this form only get added once?
  virtual bool        addOnce() const = 0;

protected:

  void                _browse ( const std::string &filters, const std::string &regSection, QLineEdit *fileName );
};


} // namespace Modflow


#endif // __MODFLOW_FILE_INPUT_FORM_H__
