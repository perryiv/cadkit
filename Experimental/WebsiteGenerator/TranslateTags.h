
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
//  Class to translate tags.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _WEBSITE_GENERATOR_TRANSLATE_TAGS_CLASS_
#define _WEBSITE_GENERATOR_TRANSLATE_TAGS_CLASS_

#include "XmlTree/Node.h"

#include "Usul/Registry/Node.h"

#include <map>
#include <string>


class TranslateTags
{
public:

  typedef std::map<std::string,std::string> Tags;

  TranslateTags ( Usul::Registry::Node &site );
  ~TranslateTags();

  void                               add ( const std::string &from, const std::string &to );

  void                               operator () ( XmlTree::Node::ValidRefPtr node );

private:

  Usul::Registry::Node &_site;
  Tags _tags;
};


#endif // _WEBSITE_GENERATOR_TRANSLATE_TAGS_CLASS_
