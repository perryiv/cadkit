
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
#include "XmlDom/DoNothing.h"

#include <string>


namespace XML {
namespace Config {


template
<
  class NodeCallback_ = XML::Callback::DoNothing < std::string >,
  class TrimPolicy_   = XML::Config::Trim,
  class StringType_   = std::string,
  class ErrorPolicy_  = XML::Config::Pair < XML::Config::Assert, XML::Config::Thrower < std::runtime_error > >
>
class Policy
{
public:

  typedef NodeCallback_ NodeCallback;
  typedef TrimPolicy_   TrimPolicy;
  typedef StringType_   String;
  typedef ErrorPolicy_  ErrorPolicy;
};


}; // namespace Config
}; // namespace XML


#endif // _XML_READER_POLICY_CLASS_H_
