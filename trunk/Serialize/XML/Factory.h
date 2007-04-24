
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
//  Serialization factory class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SERIALIZE_XML_FACTORY_CLASS_
#define _SERIALIZE_XML_FACTORY_CLASS_

#include "Serialize/XML/Export.h"
#include "Serialize/XML/BaseCreator.h"

#include <map>


namespace Serialize {
namespace XML {


///////////////////////////////////////////////////////////////////////////////
//
//  Singleton factory class.
//
///////////////////////////////////////////////////////////////////////////////

class SERIALIZE_XML_EXPORT Factory 
{
public:

  typedef std::map < std::string, BaseCreator::ValidRefPtr > Creators;

  void                              add ( BaseCreator *c );

  Usul::Base::Referenced *          create ( const std::string &name );

  static Factory &                  instance();
  static void                       instance ( Factory *f );

  void                              remove ( const std::string &name );

private:

  Factory();
  ~Factory();

  static Factory *_instance;
  Creators _creators;
};


} // namespace Serialize
} // namespace XML


#endif // _SERIALIZE_XML_FACTORY_CLASS_
