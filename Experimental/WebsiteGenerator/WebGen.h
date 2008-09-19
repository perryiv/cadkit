
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
//  Class to generate an html page.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _WEBSITE_GENERATOR_CLASS_
#define _WEBSITE_GENERATOR_CLASS_

#include "XmlTree/Document.h"

#include "Usul/Registry/Database.h"

#include <map>
#include <string>
#include <vector>


class WebGen
{
public:

  typedef Usul::Registry::Node RegistryNode;
  typedef std::map<std::string,std::string> StringMap;
  typedef std::pair<std::string,std::string> PageInfo;
  typedef std::vector<PageInfo> PageList;
  typedef std::map<std::string,std::string> PageMap;
  typedef std::vector<XmlTree::Node::ValidRefPtr> NodeVector;
  typedef std::vector<NodeVector> NodeMatrix;

  WebGen ( int argc, char **argv, char **env );
  ~WebGen();

  void                               run();

protected:

  void                               _addScripts ( XmlTree::Node::ValidRefPtr parent );
  void                               _addStyles  ( XmlTree::Node::ValidRefPtr parent );
  void                               _appendChildren ( XmlTree::Node::ValidRefPtr from, XmlTree::Node::ValidRefPtr to );

  XmlTree::Node::ValidRefPtr         _cell ( unsigned int row, unsigned int column );

  std::string                        _directory ( Usul::Registry::Node & );

  void                               _findPages();

  XmlTree::Node::ValidRefPtr         _link ( const std::string &text, const std::string &href, bool internal = true );
  XmlTree::Node::ValidRefPtr         _loadXmlFile ( const std::string &file ) const;

  XmlTree::Node::ValidRefPtr         _makeBody();
  XmlTree::Node::ValidRefPtr         _makeHead();
  XmlTree::Node::ValidRefPtr         _makeImage ( const std::string &src, const std::string &alt );
  XmlTree::Node::ValidRefPtr         _makeTable ( unsigned int numRows, unsigned int numCols, const std::string &cssClassName, NodeMatrix &matrix ) const;

  std::string                        _pageName() const;

  std::string                        _queryValue ( const std::string &name, const std::string &defaultValue = std::string() ) const;

  void                               _replaceDefinitions ( XmlTree::Node::Children &from, XmlTree::Node::ValidRefPtr to );

  std::string                        _specialChar ( const std::string &name, unsigned int num ) const;

private:

  RegistryNode &_query;
  RegistryNode &_env;
  RegistryNode &_site;
  StringMap _chars;
  PageList _pageList;
  PageMap _pageMap;
  NodeMatrix _matrix;
};


#endif // _WEBSITE_GENERATOR_CLASS_
