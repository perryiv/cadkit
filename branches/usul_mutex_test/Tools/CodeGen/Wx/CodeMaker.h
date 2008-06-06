
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CODE_MAKER_H__
#define __CODE_MAKER_H__

#include <string>
#include <vector>

class CodeMaker
{
public:
  typedef std::vector < std::string > StringArray;

  /// It is a singleton.
  static CodeMaker&     instance();

  void           createPlugin        ( const std::string& creator,
                                       const std::string& plugin, 
                                       const std::string& directory, 
                                       const StringArray& interfaces,
                                       std::string& errors );

  void           createInterface     ( const std::string& creator, const std::string& name, std::string& errors );

  void           availableLibraryDirectories  ( StringArray& libraries );

private:
  
  void           _handleCompileGuard ( const std::string& creator, const std::string& plugin, const std::string& directory, std::string& errors );
  void           _handleComponent    ( const std::string& creator, const std::string& plugin, const std::string& directory, const StringArray& interfaces, std::string& errors );
  void           _handleFactory      ( const std::string& creator, const std::string& plugin, const std::string& directory, std::string& errors );
  void           _handleExport       ( const std::string& creator, const std::string& plugin, const std::string& directory, std::string& errors );
  void           _handleProjectFiles ( const std::string& creator, const std::string& plugin, const std::string& directory, std::string& errors );

  void           _transformXML       ( const std::string& xml, const std::string& xslt, const std::string& output, std::string& errors );
  void           _transformXML       ( std::istream& xml, std::istream& xslt, std::ofstream& result, std::string& errors );

  template < class Strings >
    void           _searchAndReplace   ( const std::string& inFile, const std::string& outFile, const Strings& strings );

  std::string    _componentClassName   ( const std::string& plugin );
  std::string    _factoryClassName     ( const std::string& plugin );
  std::string    _compileGuard         ( const std::string& plugin );

  CodeMaker();
  ~CodeMaker();

  static CodeMaker* _instance;
};


#endif // __CODE_MAKER_H__

