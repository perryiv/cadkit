
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base creator class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SERIALIZE_XML_BASE_CREATOR_CLASS_
#define _SERIALIZE_XML_BASE_CREATOR_CLASS_

#include "Serialize/XML/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <string>


namespace Serialize {
namespace XML {


///////////////////////////////////////////////////////////////////////////////
//
//  Base creator class.
//
///////////////////////////////////////////////////////////////////////////////

class SERIALIZE_XML_EXPORT BaseCreator : public Usul::Base::Referenced 
{
public:

  USUL_DECLARE_REF_POINTERS ( BaseCreator );
  typedef Usul::Base::Referenced BaseClass;

  BaseCreator ( const std::string &name );

  virtual Usul::Base::Referenced *  operator()() = 0;

  const std::string &               name();

protected:

  virtual ~BaseCreator();

  std::string _name;
};


} // namespace Serialize
} // namespace XML


#endif // _SERIALIZE_XML_BASE_CREATOR_CLASS_
