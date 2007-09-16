
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Discretization file reader.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_DISCRETIZATION_READER_CLASS_H_
#define _MODFLOW_MODEL_DISCRETIZATION_READER_CLASS_H_

#include "BaseReader.h"


class Discretization : public BaseReader
{
public:

  // Useful typedefs.
  typedef BaseReader BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Discretization );

  // Construction.
  Discretization();

  // Read the file.
  virtual void            read ( ModflowDocument *doc, const std::string &file, Unknown *progress );

protected:

  // Use reference counting.
  virtual ~Discretization();

private:

  // Do not copy.
  Discretization ( const Discretization & );
  Discretization &operator = ( const Discretization & );
};


#endif // _MODFLOW_MODEL_DISCRETIZATION_READER_CLASS_H_
