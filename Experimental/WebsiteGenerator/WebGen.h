
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
  typedef std::vector<PageInfo> Pages;
  typedef std::vector<XmlTree::Node::ValidRefPtr> NodeVector;
  typedef std::vector<NodeVector> NodeMatrix;

  WebGen ( int argc, char **argv, char **env );
  ~WebGen();

  void                               run();

protected:

  void                               _addScripts ( XmlTree::Node::ValidRefPtr parent );
  void                               _addStyles  ( XmlTree::Node::ValidRefPtr parent );

  XmlTree::Node::ValidRefPtr         _cell ( unsigned int row, unsigned int column );

  std::string                        _directory ( Usul::Registry::Node & );

  void                               _findPages();

  XmlTree::Node::ValidRefPtr         _loadXmlFile ( const std::string &file ) const;

  std::string                        _protocol() const;

  XmlTree::Node::ValidRefPtr         _makeBody();
  XmlTree::Node::ValidRefPtr         _makeHead();
  XmlTree::Node::ValidRefPtr         _makeImage ( const std::string &src, const std::string &alt );
  XmlTree::Node::ValidRefPtr         _makeTable ( unsigned int numRows, unsigned int numCols, const std::string &cssClassName, NodeMatrix &matrix ) const;

  std::string                        _specialChar ( const std::string &name, unsigned int num ) const;

  std::string                        _urlDomain ( bool wantSlash ) const;
  std::string                        _urlScriptDir ( bool wantSlash ) const;
  std::string                        _urlScript() const;

private:

  RegistryNode &_query;
  RegistryNode &_env;
  StringMap _chars;
  Pages _pages;
  NodeMatrix _matrix;
};


#endif // #define _WEBSITE_GENERATOR_CLASS_
