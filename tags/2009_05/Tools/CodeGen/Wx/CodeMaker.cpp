
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "CodeMaker.h"
#include "XMLFile.h"

#include "Usul/File/Slash.h"

#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/SAX/SAXException.hpp"
#include "xercesc/SAX/SAXParseException.hpp"

#include "xalanc/Include/PlatformDefinitions.hpp"
#include "xalanc/XalanTransformer/XalanTransformer.hpp"
#include "xalanc/XSLT/XSLTInputSource.hpp"

#include "boost/algorithm/string/replace.hpp"
#include "boost/algorithm/string/find.hpp"
#include "boost/range/iterator_range.hpp"

#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <cstdlib>

#ifdef _WIN32
#include "windows.h"
#endif

/// Initiliaze static members
CodeMaker* CodeMaker::_instance ( 0x0 );

///////////////////////////////////////////////////////////////////////////////
//
//  Get the instance.
//
///////////////////////////////////////////////////////////////////////////////

CodeMaker& CodeMaker::instance()
{
  if ( 0x0 == _instance )
    _instance = new CodeMaker;

  return *_instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

CodeMaker::CodeMaker()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

CodeMaker::~CodeMaker()
{
}



///////////////////////////////////////////////////////////////////////////////
//
//  Create files for the plugin.
//
///////////////////////////////////////////////////////////////////////////////

void CodeMaker::createPlugin ( const std::string& creator,
                               const std::string& pluginName, 
                               const std::string& directory, 
                               const StringArray& interfaces,
                               std::string& errors )
{
  this->_handleCompileGuard ( creator, pluginName, directory, errors );
  this->_handleComponent    ( creator, pluginName, directory, interfaces, errors );
  this->_handleFactory      ( creator, pluginName, directory, errors );
  this->_handleExport       ( creator, pluginName, directory, errors );
  this->_handleProjectFiles ( creator, pluginName, directory, errors );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the compile guard.
//
///////////////////////////////////////////////////////////////////////////////

void CodeMaker::_handleCompileGuard ( const std::string& name, const std::string& plugin, const std::string& directory, std::string& errors )
{
  std::ostringstream xml;
  XMLFile file ( xml );

  file.openTag( "PluginTemplate" );

  typedef XMLFile::Strings Strings;
  {
    Strings keys, values;
    keys.push_back ( "name" );
    values.push_back ( name );
	  file.tagWithAttributes("User", keys, values, "" );
  }

  std::string headerDefine ( plugin );

  std::transform ( headerDefine.begin(), headerDefine.end(), headerDefine.begin(), ::toupper );
  std::replace ( headerDefine.begin(), headerDefine.end(), ' ', '_' );
  headerDefine = "__" + headerDefine + "_COMPILE_GUARD_H__";

  std::string compileGuard = "_COMPILING_" + _compileGuard ( plugin );

  {
    Strings keys, values;
    keys.push_back ( "headerDefine" );
    keys.push_back ( "compilePreprocess" );
    
    values.push_back ( headerDefine );
    values.push_back ( compileGuard  );
    file.tagWithAttributes( "CompileGuard", keys, values, "" );
  }

  file.closeTag( "PluginTemplate" );

  std::string filename ( directory + "/CompileGuard.h" );

  this->_transformXML ( xml.str(), "CompileGuard.xsl", filename, errors );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the component files.
//
///////////////////////////////////////////////////////////////////////////////

void CodeMaker::_handleComponent ( const std::string& name, const std::string& plugin, const std::string& directory, const StringArray& interfaces, std::string& errors )
{
  std::ostringstream xml;
  XMLFile file ( xml );
  
  file.openTag( "PluginTemplate" );

  typedef XMLFile::Strings Strings;
  {
    Strings keys, values;
    keys.push_back ( "name" );
    values.push_back ( name );
	  file.tagWithAttributes("User", keys, values, "" );
  }

  file.openTag( "ComponentSource" );
  file.closeTag("ComponentSource" );

  // Get the component class name.
  std::string componentClassName ( this->_componentClassName ( plugin ) );

  std::string includeGuard ( "__" + componentClassName + "_H__" );
  std::transform ( includeGuard.begin(), includeGuard.end(), includeGuard.begin(), ::toupper );

  {
    Strings keys ( 3 ), values ( 3 );
    keys[0] = "classDescription";
    keys[1] = "compileGuard";
    keys[2] = "className";
		values[0] = "";
		values[1] = includeGuard;
		values[2] = componentClassName;
		file.openTagWithAttributes( "ComponentHeader", keys, values );
  }


  // Leaving this here for an example.
#if 0
  {
    Strings keys, values;
    keys.push_back ( "file" );
    values.push_back ( "Usul/Interfaces/IPlugin.h" );
		file.tagWithAttributes( "LibInclude", keys, values, "" );
  }
#endif

  {
    // Leaving this here for an example.
#if 0
    Strings keys, values;
    keys.push_back ( "file" );
    values.push_back ( "vector" );
		file.tagWithAttributes( "SystemInclude", keys, values, "" );
#endif
  }

  
  for ( StringArray::const_iterator iter = interfaces.begin(); iter != interfaces.end(); ++iter )
  {
    std::string value ( *iter );

    typedef boost::iterator_range < std::string::iterator > Iterator;
    Iterator slash ( boost::algorithm::find_last ( value, Usul::File::slash() ) );
    Iterator dot   ( boost::algorithm::find_last ( value, "." ) );

    Strings keys, values;

    keys.push_back ( "name" );

    std::string interfaceName ( slash.begin() + 1, dot.begin() );
    values.push_back ( interfaceName );

		file.tagWithAttributes( "UsulInterface", keys, values, "" );
  }

	file.closeTag("ComponentHeader" );

  file.closeTag( "PluginTemplate" );

  // Create the component header.
  {
    std::string filename ( directory + "/" + componentClassName + ".h" );
    this->_transformXML ( xml.str(), "ComponentHeader.xsl", filename, errors );
  }

  // Create the component source.
  {
    std::string filename ( directory + "/" + componentClassName + ".cpp" );
    this->_transformXML ( xml.str(), "ComponentSource.xsl", filename, errors );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the factory files.
//
///////////////////////////////////////////////////////////////////////////////

void CodeMaker::_handleFactory ( const std::string& name, const std::string& plugin, const std::string& directory, std::string& errors )
{
  std::ostringstream xml;
  XMLFile file ( xml );
  
  file.openTag( "PluginTemplate" );

  typedef XMLFile::Strings Strings;
  {
    Strings keys, values;
    keys.push_back ( "name" );
    values.push_back ( name );
	  file.tagWithAttributes("User", keys, values, "" );
  }

  std::string factoryClassName ( this->_factoryClassName( plugin ) );

  std::string includeGuard ( "__" + factoryClassName + "_H__" );
  std::transform ( includeGuard.begin(), includeGuard.end(), includeGuard.begin(), ::toupper );

  {
    Strings keys ( 3 ), values ( 3 );
		keys[0] = "classDescription";
		values[0] = "";
		keys[1] = "compileGuard";
		values[1] = includeGuard;
		keys[2] = "className";
		values[2] = factoryClassName;
		file.tagWithAttributes( "FactoryHeader", keys, values, " ");
  }

	file.openTag( "FactorySource" );

  {
    Strings keys, values;
    keys.push_back ( "file" );
    values.push_back ( this->_componentClassName( plugin ) + ".h" );
		file.tagWithAttributes( "LibInclude", keys, values, "" );
  }

#if 0
  {
    Strings keys, values;
    keys.push_back ( "file" );
    values.push_back ( "Usul/Components/Create.h" );
		file.tagWithAttributes( "LibInclude", keys, values, "" );
  }
#endif

	file.closeTag( "FactorySource" );

  {
    Strings keys ( 3 ), values ( 3 );
    keys[0] = "classDescription";
    keys[1] = "compileGuard";
    keys[2] = "className";
		values[0] = "";
		values[1] = "";
		values[2] = this->_componentClassName( plugin );
		file.openTagWithAttributes( "ComponentHeader", keys, values );
  }

  file.closeTag("ComponentHeader" );

  file.closeTag( "PluginTemplate" );

  // Create the component header.
  {
    std::string filename ( directory + "/" + factoryClassName + ".h" );
    this->_transformXML ( xml.str(), "FactoryHeader.xsl", filename, errors );
  }

  // Create the component source.
  {
    std::string filename ( directory + "/" + factoryClassName + ".cpp" );
    this->_transformXML ( xml.str(), "FactorySource.xsl", filename, errors );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the Export.def file.
//
///////////////////////////////////////////////////////////////////////////////

void CodeMaker::_handleExport ( const std::string&, const std::string& plugin, const std::string& directory, std::string& errors )
{
  std::ostringstream xml;
  XMLFile file ( xml );

  typedef XMLFile::Strings Strings;
  {
    Strings keys, values;
    keys.push_back ( "name" );

    std::string pluginName ( plugin );
    std::string::iterator end = std::remove ( pluginName.begin(), pluginName.end(), ' ' );
    pluginName.resize ( end - pluginName.begin() );
    
    values.push_back ( pluginName );
	  file.tagWithAttributes( "PluginTemplate", keys, values, "" );
  }

  // Create the export.def file
  std::string filename ( directory + "/Export.def" );
  this->_transformXML ( xml.str(), "Export.xsl", filename, errors );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create project files.
//
///////////////////////////////////////////////////////////////////////////////

void CodeMaker::_handleProjectFiles ( const std::string&, const std::string& plugin, const std::string& directory, std::string& errors )
{
  std::ostringstream xml;
  XMLFile file ( xml );

  std::string pluginName ( plugin );
  std::string::iterator end = std::remove ( pluginName.begin(), pluginName.end(), ' ' );
  pluginName.resize ( end - pluginName.begin() );

  std::string compileGuard ( this->_compileGuard ( plugin ) );

  {
    typedef XMLFile::Strings Strings;
    Strings keys, values;
    keys.push_back ( "name" );
    keys.push_back ( "compileGuard" );
    
    values.push_back ( pluginName );
    values.push_back ( compileGuard );

	  file.tagWithAttributes( "PluginTemplate", keys, values, "" );
  }

  // Create Makefile
  {
    std::string filename ( directory + "/Makefile" );
    this->_transformXML ( xml.str(), "Makefile.xsl", filename, errors );
  }  

  std::string guid;

#ifdef _WIN32
  std::vector < unsigned char > b ( 256, 0x0 );
  UUID uuid;
  UuidCreateSequential( &uuid );
  unsigned char *p = &b.front();
  ::UuidToString ( &uuid, &p );

  unsigned int len ( ::strlen ( (char* ) p ) );
  guid.insert ( guid.end(), p, p + len );
#else
  // Create a guid.
  char buf [ 256 ];
  unsigned int t1 ( ::time ( 0x0 ) );

  ::srand ( ::time ( 0x0 ) );

  unsigned int r1 ( ::rand () );
  unsigned int r2 ( ::rand () );
  unsigned int r3 ( ::rand () );
  
  unsigned long t2 ( ::time ( 0x0 ) );
  ::sprintf ( buf, "%08x-%04x-%04x-%04x-%012x", t1, r1, r2, r3, t2 );

  guid = buf;
#endif

  std::string component    ( this->_componentClassName ( plugin ) );
  std::string factory      ( this->_factoryClassName ( plugin ) );

  typedef std::pair < std::string, std::string > StringPair;
  typedef std::vector < StringPair > Strings;

  Strings strings;
  strings.push_back ( StringPair ( "CM_PLUGIN_TITLE", plugin ) );
  strings.push_back ( StringPair ( "CM_PLUGIN_NAME", pluginName ) );
  strings.push_back ( StringPair ( "CM_GUID", guid ) );
  strings.push_back ( StringPair ( "CM_COMPILE_GUARD", compileGuard ) );
  strings.push_back ( StringPair ( "CM_COMPONENT_NAME", component ) );
  strings.push_back ( StringPair ( "CM_FACTORY_NAME", factory ) );

  char *buf ( ::getenv ( "USUL_INC_DIR" ) );

  std::string resourceDir ( buf );
  resourceDir += "/Tools/CodeGen/XLST/";

  // Create Visual Studio 7.1 project file
  {
    std::string filename ( directory + "/" + pluginName + ".vcproj" );
    this->_searchAndReplace ( resourceDir + "skeleton_vs71.xml", filename, strings );
  }

  // Create Visual Studio 8 project file
  {
    std::string filename ( directory + "/" + pluginName + "_vs8.vcproj" );
    this->_searchAndReplace ( resourceDir + "skeleton_vs80.xml", filename, strings );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Transform the xml file using xslt.
//
///////////////////////////////////////////////////////////////////////////////

void CodeMaker::_transformXML ( const std::string& xml, const std::string& input, const std::string& output, std::string& errors )
{
  char *buf ( ::getenv ( "USUL_INC_DIR" ) );

  std::string resourceDir ( buf );
  resourceDir += "/Tools/CodeGen/XLST/";

  std::string xsltFile ( resourceDir + input );

  std::ofstream fout ( output.c_str(), std::ofstream::binary );
  std::ifstream xslt ( xsltFile.c_str(), std::ifstream::in | std::ifstream::binary  );
  std::istringstream in ( xml );

  this->_transformXML ( in, xslt, fout, errors );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Transform the xml file using xslt.
//
///////////////////////////////////////////////////////////////////////////////

void CodeMaker::_transformXML ( std::istream& xml, std::istream& xslt, std::ofstream& result, std::string& errors )
{
    XALAN_USING_XERCES(XMLPlatformUtils)

		XALAN_USING_XALAN(XalanTransformer)

		// Call the static initializer for Xerces.
		XMLPlatformUtils::Initialize();

		// Initialize Xalan.
		XalanTransformer::initialize();
    
    {
      XalanTransformer theXalanTransformer;

      std::ostringstream message;

      try
      {
        theXalanTransformer.transform( xml, xslt, result );
      }
      catch ( xercesc::SAXException &e )
      {
        //std::string error ( e.getMessage() );
        std::string last ( theXalanTransformer.getLastError() );
        message << e.getMessage() << last;
      }
      catch ( std::exception &e )
      {
        std::string error ( e.what() );
        std::string last ( theXalanTransformer.getLastError() );
        message << error << last;
      }
      catch ( ... )
      {
        std::string last ( theXalanTransformer.getLastError() );
        message << last;
      }

      message << theXalanTransformer.getLastError();

      if ( !message.str().empty() )
      {
        message << std::endl;
        errors += message.str();
      }
    }

    // Terminate Xalan...
		XalanTransformer::terminate();

		// Terminate Xerces...
		XMLPlatformUtils::Terminate();

		// Clean up the ICU, if it's integrated...
		XalanTransformer::ICUCleanUp();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the component class name.
//
///////////////////////////////////////////////////////////////////////////////

std::string CodeMaker::_componentClassName ( const std::string& plugin )
{
  std::string componentClassName ( plugin + "Component" );
  std::string::iterator end  ( std::remove ( componentClassName.begin(), componentClassName.end(), ' ' ) ); 
  componentClassName.resize ( end - componentClassName.begin() );

  return componentClassName;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the factory class name.
//
///////////////////////////////////////////////////////////////////////////////

std::string CodeMaker::_factoryClassName ( const std::string& plugin )
{
  std::string factoryClassName ( plugin + "Factory" );
  std::string::iterator end ( std::remove ( factoryClassName.begin(), factoryClassName.end(), ' ' ) );
  factoryClassName.resize ( end - factoryClassName.begin() );

  return factoryClassName;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the compile guard name.
//
///////////////////////////////////////////////////////////////////////////////

std::string CodeMaker::_compileGuard ( const std::string& plugin )
{
  std::string compileGuard ( plugin );
  std::transform ( compileGuard.begin(), compileGuard.end(), compileGuard.begin(), ::toupper );
  std::replace ( compileGuard.begin(), compileGuard.end(), ' ', '_' );
  return compileGuard;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the Usul Interface.
//
///////////////////////////////////////////////////////////////////////////////

void CodeMaker::createInterface     ( const std::string& creator, const std::string& interfaceName, std::string& errors )
{
  std::string guard ( interfaceName + "_H__" );
  std::transform ( guard.begin(), guard.end(), guard.begin(), ::toupper );

  char *buf ( ::getenv ( "USUL_INC_DIR" ) );

  std::string usuldir ( buf );
  usuldir += "/Usul/Interfaces/";

  std::string filename ( interfaceName );
  filename += ".h";

  unsigned int num ( ::time( 0x0 ) );
  std::ostringstream out;
  out << num;
  while ( out.tellp() < 10 )
    out << '0';
  out << 'u';

  std::string iid ( out.str() );

  typedef XMLFile::Strings Strings;

  std::ostringstream xml;
  XMLFile file ( xml );

  file.openTag( "PluginTemplate" );

  {
    Strings keys, values;
    keys.push_back ( "name" );
    values.push_back ( creator );
		file.tagWithAttributes("User", keys, values, "" );
  }

  {
    Strings keys (4), values(4);
		keys[0] = "classDescription";
		values[0] = "";
		keys[1] = "compileGuard";
		values[1] = guard;
		keys[2] = "className";
		values[2] = interfaceName;
		keys[3] = "IID";
		values[3] = iid;
		file.openTagWithAttributes( "UsulHeader", keys, values );
		file.closeTag( "UsulHeader" );
  }

	file.closeTag( "PluginTemplate" );

  this->_transformXML ( xml.str(), "UsulHeader.xsl", usuldir += filename, errors );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Search for the keys in inFile and replace them with given values.
//  Outputs result into outFile.
//
///////////////////////////////////////////////////////////////////////////////

template < class Strings >
void CodeMaker::_searchAndReplace   ( const std::string& inFile, const std::string& outFile, const Strings& strings )
{
  typedef typename Strings::value_type StringPair;
  typedef typename Strings::const_iterator Iterator;

  std::ifstream fin ( inFile.c_str() );

  std::ostringstream file;

  while ( !fin.eof() )
  {
    const unsigned int size ( 1024 );
    char buf [ size ];
    fin.getline ( buf, size );

    file << buf << std::endl;
  }

  std::string contents ( file.str() );

  for ( Iterator iter = strings.begin(); iter != strings.end(); ++iter )
  {
    std::string search ( iter->first );
    std::string replace ( iter->second );

    boost::algorithm::replace_all( contents, search, replace );
  }

  std::ofstream fout ( outFile.c_str(), std::ofstream::out | std::ofstream::binary  );
  fout << contents;
}

