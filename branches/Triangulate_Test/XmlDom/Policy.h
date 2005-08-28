
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Sample policy for xml reader.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _XML_READER_POLICY_CLASS_H_
#define _XML_READER_POLICY_CLASS_H_

#include "XmlDom/Trim.h"
#include "XmlDom/Error.h"
#include "XmlDom/Missing.h"
#include "XmlDom/Filter.h"

#include <string>
#include <stdexcept>


namespace XML {
namespace Config {


template
<
  class TrimPolicy_    = XML::Config::Trim,
  class StringType_    = std::string,
  class MissingPolicy_ = XML::Config::CreateMissing,
  class FilterPolicy_  = XML::Filter::Use2,
  class ErrorPolicy_   = XML::Config::Pair < XML::Config::Assert, XML::Config::Thrower < std::runtime_error > >
>
class Policy
{
public:

  typedef TrimPolicy_     TrimPolicy;
  typedef StringType_     String;
  typedef MissingPolicy_  MissingPolicy;
  typedef FilterPolicy_   FilterPolicy;
  typedef ErrorPolicy_    ErrorPolicy;
};


}; // namespace Config
}; // namespace XML


#endif // _XML_READER_POLICY_CLASS_H_
