
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Basic package file reader.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_BASIC_PACKAGE_READER_CLASS_H_
#define _MODFLOW_MODEL_BASIC_PACKAGE_READER_CLASS_H_

#include "BaseReader.h"


class BasicPackage : public BaseReader
{
public:

  // Useful typedefs.
  typedef BaseReader BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( BasicPackage );

  // Construction.
  BasicPackage();

  // Read the file.
  virtual void            read ( ModflowDocument *doc, const std::string &file, Unknown *progress );

protected:

  // Use reference counting.
  virtual ~BasicPackage();

private:

  // Do not copy.
  BasicPackage ( const BasicPackage & );
  BasicPackage &operator = ( const BasicPackage & );
};


#endif // _MODFLOW_MODEL_BASIC_PACKAGE_READER_CLASS_H_
