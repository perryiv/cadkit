
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Discretization file reader.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_DISCRETIZATION_READER_CLASS_H_
#define _MODFLOW_MODEL_DISCRETIZATION_READER_CLASS_H_

#include "ModflowModel/Readers/BaseReader.h"


namespace Modflow {
namespace Readers {


class Discretization : public BaseReader
{
public:

  // Useful typedefs.
  typedef BaseReader BaseClass;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Discretization );

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


} // namespace Readers
} // namespace Modflow


#endif // _MODFLOW_MODEL_DISCRETIZATION_READER_CLASS_H_
