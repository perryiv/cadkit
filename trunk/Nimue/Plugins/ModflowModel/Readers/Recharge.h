
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
//  Recharge input reader.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_RECHARGE_INPUT_READER_CLASS_H_
#define _MODFLOW_MODEL_RECHARGE_INPUT_READER_CLASS_H_

#include "ModflowModel/Readers/BaseReader.h"

#include "Usul/Types/Types.h"


namespace Modflow {
namespace Readers {


class Recharge : public BaseReader
{
public:

  // Useful typedefs.
  typedef BaseReader BaseClass;
  typedef Usul::Types::Uint64 TimeStamp;
  typedef std::pair<TimeStamp,TimeStamp> TimeSpan;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Recharge );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Recharge );

  // Construction.
  Recharge();

  // Read the file.
  virtual void            read ( ModflowDocument *doc, const std::string &file, Unknown *progress );

protected:

  // Use reference counting.
  virtual ~Recharge();

  bool                    _getTimeSpan ( const std::string &, TimeSpan & );

private:

  // Do not copy.
  Recharge ( const Recharge & );
  Recharge &operator = ( const Recharge & );
};


} // namespace Readers
} // namespace Modflow


#endif // _MODFLOW_MODEL_RECHARGE_INPUT_READER_CLASS_H_
