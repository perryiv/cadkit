
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "Experimental/GeneralShader/GeneralShader/GeneralShaderDocument.h"
#include "Experimental/GeneralShader/GeneralShader/Commands/SetModelCommand.h"
#include "Experimental/GeneralShader/GeneralShader/Commands/SetShaderCommand.h"
#include "Experimental/GeneralShader/GeneralShader/Commands/ReadSettingsCommand.h"

#include "Usul/Interfaces/IDisplaylists.h"
#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Interfaces/IViewPort.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Predicates/FileExists.h"

#include "Usul/Strings/Convert.h"
#include "Usul/Strings/Case.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Trace/Trace.h"
#include "Usul/File/Path.h"
#include "Usul/File/Find.h"
#include "Usul/File/Slash.h"
#include "Usul/Print/Matrix.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Math/Constants.h"
#include "Usul/System/Directory.h"

#include "OsgTools/DisplayLists.h"
#include "OsgTools/Group.h"
#include "OsgTools/Convert.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
#include "MenuKit/ToggleButton.h"
#include "MenuKit/RadioButton.h"

#include "osgDB/ReadFile"
#include "osg/Group"
#include "osg/Switch"
#include "osg/MatrixTransform"
#include "osgDB/WriteFile"
#include "osg/StateSet"
#include "osg/Texture2D"
#include "osg/BoundingBox"
#include "osg/Shader"
#include "osg/Program"
#include "osg/Uniform"
#include "osg/Image"

#include "osg/ShapeDrawable"
#include "osg/StateSet"
#include "osg/PositionAttitudeTransform"
#include "osgDB/ReadFile"
#include "osg/TexGen"
#include "osg/MatrixTransform"
#include "osg/Matrix"
#include "osg/TexEnv"
#include "osg/TexMat"
#include "osg/NodeCallback"
#include "osg/Depth"

#include "osgUtil/CullVisitor"

#include <iterator>
#include <vector>
#include <fstream>
#include <iostream>


///////////////////////////////////////////////////////////////////////////////
//
//  Cull Callback for rotating the compass based on the current view matrix
//
///////////////////////////////////////////////////////////////////////////////

class ViewMatrixCallback : public osg::NodeCallback
{
    public:
      ViewMatrixCallback(  )  :
            _eye( 0.0, 0.0, 0.0 )
		  {
  			
		  }
      ~ViewMatrixCallback()         
		  {
		  }
      virtual void operator()(osg::Node *node, osg::NodeVisitor *nv)
      {
          osgUtil::CullVisitor *cv = dynamic_cast<osgUtil::CullVisitor *>(nv);
          if( 0L != cv)
          {
          #if 1
            osg::Matrix VM = cv->getState()->getInitialViewMatrix();
            osg::Vec3d temp( 0, 0, 0 );
            VM.getLookAt( _eye, temp, temp );
          #else
            _eye = cv->getEyeLocal();
          #endif
          }
          traverse( node, nv );
      }
		  
      osg::Vec3d getEyePosition()
      {
        return _eye;
      }
	 protected:
     osg::Vec3d _eye;
}; 


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( GeneralShaderDocument, GeneralShaderDocument::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

GeneralShaderDocument::GeneralShaderDocument() : 
  BaseClass ( "General Shader Document" ),
  _root( new osg::Group ),
  _scene( new osg::Group ),
  _eyePosition( new osg::Uniform( "eyePosition", osg::Vec3( 0.0, 0.0, 0.0 ) ) ),
  _modelMatrixUniform( new osg::Uniform( "modelMatrix", osg::Matrixd()  ) ),
  _currentModel( 0 ),
  _currentShader( 1 ),
  _workingDir( "." ),
  _eye( 0.0, 0.0, 0.0 ),
  _light( -50.0, -30.0, 100.0 ),
  _theta( 0.0, 0.0 ),
  _radius( 50.0 ),
  _modelMatrixTransform( new osg::MatrixTransform ),
  _heightMapImage( new osg::Image )
{
  USUL_TRACE_SCOPE;
   
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

GeneralShaderDocument::~GeneralShaderDocument()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

inline std::vector< std::string > split_string( const std::string& str, const std::string& split_str ) 
{
	std::vector< std::string > stlv_string;
	std::string part_string("");
	std::string::size_type i;
	
	i=0;
	while( i < str.size() ) 
	{
		if( split_str.find( str[i] ) != std::string::npos ) 
		{
			stlv_string.push_back( part_string );
			part_string = "";
			while( split_str.find( str[i] ) != std::string::npos ) 
			{
				++i;
			}
		}
		else 
		{
			part_string += str[i];
			++i;
		}
	}
	if ( false == part_string.empty() )
		stlv_string.push_back( part_string );

	return stlv_string;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void GeneralShaderDocument::clear ( Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *GeneralShaderDocument::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;

  switch ( iid )
  {
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  case Usul::Interfaces::IUpdateListener::IID:
    return static_cast < Usul::Interfaces::IUpdateListener* > ( this );
  case Usul::Interfaces::IMenuAdd::IID:
    return static_cast < Usul::Interfaces::IMenuAdd * > ( this );
  case Usul::Interfaces::IGeneralShader::IID:
    return static_cast < Usul::Interfaces::IGeneralShader * > ( this );
  case Usul::Interfaces::IKeyListener::IID:
    return static_cast < Usul::Interfaces::IKeyListener * > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document exports given extension
//
///////////////////////////////////////////////////////////////////////////////

bool GeneralShaderDocument::canExport ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document inserts given extension
//
///////////////////////////////////////////////////////////////////////////////

bool GeneralShaderDocument::canInsert ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return this->canOpen ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document reads given extension
//
///////////////////////////////////////////////////////////////////////////////

bool GeneralShaderDocument::canOpen ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "cse591" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document writes given extension
//
///////////////////////////////////////////////////////////////////////////////

bool GeneralShaderDocument::canSave ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "cse591" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void GeneralShaderDocument::write ( const std::string &name, Unknown *caller, Unknown *progress ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void GeneralShaderDocument::read ( const std::string &name, Unknown *caller, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
   XmlTree::Document::RefPtr document ( new XmlTree::Document );
  _workingDir = Usul::File::directory( name, true );

  document->load ( name );

  if ( "cse591" == document->name() )
  {
    this->_parseXML( *document, caller, progress );
  }


}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

GeneralShaderDocument::Filters GeneralShaderDocument::filtersSave() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  filters.push_back ( Filter ( "General Shader (*.cse591)", "*.cse591" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

GeneralShaderDocument::Filters GeneralShaderDocument::filtersExport() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

GeneralShaderDocument::Filters GeneralShaderDocument::filtersOpen() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  filters.push_back ( Filter ( "General Shader (*.cse591)", "*.cse591" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

GeneralShaderDocument::Filters GeneralShaderDocument::filtersInsert() const
{
  USUL_TRACE_SCOPE;
  return this->filtersOpen();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *GeneralShaderDocument::buildScene ( const BaseClass::Options &options, Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _root->removeChild( 0, _root->getNumChildren() );

  // add the scene to the root node
  _root->addChild( _scene.get() );
  _root->setCullCallback( new ViewMatrixCallback );

  std::cout << "Buildscene complete!" << std::endl;
  return _root.get();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void GeneralShaderDocument::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  {
    Guard guard ( this->mutex() );
    osg::ref_ptr< ViewMatrixCallback > vm ( dynamic_cast< ViewMatrixCallback* > ( _root->getCullCallback() ) );
    
    if( 0x0 != vm )
    {  
      _eye = vm->getEyePosition();   
      _eyePosition->set( _eye );
          
    }
    
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the document.
//
///////////////////////////////////////////////////////////////////////////////

void GeneralShaderDocument::_openDocument ( const std::string &file, Usul::Documents::Document *document, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  //Guard guard ( this->mutex() );
  if ( 0x0 == document )
    return;
#if 1
  if( true == document->canOpen( file ) )
    document->open ( file, caller );
  else
    std::cout << "Unable to open document for file: " << file << std::endl;
#else
    document->open ( file, caller );
#endif
  std::cout << "Done" << std::endl;
  
	
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function to set status bar and flush events.
//
///////////////////////////////////////////////////////////////////////////////

void GeneralShaderDocument::_setStatusBar ( const std::string &text, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  Usul::Interfaces::IStatusBar::QueryPtr status ( caller );
  if ( status.valid() )
    status->setStatusBarText ( text, true );
}



///////////////////////////////////////////////////////////////////////////////
//
//  Read the parameter file and parse elements.
//
///////////////////////////////////////////////////////////////////////////////

void GeneralShaderDocument::_parseXML( XmlTree::Node &node, Unknown *caller, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  

  Attributes& attributes ( node.attributes() );
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    //if ( "usetools" == iter->first )
    //{
    //  //Usul::Strings::fromString ( iter->second, value );
    //}
  }

  Children& children ( node.children() );
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    if ( "models" == node->name() )
    {
      this->_parseModels( *node, caller, progress );
    }
    if ( "simple" == node->name() )
    {
      this->_parseSimpleShaders( *node, caller, progress );
    }
    if ( "environment_map" == node->name() )
    {
      this->_parseEMShaders( *node, caller, progress );

    }
    if ( "bump_map" == node->name() )
    {
      this->_parseBMShaders( *node, caller, progress );

    }
   
  }
  this->_updateScene();
 }


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void GeneralShaderDocument::_parseModels( XmlTree::Node &node, Unknown *caller, Unknown *progress)
{
 USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  Usul::System::Directory::ScopedCwd cwd ( _workingDir );

  Attributes& attributes ( node.attributes() );
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    //if ( "usetools" == iter->first )
    //{

    //  //Usul::Strings::fromString ( iter->second, value );
 
    //}
  }

  Children& children ( node.children() );
  GeodePtr geode ( new osg::Geode );
  std::string name = "";
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
   
    if ( "model" == node->name() )
    {
      Attributes& att ( node->attributes() );
      std::string type = "";
      std::string path = "";
      for ( Attributes::iterator att_iter = att.begin(); att_iter != att.end(); ++att_iter )
      {
        
        if ( "type" == att_iter->first )
        {
          Usul::Strings::fromString ( att_iter->second, type );
          if( "cube" == type )
          {
            geode = this->_createCubeModel();
          }
          if( "sphere" == type )
          {
            geode = this->_createSphereModel();
          }
          if( "torus" == type )
          {
            geode = this->_createTorusModel();
          }
          
        }
        if( "name" == att_iter->first )
        {
          Usul::Strings::fromString ( att_iter->second, name ); 
        }
        if( "path" == att_iter->first )
        {
          Usul::Strings::fromString ( att_iter->second, path ); 
        }

      }
      if( "mesh" == type )
      {
        geode = this->_createMeshModel( path );
      }
    }  
   _modelList[ name ] = _models.size();
   _modelNames.push_back( name );
   _models.push_back( geode );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void GeneralShaderDocument::_parseSimpleShaders( XmlTree::Node &node, Unknown *caller, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  
  Usul::System::Directory::ScopedCwd cwd ( _workingDir );

  Attributes& attributes ( node.attributes() );
  
  std::string vertex = "", fragment = "";
  ShaderGroup shaderGroup;
  shaderGroup.name = "unknown";
  shaderGroup.index = 0;
  shaderGroup.fragment = "";
  shaderGroup.vertex = "";
  shaderGroup.type = "Simple";
  shaderGroup.hasSkyBox = false;
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    if ( "name" == iter->first )
    {
      std::string name = ""; 
      Usul::Strings::fromString ( iter->second, name );
      _shaderNames.push_back( name );
      _shaderList[ name ] = _shaders.size();
      shaderGroup.name = name;
    }
    if ( "vertex" == iter->first )
    {

      Usul::Strings::fromString ( iter->second, vertex );
      shaderGroup.vertex = vertex;
    }
    if ( "fragment" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, fragment );
      shaderGroup.fragment = fragment;
    }


  }

  Children& children ( node.children() );
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    if ( "uniform" == node->name() )
    {
      UniformPtr uniform ( this->_parseUniform( *node, caller, progress ) );
      shaderGroup.uniforms.push_back( uniform );
    }
   
  }

  unsigned int index = _shaderGroups.size();
  _shaderGroups.push_back( shaderGroup );
  this->_createSimpleShader( index, caller, progress );
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void GeneralShaderDocument::_parseBMShaders( XmlTree::Node &node, Unknown *caller, Unknown *progress )
{
 USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  

  Usul::System::Directory::ScopedCwd cwd ( _workingDir );

  Attributes& attributes ( node.attributes() );

  
  ShaderGroup shaderGroup;
  shaderGroup.name = "unknown";
  shaderGroup.vertex = "";
  shaderGroup.fragment = "";
  shaderGroup.cubeList.resize( 6 );
  shaderGroup.type = "BumpMap";
  shaderGroup.normalMap = new osg::Image;
  shaderGroup.heightMap = new osg::Image;
  shaderGroup.normalMapPath = "";
  shaderGroup.hasSkyBox = false;
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    if ( "name" == iter->first )
    {
      std::string name = ""; 
      Usul::Strings::fromString ( iter->second, name );
      _shaderNames.push_back( name );
      _shaderList[ name ] = _shaders.size();
      shaderGroup.name = name;
      shaderGroup.normalMapPath = Usul::Strings::format( "NormalMaps/", name, ".jpg" );
    }
    if ( "vertex" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, shaderGroup.vertex );
    }
    if ( "fragment" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, shaderGroup.fragment );
    }
    if ( "has_sky_box" == iter->first )
    {
      std::string hasSkyBox = "false";
      Usul::Strings::fromString ( iter->second, hasSkyBox );
      if( "true" == hasSkyBox )
      {
        shaderGroup.hasSkyBox = true;
      }
    }
    if ( "height_map" == iter->first )
    {
      std::string hm;
      Usul::Strings::fromString ( iter->second, hm );
      shaderGroup.heightMap = this->_setHeightMap( hm );
    }
    if ( "posx" == iter->first )
    {
      std::string texName = "";
      Usul::Strings::fromString ( iter->second, texName );
      shaderGroup.cubeList.at( 0 ) = texName;
    }
    if ( "negx" == iter->first )
    {
      std::string texName = "";
      Usul::Strings::fromString ( iter->second, texName );
      shaderGroup.cubeList.at( 1 ) = texName;
    }
    if ( "posy" == iter->first )
    {
      std::string texName = "";
      Usul::Strings::fromString ( iter->second, texName );
      shaderGroup.cubeList.at( 3 ) = texName;
    }
    if ( "negy" == iter->first )
    {
      std::string texName = "";
      Usul::Strings::fromString ( iter->second, texName );
      shaderGroup.cubeList.at( 2 ) = texName;
    }
    if ( "posz" == iter->first )
    {
      std::string texName = "";
      Usul::Strings::fromString ( iter->second, texName );
      shaderGroup.cubeList.at( 4 ) = texName;
    }
    if ( "negz" == iter->first )
    {
      std::string texName = "";
      Usul::Strings::fromString ( iter->second, texName );
      shaderGroup.cubeList.at( 5 ) = texName;
    }
  
  }

  Children& children ( node.children() );
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    if ( "uniform" == node->name() )
    {
      UniformPtr uniform ( this->_parseUniform( *node, caller, progress ) );
      shaderGroup.uniforms.push_back( uniform );
    }
   
  }

  unsigned int index = _shaderGroups.size();
  _shaderGroups.push_back( shaderGroup );

  this->_createBMShader( index, caller, progress );
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void GeneralShaderDocument::_parseEMShaders( XmlTree::Node &node, Unknown *caller, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  Usul::System::Directory::ScopedCwd cwd ( _workingDir );
  
  Attributes& attributes ( node.attributes() );

  std::string vertex = "", fragment = "";
  ShaderGroup shaderGroup;
  shaderGroup.name = "unknown";
  shaderGroup.vertex = "";
  shaderGroup.fragment = "";
  shaderGroup.cubeList.resize( 6 );
  shaderGroup.type = "EnvironmentMap";
  shaderGroup.hasSkyBox = true;
  for( unsigned int i = 0; i < 6; ++i )
  {
    shaderGroup.cubeList.push_back( "" );
  }
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    if ( "name" == iter->first )
    {
      std::string name = ""; 
      Usul::Strings::fromString ( iter->second, name );
      _shaderNames.push_back( name );
      _shaderList[ name ] = _shaders.size();
      shaderGroup.name = name;
    }
    if ( "vertex" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, vertex );
      shaderGroup.vertex = vertex;
    }
    if ( "fragment" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, fragment );
      shaderGroup.fragment = fragment;
    }
    if ( "posx" == iter->first )
    {
      std::string texName = "";
      Usul::Strings::fromString ( iter->second, texName );
      shaderGroup.cubeList.at( 0 ) = texName;
    }
    if ( "negx" == iter->first )
    {
      std::string texName = "";
      Usul::Strings::fromString ( iter->second, texName );
      shaderGroup.cubeList.at( 1 ) = texName;
    }
    if ( "posy" == iter->first )
    {
      std::string texName = "";
      Usul::Strings::fromString ( iter->second, texName );
      shaderGroup.cubeList.at( 3 ) = texName;
    }
    if ( "negy" == iter->first )
    {
      std::string texName = "";
      Usul::Strings::fromString ( iter->second, texName );
      shaderGroup.cubeList.at( 2 ) = texName;
    }
    if ( "posz" == iter->first )
    {
      std::string texName = "";
      Usul::Strings::fromString ( iter->second, texName );
      shaderGroup.cubeList.at( 4 ) = texName;
    }
    if ( "negz" == iter->first )
    {
      std::string texName = "";
      Usul::Strings::fromString ( iter->second, texName );
      shaderGroup.cubeList.at( 5 ) = texName;
    }
  }

  Children& children ( node.children() );
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    if ( "uniform" == node->name() )
    {
      UniformPtr uniform ( this->_parseUniform( *node, caller, progress ) );
      shaderGroup.uniforms.push_back( uniform );
    }
   
  }

  unsigned int index = _shaderGroups.size();
  _shaderGroups.push_back( shaderGroup );
  
  this->_createEMShader( index, caller, progress );
}



///////////////////////////////////////////////////////////////////////////////
//
// Add a uniform to the shader
//
///////////////////////////////////////////////////////////////////////////////

osg::Uniform* GeneralShaderDocument::_parseUniform( XmlTree::Node &node, GeneralShaderDocument::Unknown *caller, GeneralShaderDocument::Unknown *progress )
{
  UniformPtr uniform ( new osg::Uniform );

  
  Attributes& attributes ( node.attributes() );
  std::string type = "";
  std::string name = "unknown";
  std::string value = "";
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    if ( "type" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, type );
    }
    if ( "name" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, name );
    }
    if ( "value" == iter->first )
    {
      Usul::Strings::fromString ( iter->second, value );
    }
  }

  // Parse the string representation of the value based upon the type
  std::istringstream is( value ); 
  if ( "float" == type )
  {
    float f = 0;
    is >> f;
    uniform = new osg::Uniform( name.c_str(), float( f ) );

  }
  if ( "vec3" == type )
  {
    float x =0, y = 0, z = 0;
    char temp;
    is >> x >> temp >> y >> temp >> z;
    uniform = new osg::Uniform( name.c_str(), osg::Vec3( x, y, z ) );

  }
  if ( "vec4" == type )
  {
    float x =0, y = 0, z = 0, w = 0;
    char temp;
    is >> x >> temp >> y >> temp >> z >> temp >> w;
    uniform = new osg::Uniform( name.c_str(), osg::Vec4( x, y, z, w ) );
  }
  

  return uniform.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void GeneralShaderDocument::_createSimpleShader( unsigned int index, Unknown *caller, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  Usul::System::Directory::ScopedCwd cwd ( _workingDir );

  GroupPtr group ( new osg::Group );
  osg::ref_ptr< osg::PositionAttitudeTransform > posAttXform ( new osg::PositionAttitudeTransform );
  posAttXform->setPosition( osg::Vec3( 0.0f, 0.0f, 0.0f ) );
  osg::ref_ptr< osg::StateSet > stateset = posAttXform->getOrCreateStateSet();
  ProgramPtr program ( new osg::Program );

  // create and load shaders
  ShaderPtr vertShader ( new osg::Shader( osg::Shader::VERTEX ) );
  ShaderPtr fragShader ( new osg::Shader( osg::Shader::FRAGMENT ) );

#if 1
  vertShader->loadShaderSourceFromFile( _shaderGroups.at( index ).vertex );
  fragShader->loadShaderSourceFromFile( _shaderGroups.at( index ).fragment );
  
  // load shaders into program
  program->addShader( vertShader.get() );
  program->addShader( fragShader.get() );

  // Add the eye position
  stateset->addUniform( _eyePosition.get() );
  
  for( unsigned int i = 0; i < _shaderGroups.at( index ).uniforms.size(); ++i )
  {
    stateset->addUniform( _shaderGroups.at( index ).uniforms.at( i ).get() );
  }

#endif

  stateset->setAttributeAndModes( program.get(), osg::StateAttribute::ON );
  _shaderGroups.at( index ).shader = posAttXform;
  _shaders.push_back( posAttXform.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void GeneralShaderDocument::_createEMShader( unsigned int index, Unknown *caller, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  Usul::System::Directory::ScopedCwd cwd ( _workingDir );

  GroupPtr group ( new osg::Group );
  osg::ref_ptr< osg::PositionAttitudeTransform > posAttXform ( new osg::PositionAttitudeTransform );
  posAttXform->setPosition( osg::Vec3( 0.0f, 0.0f, 0.0f ) );
  osg::ref_ptr< osg::StateSet > stateset = posAttXform->getOrCreateStateSet();
  ProgramPtr program ( new osg::Program );

  // create and load shaders
  ShaderPtr vertShader ( new osg::Shader( osg::Shader::VERTEX ) );
  ShaderPtr fragShader ( new osg::Shader( osg::Shader::FRAGMENT ) );

#if 1
  //Environment Map Shaders
  vertShader->loadShaderSourceFromFile( _shaderGroups.at( index ).vertex );
  fragShader->loadShaderSourceFromFile( _shaderGroups.at( index ).fragment );

  // load shaders into program
  program->addShader( vertShader.get() );
  program->addShader( fragShader.get() );

  // Add the eye position
  stateset->addUniform( _eyePosition.get() );  

  // Add Environment map textures
  osg::ref_ptr< osg::TextureCubeMap > map ( new osg::TextureCubeMap );
  map->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
  map->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
  map->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
  map->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
  map->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);    

  // assign the six images to the texture object
  ImagePtr pos_x = osgDB::readImageFile( _shaderGroups.at( index ).cubeList.at( 0 ) );
  ImagePtr neg_x = osgDB::readImageFile( _shaderGroups.at( index ).cubeList.at( 1 ) );
  ImagePtr pos_y = osgDB::readImageFile( _shaderGroups.at( index ).cubeList.at( 2 ) );
  ImagePtr neg_y = osgDB::readImageFile( _shaderGroups.at( index ).cubeList.at( 3 ) );
  ImagePtr pos_z = osgDB::readImageFile( _shaderGroups.at( index ).cubeList.at( 4 ) );
  ImagePtr neg_z = osgDB::readImageFile( _shaderGroups.at( index ).cubeList.at( 5 ) );

  map->setImage(osg::TextureCubeMap::POSITIVE_X, pos_x.get() );
  map->setImage(osg::TextureCubeMap::NEGATIVE_X, neg_x.get() );
  map->setImage(osg::TextureCubeMap::POSITIVE_Y, pos_y.get() );
  map->setImage(osg::TextureCubeMap::NEGATIVE_Y, neg_y.get() );
  map->setImage(osg::TextureCubeMap::POSITIVE_Z, pos_z.get() );
  map->setImage(osg::TextureCubeMap::NEGATIVE_Z, neg_z.get() );

  // enable texturing, replacing any textures in the subgraphs
  stateset->setTextureAttributeAndModes(0, map.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );
  
  // add cube map to stateset
  stateset->addUniform( new osg::Uniform( "map", 0 ) );
  for( unsigned int i = 0; i < _shaderGroups.at( index ).uniforms.size(); ++i )
  {
    stateset->addUniform( _shaderGroups.at( index ).uniforms.at( i ).get() );
  }
#endif
  _shaderGroups.at( index ).emMap = map;
  stateset->setAttributeAndModes( program.get(), osg::StateAttribute::ON );
  _shaderGroups.at( index ).shader = posAttXform;
  _shaders.push_back( posAttXform.get() );
  this->_createSkyBox( index );

}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void GeneralShaderDocument::_createBMShader( unsigned int index, Unknown *caller, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  Usul::System::Directory::ScopedCwd cwd ( _workingDir );

  std::string normalMapPath = _shaderGroups.at( index ).normalMapPath;
  _shaderGroups.at( index ).normalMap = this->_createNormalMap( normalMapPath, index );

  GroupPtr group ( new osg::Group );
  osg::ref_ptr< osg::PositionAttitudeTransform > posAttXform ( new osg::PositionAttitudeTransform );
  posAttXform->setPosition( osg::Vec3( 0.0f, 0.0f, 0.0f ) );
  osg::ref_ptr< osg::StateSet > stateset = posAttXform->getOrCreateStateSet();
  ProgramPtr program ( new osg::Program );

  // create and load shaders
  ShaderPtr vertShader ( new osg::Shader( osg::Shader::VERTEX ) );
  ShaderPtr fragShader ( new osg::Shader( osg::Shader::FRAGMENT ) );

  //Bump Map Shaders
  vertShader->loadShaderSourceFromFile( _shaderGroups.at( index ).vertex );
  fragShader->loadShaderSourceFromFile( _shaderGroups.at( index ).fragment );

  // load shaders into program
  program->addShader( vertShader.get() );
  program->addShader( fragShader.get() );

  // Add the eye position
  stateset->addUniform( _eyePosition.get() );  

  ImagePtr image = osgDB::readImageFile( normalMapPath.c_str() );
  if( true == image.valid() )
  {
    osg::ref_ptr< osg::Texture2D > tex ( new osg::Texture2D );

    tex->setDataVariance( osg::Object::DYNAMIC ); 
    tex->setImage( image.get() );

    stateset->setTextureAttributeAndModes( 0, tex.get(), osg::StateAttribute::ON );
  }
  // add bump map to stateset
  stateset->addUniform( new osg::Uniform( "normalMap", 0 ) );

  for( unsigned int i = 0; i < _shaderGroups.at( index ).uniforms.size(); ++i )
  {
    stateset->addUniform( _shaderGroups.at( index ).uniforms.at( i ).get() );
  }

  // Bump Map has an environment map so add proper uniforms and variables
  if( true == _shaderGroups.at( index ).hasSkyBox )
  {
    Usul::System::Directory::ScopedCwd cwd ( _workingDir );

    // Add Environment map textures
    osg::ref_ptr< osg::TextureCubeMap > map ( new osg::TextureCubeMap );
    map->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    map->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    map->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
    map->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    map->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);    

    // assign the six images to the texture object
    ImagePtr pos_x = osgDB::readImageFile( _shaderGroups.at( index ).cubeList.at( 0 ) );
    ImagePtr neg_x = osgDB::readImageFile( _shaderGroups.at( index ).cubeList.at( 1 ) );
    ImagePtr pos_y = osgDB::readImageFile( _shaderGroups.at( index ).cubeList.at( 2 ) );
    ImagePtr neg_y = osgDB::readImageFile( _shaderGroups.at( index ).cubeList.at( 3 ) );
    ImagePtr pos_z = osgDB::readImageFile( _shaderGroups.at( index ).cubeList.at( 4 ) );
    ImagePtr neg_z = osgDB::readImageFile( _shaderGroups.at( index ).cubeList.at( 5 ) );

    map->setImage(osg::TextureCubeMap::POSITIVE_X, pos_x.get() );
    map->setImage(osg::TextureCubeMap::NEGATIVE_X, neg_x.get() );
    map->setImage(osg::TextureCubeMap::POSITIVE_Y, pos_y.get() );
    map->setImage(osg::TextureCubeMap::NEGATIVE_Y, neg_y.get() );
    map->setImage(osg::TextureCubeMap::POSITIVE_Z, pos_z.get() );
    map->setImage(osg::TextureCubeMap::NEGATIVE_Z, neg_z.get() );

    // enable texturing, replacing any textures in the subgraphs
    stateset->setTextureAttributeAndModes( 1, map.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );
    
    // add cube map to stateset
    stateset->addUniform( new osg::Uniform( "map", 1 ) );
    _shaderGroups.at( index ).emMap = map;

    
  }

  stateset->setAttributeAndModes( program.get(), osg::StateAttribute::ON );
  _shaderGroups.at( index ).shader = posAttXform;
  _shaders.push_back( posAttXform.get() );

  if( true == _shaderGroups.at( index ).hasSkyBox )
  {
    this->_createSkyBox( index );
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

osg::Geode* GeneralShaderDocument::_createSphereModel()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  GeodePtr geode( new osg::Geode );
#if 1
  geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),1.0f)));
#else
  osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );
  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
  osg::ref_ptr< osg::Vec3Array > normals ( new osg::Vec3Array );
  osg::ref_ptr< osg::Vec4Array > tangents ( new osg::Vec4Array );
  osg::ref_ptr< osg::Vec2Array > texCoords ( new osg::Vec2Array );

  for( unsigned int i = 0; i < 9; ++i )
  {
    for( unsigned int j = 0; j < 9; ++j )
    {
      float theta1 = static_cast< float > ( ( i / 10 ) * 360 );
      float theta2 = static_cast< float > ( ( j / 10 ) * 360 );

      float stepValue = static_cast< float > ( ( 1.0f / 10.0f ) * 360.0f );
      
      //-----------------------------------------------------------------------------------------------------------------------------------------------------

      // Vertex 1 
      osg::Vec3 v1 ( 0.0, 0.0, 0.0 );
      v1.x() = static_cast< float > ( ( 1.0f * sin( ( Usul::Math::PIE / 180 ) * theta1 ) * cos( ( Usul::Math::PIE / 180 ) * theta2 ) ) );
	    v1.y() = static_cast< float > ( ( 1.0f * sin( ( Usul::Math::PIE / 180 ) * theta1 ) * sin( ( Usul::Math::PIE / 180 ) * theta2 ) ) );
	    v1.z() = static_cast< float > ( ( 1.0f * cos( ( Usul::Math::PIE / 180 ) * theta1 ) ) );	

      osg::Vec3 n1 = v1;
      n1.normalize();

      osg::Vec2 tex1 ( 0.0, 0.0 );
      tex1.x() = static_cast< float > ( i / 10 );
      tex1.y() = static_cast< float > ( j / 10 );

      //-----------------------------------------------------------------------------------------------------------------------------------------------------

      // Vertex 2
      osg::Vec3 v2 ( 0.0, 0.0, 0.0 );
      float th1 = static_cast< float > ( theta1 + stepValue ); 
      v2.x() = static_cast< float > ( ( 1.0f * sin( ( Usul::Math::PIE / 180 ) * th1 ) * cos( ( Usul::Math::PIE / 180 ) * theta2 ) ) );
	    v2.y() = static_cast< float > ( ( 1.0f * sin( ( Usul::Math::PIE / 180 ) * th1 ) * sin( ( Usul::Math::PIE / 180 ) * theta2 ) ) );
	    v2.z() = static_cast< float > ( ( 1.0f * cos( ( Usul::Math::PIE / 180 ) * th1 ) ) );	

      osg::Vec3 n2 = v2;
      n2.normalize();

      osg::Vec2 tex2 ( 0.0, 0.0 );
      tex2.x() = static_cast< float > ( ( i + 1 ) / 10 );
      tex2.y() = static_cast< float > ( j / 10 );

      //-----------------------------------------------------------------------------------------------------------------------------------------------------

      // Vertex 3
      osg::Vec3 v3 ( 0.0, 0.0, 0.0 );
      float th2 = static_cast< float > ( theta2 + stepValue );
      v3.x() = static_cast< float > ( ( 1.0f * sin( ( Usul::Math::PIE / 180 ) * theta1 ) * cos( ( Usul::Math::PIE / 180 ) * th2 ) ) );
	    v3.y() = static_cast< float > ( ( 1.0f * sin( ( Usul::Math::PIE / 180 ) * theta1 ) * sin( ( Usul::Math::PIE / 180 ) * th2 ) ) );
	    v3.z() = static_cast< float > ( ( 1.0f * cos( ( Usul::Math::PIE / 180 ) * theta1 ) ) );	

      osg::Vec3 n3 = v3;
      n3.normalize();

      osg::Vec2 tex3 ( 0.0, 0.0 );
      tex3.x() = static_cast< float > ( i / 10 );
      tex3.y() = static_cast< float > ( ( j + 1 ) / 10 );

      //-----------------------------------------------------------------------------------------------------------------------------------------------------

      osg::Vec4 tv1 ( 0.0, 0.0, 0.0, 0.0 );
      osg::Vec4 tv2 ( 0.0, 0.0, 0.0, 0.0 );

      float tangentTheta = 0;

      if( i == 0 )
      {
        tangentTheta = static_cast< float > ( 360.0 - stepValue );
      }
      else
      {
        tangentTheta = static_cast< float > ( theta1 - stepValue );
      }

      tv1.x() = static_cast< float > ( ( 1.0f * sin( ( Usul::Math::PIE / 180 ) * tangentTheta ) * cos( ( Usul::Math::PIE / 180 ) * theta2 ) ) );
	    tv1.y() = static_cast< float > ( ( 1.0f * sin( ( Usul::Math::PIE / 180 ) * tangentTheta ) * sin( ( Usul::Math::PIE / 180 ) * theta2 ) ) );
	    tv1.z() = static_cast< float > ( ( 1.0f * cos( ( Usul::Math::PIE / 180 ) * tangentTheta ) ) );

      tangentTheta = static_cast< float > ( theta1 + stepValue );
      tv2.x() = static_cast< float > ( ( 1.0f * sin( ( Usul::Math::PIE / 180 ) * tangentTheta ) * cos( ( Usul::Math::PIE / 180 ) * theta2 ) ) );
	    tv2.y() = static_cast< float > ( ( 1.0f * sin( ( Usul::Math::PIE / 180 ) * tangentTheta ) * sin( ( Usul::Math::PIE / 180 ) * theta2 ) ) );
	    tv2.z() = static_cast< float > ( ( 1.0f * cos( ( Usul::Math::PIE / 180 ) * tangentTheta ) ) );

      vertices->push_back( v1 ); vertices->push_back( v2 ); vertices->push_back( v3 );   
      normals->push_back( n1 ); normals->push_back( n2 ); normals->push_back( n3 );
      texCoords->push_back( tex1 ); texCoords->push_back( tex2 ); texCoords->push_back( tex3 );
      tangents->push_back( tv1 - tv2 ); tangents->push_back( tv1 - tv2 ); tangents->push_back( tv1 - tv2 );
      
    }
  }
  // Set Vertex array
  geometry->setVertexArray( vertices.get() );

  // Set Normal array and binding
  geometry->setNormalArray( normals.get() );
  geometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

  // Set color array and binding
  geometry->setColorArray( tangents.get() );
  geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

  // set tex coord array
  geometry->setTexCoordArray( 0, texCoords.get() );

  geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLES, 0, vertices->size() ) );
  geode->addDrawable( geometry.get() );

#endif
  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

osg::Geode* GeneralShaderDocument::_createCubeModel()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  GeodePtr geode( new osg::Geode );
#if 1
  osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );
  
  // Vertices and faceNormals
	osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
	osg::ref_ptr< osg::Vec3Array > faceNormals ( new osg::Vec3Array );
	osg::ref_ptr< osg::Vec3Array > vertexNormals ( new osg::Vec3Array );
  osg::ref_ptr< osg::Vec3Array > normals ( new osg::Vec3Array );
  
  // Vertices of the cube
  osg::Vec3d p0( -0.5, -0.5, -0.5 );
	osg::Vec3d p1( 0.5, -0.5, -0.5 );
	osg::Vec3d p2( -0.5, 0.5, -0.5 );
	osg::Vec3d p3( 0.5, 0.5, -0.5 );
	osg::Vec3d p4( -0.5, -0.5, 0.5 );
	osg::Vec3d p5( 0.5, -0.5, 0.5 );
	osg::Vec3d p6( -0.5, 0.5, 0.5 );
	osg::Vec3d p7( 0.5, 0.5, 0.5 );

  // N0
  vertices->push_back( p0 ); vertices->push_back( p3 ); vertices->push_back( p1 );
	faceNormals->push_back( ( p1 - p3 ) ^ ( p0 - p3 ) );

  //N1
	vertices->push_back( p0 ); vertices->push_back( p2 ); vertices->push_back( p3 );
	faceNormals->push_back( ( p3 - p2 ) ^ ( p0 - p2 ) );

  //N2
	vertices->push_back( p0 ); vertices->push_back( p6 ); vertices->push_back( p2 );
	faceNormals->push_back( ( p2 - p6 ) ^ ( p0 - p6 ) );

  //N3
	vertices->push_back( p0 ); vertices->push_back( p4 ); vertices->push_back( p6 );
	faceNormals->push_back( ( p6 - p4 ) ^ ( p0 - p4 ) );

  //N4
	vertices->push_back( p0 ); vertices->push_back( p1 ); vertices->push_back( p5 );
	faceNormals->push_back( ( p5 - p1 ) ^ ( p0 - p1 ) );

  //N5
	vertices->push_back( p0 ); vertices->push_back( p5 ); vertices->push_back( p4 );
	faceNormals->push_back( ( p4 - p5 ) ^ ( p0 - p5 ) );

  //N6
	vertices->push_back( p7 ); vertices->push_back( p3 ); vertices->push_back( p2 );
	faceNormals->push_back( ( p2 - p3 ) ^ ( p7 - p3 ) );

  //N7
	vertices->push_back( p7 ); vertices->push_back( p2 ); vertices->push_back( p6 );
	faceNormals->push_back( ( p6 - p2 ) ^ ( p7 - p2 ) );

  //N8
	vertices->push_back( p7 ); vertices->push_back( p6 ); vertices->push_back( p4 );
	faceNormals->push_back( ( p4 - p6 ) ^ ( p7 - p6 ) );

  //N9
	vertices->push_back( p7 ); vertices->push_back( p4 ); vertices->push_back( p5 );
	faceNormals->push_back( ( p5 - p4 ) ^ ( p7 - p4 ) );

  //N10
	vertices->push_back( p7 ); vertices->push_back( p5 ); vertices->push_back( p1 );
	faceNormals->push_back( ( p1 - p5 ) ^ ( p7 - p5 ) );

  //N11
	vertices->push_back( p7 ); vertices->push_back( p1 ); vertices->push_back( p3 );
	faceNormals->push_back( ( p3 - p1 ) ^ ( p7 - p1 ) );
  
#if 1
  // normal and tex coords for v0 is fn[2], fn[1], fn[4] 
  vertexNormals->push_back( ( faceNormals->at( 4 ) - faceNormals->at( 1 ) ) ^ ( faceNormals->at( 2 ) - faceNormals->at( 1 ) ) );
  // normal and tex coords for v1 is fn[0], fn[11], fn[4]
  vertexNormals->push_back( ( faceNormals->at( 4 ) - faceNormals->at( 11 ) ) ^ ( faceNormals->at( 0 ) - faceNormals->at( 11 ) ) );
  // normal and tex coords for v2 is fn[6], fn[1], fn[2]
  vertexNormals->push_back( ( faceNormals->at( 2 ) - faceNormals->at( 1 ) ) ^ ( faceNormals->at( 6 ) - faceNormals->at( 1 ) ) );
  // normal and tex coords for v3 is fn[6], fn[11], fn[0]
  vertexNormals->push_back( ( faceNormals->at( 0 ) - faceNormals->at( 11 ) ) ^ ( faceNormals->at( 6 ) - faceNormals->at( 11 ) ) );
  // normal and tex coords for v4 is fn[8], fn[3], fn[5]
  vertexNormals->push_back( ( faceNormals->at( 5 ) - faceNormals->at( 3 ) ) ^ ( faceNormals->at( 8 ) - faceNormals->at( 3 ) ) );
  // normal and tex coords for v5 is fn[10], fn[9], fn[5]
  vertexNormals->push_back( ( faceNormals->at( 5 ) - faceNormals->at( 9 ) ) ^ ( faceNormals->at( 10 ) - faceNormals->at( 9 ) ) );
  // normal and tex coords for v6 is fn[7], fn[2], fn[8]
  vertexNormals->push_back( ( faceNormals->at( 8 ) - faceNormals->at( 2 ) ) ^ ( faceNormals->at( 7 ) - faceNormals->at( 2 ) ) );
  // normal and tex coords for v7 is fn[7], fn[8], fn[10]
  vertexNormals->push_back( ( faceNormals->at( 10 ) - faceNormals->at( 8 ) ) ^ ( faceNormals->at( 7 ) - faceNormals->at( 8 ) ) );
#endif
  // texture coordinate array
  osg::ref_ptr< osg::Vec2Array > tex ( new osg::Vec2Array );
  osg::Vec2f t00 ( 0.0f, 0.0f );
  osg::Vec2f t01 ( 0.0f, 1.0f );
  osg::Vec2f t10 ( 1.0f, 0.0f );
  osg::Vec2f t11 ( 1.0f, 1.0f );

  // Tangent vector attributes
  osg::ref_ptr< osg::Vec4Array > tangent ( new osg::Vec4Array );

//--------------------------------------------------------------------------
  // +Z face
  
  tex->push_back( t10 ); tex->push_back( t01 ); tex->push_back( t00 );
#if 0
  normals->push_back( vertexNormals->at( 0 ) );normals->push_back( vertexNormals->at( 3 ) );normals->push_back( vertexNormals->at( 1 ) );
  tangent->push_back( p0 - p3 ); tangent->push_back( p3 - p1 ); tangent->push_back( p1 - p0 );
#else
#if 0
  normals->push_back( vertexNormals->at( 0 ) );normals->push_back( vertexNormals->at( 3 ) );normals->push_back( vertexNormals->at( 1 ) );
#else
  normals->push_back( osg::Vec3( 0.0, 0.0, 1.0 ) );
  normals->push_back( osg::Vec3( 0.0, 0.0, 1.0 ) );
  normals->push_back( osg::Vec3( 0.0, 0.0, 1.0 ) );
#endif

  tangent->push_back( osg::Vec4( 0.0, 0.5, 0.5, 1.0 ) );
  tangent->push_back( osg::Vec4( 0.0, 0.5, 0.5, 1.0 ) );
  tangent->push_back( osg::Vec4( 0.0, 0.5, 0.5, 1.0 ) );
#endif

  
  tex->push_back( t10 ); tex->push_back( t11 ); tex->push_back( t01 ); 
#if 0
  normals->push_back( vertexNormals->at( 0 ) );normals->push_back( vertexNormals->at( 2 ) );normals->push_back( vertexNormals->at( 3 ) );
  tangent->push_back( p0 - p2 ); tangent->push_back( p2 - p3 ); tangent->push_back( p3 - p0 );
#else
#if 0
  normals->push_back( vertexNormals->at( 0 ) );normals->push_back( vertexNormals->at( 2 ) );normals->push_back( vertexNormals->at( 3 ) );
#else
  normals->push_back( osg::Vec3( 0.0, 0.0, 1.0 ) );
  normals->push_back( osg::Vec3( 0.0, 0.0, 1.0 ) );
  normals->push_back( osg::Vec3( 0.0, 0.0, 1.0 ) );
#endif
  tangent->push_back( osg::Vec4( 0.0, 0.5, 0.5, 1.0 ) );
  tangent->push_back( osg::Vec4( 0.0, 0.5, 0.5, 1.0 ) );
  tangent->push_back( osg::Vec4( 0.0, 0.5, 0.5, 1.0 ) );
#endif

  //--------------------------------------------------------------------------

  // -X Face
  
  tex->push_back( t00 ); tex->push_back( t11 ); tex->push_back( t01 );

#if 0
  tangent->push_back( p0 - p6 ); tangent->push_back( p6 - p2 ); tangent->push_back( p2 - p0 );
#else
#if 0
  normals->push_back( vertexNormals->at( 0 ) );normals->push_back( vertexNormals->at( 6 ) );normals->push_back( vertexNormals->at( 2 ) );
#else
  normals->push_back( osg::Vec3( -1.0, 0.0, 0.0 ) );
  normals->push_back( osg::Vec3( -1.0, 0.0, 0.0 ) );
  normals->push_back( osg::Vec3( -1.0, 0.0, 0.0 ) );
#endif
  tangent->push_back( osg::Vec4( 0.5, 0.5, 0.0, 1.0 ) );
  tangent->push_back( osg::Vec4( 0.5, 0.5, 0.0, 1.0 ) );
  tangent->push_back( osg::Vec4( 0.5, 0.5, 0.0, 1.0 ) );
#endif

  
  tex->push_back( t00 ); tex->push_back( t10 ); tex->push_back( t11 );

#if 0
  tangent->push_back( p0 - p4 ); tangent->push_back( p4 - p6 ); tangent->push_back( p6 - p0 );
#else
#if 0
  normals->push_back( vertexNormals->at( 0 ) );normals->push_back( vertexNormals->at( 4 ) );normals->push_back( vertexNormals->at( 6 ) );
#else
  normals->push_back( osg::Vec3( -1.0, 0.0, 0.0 ) );
  normals->push_back( osg::Vec3( -1.0, 0.0, 0.0 ) );
  normals->push_back( osg::Vec3( -1.0, 0.0, 0.0 ) );
#endif
  tangent->push_back( osg::Vec4( 0.5, 0.5, 0.0, 1.0 ) );
  tangent->push_back( osg::Vec4( 0.5, 0.5, 0.0, 1.0 ) );
  tangent->push_back( osg::Vec4( 0.5, 0.5, 0.0, 1.0 ) );
#endif

  //--------------------------------------------------------------------------

  // -Y Face
  
  tex->push_back( t00 ); tex->push_back( t10 ); tex->push_back( t11 );

#if 0
  tangent->push_back( p0 - p1 ); tangent->push_back( p1 - p5 ); tangent->push_back( p5 - p0 );
#else
#if 0
  normals->push_back( vertexNormals->at( 0 ) );normals->push_back( vertexNormals->at( 1 ) );normals->push_back( vertexNormals->at( 5 ) );
#else
  normals->push_back( osg::Vec3( 0.0, -1.0, 0.0 ) );
  normals->push_back( osg::Vec3( 0.0, -1.0, 0.0 ) );
  normals->push_back( osg::Vec3( 0.0, -1.0, 0.0 ) );
#endif
  tangent->push_back( osg::Vec4( 1.0, 0.5, 0.5, 1.0 ) );
  tangent->push_back( osg::Vec4( 1.0, 0.5, 0.5, 1.0 ) );
  tangent->push_back( osg::Vec4( 1.0, 0.5, 0.5, 1.0 ) );
#endif

  
  tex->push_back( t00 ); tex->push_back( t11 ); tex->push_back( t01 );

#if 0
  tangent->push_back( p0 - p5 ); tangent->push_back( p5 - p4 ); tangent->push_back( p4 - p0 );
#else
#if 0
  normals->push_back( vertexNormals->at( 0 ) );normals->push_back( vertexNormals->at( 5 ) );normals->push_back( vertexNormals->at( 4 ) );
#else
  normals->push_back( osg::Vec3( 0.0, -1.0, 0.0 ) );
  normals->push_back( osg::Vec3( 0.0, -1.0, 0.0 ) );
  normals->push_back( osg::Vec3( 0.0, -1.0, 0.0 ) );
#endif
  tangent->push_back( osg::Vec4( 1.0, 0.5, 0.5, 1.0 ) );
  tangent->push_back( osg::Vec4( 1.0, 0.5, 0.5, 1.0 ) );
  tangent->push_back( osg::Vec4( 1.0, 0.5, 0.5, 1.0 ) );
#endif

  //--------------------------------------------------------------------------

  // +Y Face
  
  tex->push_back( t10 ); tex->push_back( t11 ); tex->push_back( t01 );

#if 0
  tangent->push_back( p7 - p3 ); tangent->push_back( p3 - p2 ); tangent->push_back( p2 - p7 );
#else

#if 0
  normals->push_back( vertexNormals->at( 7 ) );normals->push_back( vertexNormals->at( 3 ) );normals->push_back( vertexNormals->at( 2 ) );
#else
  normals->push_back( osg::Vec3( 0.0, 1.0, 0.0 ) );
  normals->push_back( osg::Vec3( 0.0, 1.0, 0.0 ) );
  normals->push_back( osg::Vec3( 0.0, 1.0, 0.0 ) );
#endif

  tangent->push_back( osg::Vec4( 1.0, 0.5, 0.5, 1.0 ) );
  tangent->push_back( osg::Vec4( 1.0, 0.5, 0.5, 1.0 ) );
  tangent->push_back( osg::Vec4( 1.0, 0.5, 0.5, 1.0 ) );
#endif

  
  tex->push_back( t10 ); tex->push_back( t01 ); tex->push_back( t00 );

#if 0
  tangent->push_back( p7 - p2 ); tangent->push_back( p2 - p6 ); tangent->push_back( p6 - p7 );
#else

#if 0
  normals->push_back( vertexNormals->at( 7 ) );normals->push_back( vertexNormals->at( 2 ) );normals->push_back( vertexNormals->at( 6 ) );
#else
  normals->push_back( osg::Vec3( 0.0, 1.0, 0.0 ) );
  normals->push_back( osg::Vec3( 0.0, 1.0, 0.0 ) );
  normals->push_back( osg::Vec3( 0.0, 1.0, 0.0 ) );
#endif

  tangent->push_back( osg::Vec4( 1.0, 0.5, 0.5, 1.0 ) );
  tangent->push_back( osg::Vec4( 1.0, 0.5, 0.5, 1.0 ) );
  tangent->push_back( osg::Vec4( 1.0, 0.5, 0.5, 1.0 ) );
#endif
  
  //--------------------------------------------------------------------------

  // -Z Face
  
  tex->push_back( t11 ); tex->push_back( t01 ); tex->push_back( t00 );

#if 0
  tangent->push_back( p7 - p6 ); tangent->push_back( p6 - p4 ); tangent->push_back( p4 - p7 );
#else
#if 0
  normals->push_back( vertexNormals->at( 7 ) );normals->push_back( vertexNormals->at( 6 ) );normals->push_back( vertexNormals->at( 4 ) );
#else
  normals->push_back( osg::Vec3( 0.0, 0.0, -1.0 ) );
  normals->push_back( osg::Vec3( 0.0, 0.0, -1.0 ) );
  normals->push_back( osg::Vec3( 0.0, 0.0, -1.0 ) );
#endif
  tangent->push_back( osg::Vec4( 1.0, 0.5, 0.5, 1.0 ) );
  tangent->push_back( osg::Vec4( 1.0, 0.5, 0.5, 1.0 ) );
  tangent->push_back( osg::Vec4( 1.0, 0.5, 0.5, 1.0 ) );
#endif

  
  tex->push_back( t11 ); tex->push_back( t00 ); tex->push_back( t10 );

#if 0
  tangent->push_back( p7 - p4 ); tangent->push_back( p4 - p5 ); tangent->push_back( p5 - p7 );
#else

#if 0
  normals->push_back( vertexNormals->at( 7 ) );normals->push_back( vertexNormals->at( 4 ) );normals->push_back( vertexNormals->at( 5 ) );
#else
  normals->push_back( osg::Vec3( 0.0, 0.0, -1.0 ) );
  normals->push_back( osg::Vec3( 0.0, 0.0, -1.0 ) );
  normals->push_back( osg::Vec3( 0.0, 0.0, -1.0 ) );
#endif

  tangent->push_back( osg::Vec4( 1.0, 0.5, 0.5, 1.0 ) );
  tangent->push_back( osg::Vec4( 1.0, 0.5, 0.5, 1.0 ) );
  tangent->push_back( osg::Vec4( 1.0, 0.5, 0.5, 1.0 ) );
#endif

  //--------------------------------------------------------------------------

  // +x Face
  
  tex->push_back( t01 ); tex->push_back( t00 ); tex->push_back( t10 );

#if 0
  tangent->push_back( p7 - p5 ); tangent->push_back( p5 - p1 ); tangent->push_back( p1 - p7 );
#else

#if 0
  normals->push_back( vertexNormals->at( 7 ) );normals->push_back( vertexNormals->at( 5 ) );normals->push_back( vertexNormals->at( 1 ) );
#else
  normals->push_back( osg::Vec3( 1.0, 0.0, 0.0 ) );
  normals->push_back( osg::Vec3( 1.0, 0.0, 0.0 ) );
  normals->push_back( osg::Vec3( 1.0, 0.0, 0.0 ) );
#endif

  tangent->push_back( osg::Vec4( 0.5, 0.5, 1.0, 1.0 ) );
  tangent->push_back( osg::Vec4( 0.5, 0.5, 1.0, 1.0 ) );
  tangent->push_back( osg::Vec4( 0.5, 0.5, 1.0, 1.0 ) );
#endif

  tex->push_back( t01 ); tex->push_back( t10 ); tex->push_back( t11 );

#if 0
  tangent->push_back( p7 - p1 ); tangent->push_back( p1 - p3 ); tangent->push_back( p3 - p7 );
#else

#if 0
  normals->push_back( vertexNormals->at( 7 ) );normals->push_back( vertexNormals->at( 1 ) );normals->push_back( vertexNormals->at( 3 ) );
#else
  normals->push_back( osg::Vec3( 1.0, 0.0, 0.0 ) );
  normals->push_back( osg::Vec3( 1.0, 0.0, 0.0 ) );
  normals->push_back( osg::Vec3( 1.0, 0.0, 0.0 ) );
#endif

  tangent->push_back( osg::Vec4( 0.5, 0.5, 1.0, 1.0 ) );
  tangent->push_back( osg::Vec4( 0.5, 0.5, 1.0, 1.0 ) );
  tangent->push_back( osg::Vec4( 0.5, 0.5, 1.0, 1.0 ) );
#endif

  //--------------------------------------------------------------------------

  /*for( unsigned int i = 0; i < tangent->size(); ++i )
  {
    tangent->at( i ).normalize();
  }*/

	geometry->setVertexArray( vertices.get() );
  geometry->setTexCoordArray( 0, tex.get() );
  //geometry->setVertexAttribArray( 0, tangent.get() );
  //geometry->setVertexAttribBinding( 0, osg::Geometry::BIND_PER_VERTEX );
  geometry->setColorArray( tangent.get() );
  geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
  
	geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLES, 0, vertices->size() ) );

#if 1
	geometry->setNormalArray( faceNormals.get() );
  geometry->setNormalBinding( osg::Geometry::BIND_PER_PRIMITIVE );
#else
	geometry->setNormalArray( normals.get() );
  geometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

#endif
  geode->addDrawable( geometry.get() );
#else
  osg::ref_ptr< osg::ShapeDrawable > box ( new osg::ShapeDrawable( new osg::Box( osg::Vec3(0.0f,0.0f,0.0f),1.0f ) ) ); 
  osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );

  geometry = dynamic_cast< osg::Geometry* >( box.get() );
  geode->addDrawable( new osg::ShapeDrawable( new osg::Box( osg::Vec3(0.0f,0.0f,0.0f),1.0f ) ) );
 
#endif
  return geode.release();
}

///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////
//
//void GeneralShaderDocument::_createSkyBox( unsigned int index )
//{
//   
//    //osg::StateSet* stateset = new osg::StateSet();
//    osg::ref_ptr< osg::StateSet > stateset ( new osg::StateSet );
//
//    //osg::TexEnv* te = new osg::TexEnv;
//    osg::ref_ptr< osg::TexEnv > te ( new osg::TexEnv );
//    te->setMode(osg::TexEnv::REPLACE);
//    stateset->setTextureAttributeAndModes( 0, te.get(), osg::StateAttribute::ON );
//
//    //osg::TexGen *tg = new osg::TexGen;
//    osg::ref_ptr< osg::TexGen > tg ( new osg::TexGen );
//    tg->setMode( osg::TexGen::NORMAL_MAP );
//    stateset->setTextureAttributeAndModes( 0, tg.get(), osg::StateAttribute::ON );
//
//    //osg::TexMat *tm = new osg::TexMat;
//    osg::ref_ptr< osg::TexMat > tm ( new osg::TexMat );
//    stateset->setTextureAttribute( 0, tm.get() );
//
//    osg::ref_ptr< osg::TextureCubeMap > skymap = _shaderGroups.at( index ).emMap; 
//    stateset->setTextureAttributeAndModes( 0, skymap.get(), osg::StateAttribute::ON );
//
//    stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
//    stateset->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );
//
//    // clear the depth to the far plane.
//    //osg::Depth* depth = new osg::Depth;
//    osg::ref_ptr< osg::Depth > depth ( new osg::Depth );
//    depth->setFunction( osg::Depth::ALWAYS );
//    depth->setRange( 1.0, 1.0 );   
//    stateset->setAttributeAndModes( depth.get(), osg::StateAttribute::ON );
//
//    stateset->setRenderBinDetails( -1, "RenderBin" );
//
//    osg::ref_ptr< osg::Drawable > drawable = new osg::ShapeDrawable ( new osg::Sphere( osg::Vec3( 0.0f,0.0f,0.0f), 10 ) );
//    //osg::Drawable* drawable = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),1));
//
//    //osg::Geode* geode = new osg::Geode;
//    GeodePtr geode ( new osg::Geode );
//    geode->setCullingActive( false );
//    geode->setStateSet( stateset.get() );
//    geode->addDrawable( drawable.get() );
//
//
//    //osg::Transform* transform = new MoveEarthySkyWithEyePointTransform;
//    osg::ref_ptr< osg::Transform > transform = new MoveEarthySkyWithEyePointTransform;
//    transform->setCullingActive( false );
//    transform->addChild( geode.get() );
//
//   // osg::ClearNode* clearNode = new osg::ClearNode;
//    osg::ref_ptr< osg::ClearNode > clearNode ( new osg::ClearNode );
////  clearNode->setRequiresClear(false);
//    clearNode->setCullCallback( new TexMatCallback( *tm.get() ) );
//    //clearNode->setStateSet( stateset.get() );
//    clearNode->addChild( transform.get() );
//
//    _shaderGroups.at( index ).skyBoxIndex = _skyBoxes.size();
//    _skyBoxes.push_back( clearNode.get() );
//    //return clearNode.release();
//}

void GeneralShaderDocument::_createSkyBox( unsigned int index )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  Usul::System::Directory::ScopedCwd cwd ( _workingDir );

  // Vertices and faceNormals
	osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
 
	// set the vertices of the cube
  osg::Vec3d p0( -0.5, -0.5, -0.5 );
	osg::Vec3d p1( 0.5, -0.5, -0.5 );
	osg::Vec3d p2( -0.5, 0.5, -0.5 );
	osg::Vec3d p3( 0.5, 0.5, -0.5 );
	osg::Vec3d p4( -0.5, -0.5, 0.5 );
	osg::Vec3d p5( 0.5, -0.5, 0.5 );
	osg::Vec3d p6( -0.5, 0.5, 0.5 );
	osg::Vec3d p7( 0.5, 0.5, 0.5 );

  double scale = 300.0;
  p0 *= scale;
  p1 *= scale;
  p2 *= scale;
  p3 *= scale;
  p4 *= scale;
  p5 *= scale;
  p6 *= scale;
  p7 *= scale;

  // texture coordinate array
  osg::ref_ptr< osg::Vec2Array > tex ( new osg::Vec2Array );
  osg::Vec2f t00 ( 0.0f, 0.0f );
  osg::Vec2f t01 ( 0.0f, 1.0f );
  osg::Vec2f t10 ( 1.0f, 0.0f );
  osg::Vec2f t11 ( 1.0f, 1.0f );

  // Neg Z
  vertices->push_back( p0 ); vertices->push_back( p3 ); vertices->push_back( p1 );
	vertices->push_back( p0 ); vertices->push_back( p2 ); vertices->push_back( p3 );
#if 0
  tex->push_back( t00 ); tex->push_back( t11 ); tex->push_back( t10 );
  tex->push_back( t00 ); tex->push_back( t01 ); tex->push_back( t11 ); 
#else
  tex->push_back( t11 ); tex->push_back( t00 ); tex->push_back( t01 );
  tex->push_back( t11 ); tex->push_back( t10 ); tex->push_back( t00 ); 
#endif

  //Neg X
	vertices->push_back( p0 ); vertices->push_back( p6 ); vertices->push_back( p2 );
	vertices->push_back( p0 ); vertices->push_back( p4 ); vertices->push_back( p6 );
#if 0
  tex->push_back( t10 ); tex->push_back( t01 ); tex->push_back( t11 );
  tex->push_back( t10 ); tex->push_back( t00 ); tex->push_back( t01 );
#else
  tex->push_back( t01 ); tex->push_back( t10 ); tex->push_back( t00 );
  tex->push_back( t01 ); tex->push_back( t11 ); tex->push_back( t10 );
#endif

  //Neg Y
	vertices->push_back( p0 ); vertices->push_back( p1 ); vertices->push_back( p5 );
	vertices->push_back( p0 ); vertices->push_back( p5 ); vertices->push_back( p4 );
#if 0
  tex->push_back( t10 ); tex->push_back( t00 ); tex->push_back( t01 );
  tex->push_back( t10 ); tex->push_back( t01 ); tex->push_back( t11 );
#else
  tex->push_back( t01 ); tex->push_back( t11 ); tex->push_back( t10 );
  tex->push_back( t01 ); tex->push_back( t10 ); tex->push_back( t00 );
#endif

  //Pos Y
	vertices->push_back( p7 ); vertices->push_back( p3 ); vertices->push_back( p2 );
	vertices->push_back( p7 ); vertices->push_back( p2 ); vertices->push_back( p6 );
#if 0
  tex->push_back( t00 ); tex->push_back( t01 ); tex->push_back( t11 );
  tex->push_back( t00 ); tex->push_back( t11 ); tex->push_back( t10 );
#else
  tex->push_back( t11 ); tex->push_back( t10 ); tex->push_back( t00 );
  tex->push_back( t11 ); tex->push_back( t00 ); tex->push_back( t01 );
#endif

  //Pos Z
	vertices->push_back( p7 ); vertices->push_back( p6 ); vertices->push_back( p4 );
	vertices->push_back( p7 ); vertices->push_back( p4 ); vertices->push_back( p5 );
#if 0
  tex->push_back( t01 ); tex->push_back( t11 ); tex->push_back( t10 );
  tex->push_back( t01 ); tex->push_back( t10 ); tex->push_back( t00 );
#else
  tex->push_back( t10 ); tex->push_back( t00 ); tex->push_back( t01 );
  tex->push_back( t10 ); tex->push_back( t01 ); tex->push_back( t11 );
#endif

  //Pos X
	vertices->push_back( p7 ); vertices->push_back( p5 ); vertices->push_back( p1 );
	vertices->push_back( p7 ); vertices->push_back( p1 ); vertices->push_back( p3 );
#if 0
  tex->push_back( t11 ); tex->push_back( t10 ); tex->push_back( t00 );
  tex->push_back( t11 ); tex->push_back( t00 ); tex->push_back( t01 );
#else
  tex->push_back( t00 ); tex->push_back( t01 ); tex->push_back( t11 );
  tex->push_back( t00 ); tex->push_back( t11 ); tex->push_back( t10 );
#endif

  //---------------------------------------------------------------------------------------------------

  // create PosX face of the skybox
  osg::ref_ptr< osg::Vec3Array > Vposx ( new osg::Vec3Array );
  osg::ref_ptr< osg::Vec2Array > Tposx ( new osg::Vec2Array );
  osg::ref_ptr< osg::Geometry > Gposx ( new osg::Geometry );
  osg::ref_ptr< osg::StateSet > SSposx = Gposx->getOrCreateStateSet();
  for( unsigned int i = 30; i < 36; ++i )
  {
    Vposx->push_back( vertices->at ( i ) );
    Tposx->push_back( tex->at ( i ) );
  }
  Gposx->setVertexArray( Vposx.get() );
  Gposx->setTexCoordArray( 0, Tposx.get() );
  ImagePtr Iposx = osgDB::readImageFile( _shaderGroups.at( index ).cubeList.at( 0 ) );
  if( true == Iposx.valid() )
  {
    osg::ref_ptr< osg::Texture2D > tex ( new osg::Texture2D );
    tex->setImage( Iposx.get() );

    SSposx->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    SSposx->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );
    tex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    tex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    tex->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
    tex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    tex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);  
    SSposx->setTextureAttributeAndModes ( 0, tex.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
  }
  Gposx->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLES, 0, Vposx->size() ) );

   //---------------------------------------------------------------------------------------------------

  // create PosY face of the skybox
  osg::ref_ptr< osg::Vec3Array > Vposy ( new osg::Vec3Array );
  osg::ref_ptr< osg::Vec2Array > Tposy ( new osg::Vec2Array );
  osg::ref_ptr< osg::Geometry > Gposy ( new osg::Geometry );
  osg::ref_ptr< osg::StateSet > SSposy = Gposy->getOrCreateStateSet();
  for( unsigned int i = 18; i < 24; ++i )
  {
    Vposy->push_back( vertices->at ( i ) );
    Tposy->push_back( tex->at ( i ) );
  }
  Gposy->setVertexArray( Vposy.get() );
  Gposy->setTexCoordArray( 0, Tposy.get() );
  ImagePtr Iposy = osgDB::readImageFile( _shaderGroups.at( index ).cubeList.at( 2 ) );
  if( true == Iposy.valid() )
  {
    osg::ref_ptr< osg::Texture2D > tex ( new osg::Texture2D );
    tex->setImage( Iposy.get() );

    SSposy->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    SSposy->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );
    tex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    tex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    tex->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
    tex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    tex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    SSposy->setTextureAttributeAndModes ( 0, tex.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
  }
  Gposy->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLES, 0, Vposy->size() ) );

   //---------------------------------------------------------------------------------------------------
  
  // create PosZ face of the skybox
  osg::ref_ptr< osg::Vec3Array > Vposz ( new osg::Vec3Array );
  osg::ref_ptr< osg::Vec2Array > Tposz ( new osg::Vec2Array );
  osg::ref_ptr< osg::Geometry > Gposz ( new osg::Geometry );
  osg::ref_ptr< osg::StateSet > SSposz =  Gposz->getOrCreateStateSet();
  for( unsigned int i = 24; i < 30; ++i )
  {
    Vposz->push_back( vertices->at ( i ) );
    Tposz->push_back( tex->at ( i ) );
  }
  Gposz->setVertexArray( Vposz.get() );
  Gposz->setTexCoordArray( 0, Tposz.get() );
  ImagePtr Iposz = osgDB::readImageFile( _shaderGroups.at( index ).cubeList.at( 4 ) );
  if( true == Iposz.valid() )
  {
    osg::ref_ptr< osg::Texture2D > tex ( new osg::Texture2D );
    tex->setImage( Iposz.get() );

    SSposz->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    SSposz->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );
    tex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    tex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    tex->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
    tex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    tex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    SSposz->setTextureAttributeAndModes ( 0, tex.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
  }
  Gposz->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLES, 0, Vposz->size() ) );

   //---------------------------------------------------------------------------------------------------

  // create NegX face of the skybox
  osg::ref_ptr< osg::Vec3Array > Vnegx ( new osg::Vec3Array );
  osg::ref_ptr< osg::Vec2Array > Tnegx ( new osg::Vec2Array );
  osg::ref_ptr< osg::Geometry > Gnegx ( new osg::Geometry );
  osg::ref_ptr< osg::StateSet > SSnegx =  Gnegx->getOrCreateStateSet();
  for( unsigned int i = 6; i < 12; ++i )
  {
    Vnegx->push_back( vertices->at ( i ) );
    Tnegx->push_back( tex->at ( i ) );
  }
  Gnegx->setVertexArray( Vnegx.get() );
  Gnegx->setTexCoordArray( 0, Tnegx.get() );
  ImagePtr Inegx = osgDB::readImageFile( _shaderGroups.at( index ).cubeList.at( 1 ) );
  if( true == Inegx.valid() )
  {
    osg::ref_ptr< osg::Texture2D > tex ( new osg::Texture2D );
    tex->setImage( Inegx.get() );

    SSnegx->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    SSnegx->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );
    tex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    tex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    tex->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
    tex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    tex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    SSnegx->setTextureAttributeAndModes ( 0, tex.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
  }
  Gnegx->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLES, 0, Vnegx->size() ) );

   //---------------------------------------------------------------------------------------------------
 
  // create NegY face of the skybox
  osg::ref_ptr< osg::Vec3Array > Vnegy ( new osg::Vec3Array );
  osg::ref_ptr< osg::Vec2Array > Tnegy ( new osg::Vec2Array );
  osg::ref_ptr< osg::Geometry > Gnegy ( new osg::Geometry );
  osg::ref_ptr< osg::StateSet > SSnegy =  Gnegy->getOrCreateStateSet();
  for( unsigned int i = 12; i < 18; ++i )
  {
    Vnegy->push_back( vertices->at ( i ) );
    Tnegy->push_back( tex->at ( i ) );
  }
  Gnegy->setVertexArray( Vnegy.get() );
  Gnegy->setTexCoordArray( 0, Tnegy.get() );
  ImagePtr Inegy = osgDB::readImageFile( _shaderGroups.at( index ).cubeList.at( 3 ) );
  if( true == Inegy.valid() )
  {
    osg::ref_ptr< osg::Texture2D > tex ( new osg::Texture2D );
    tex->setImage( Inegy.get() );

    SSnegy->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    SSnegy->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );
    tex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    tex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    tex->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
    tex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    tex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    SSnegy->setTextureAttributeAndModes ( 0, tex.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
  }
  Gnegy->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLES, 0, Vnegy->size() ) );

   //---------------------------------------------------------------------------------------------------

  // create NegY face of the skybox
  osg::ref_ptr< osg::Vec3Array > Vnegz ( new osg::Vec3Array );
  osg::ref_ptr< osg::Vec2Array > Tnegz ( new osg::Vec2Array );
  osg::ref_ptr< osg::Geometry > Gnegz ( new osg::Geometry );
  osg::ref_ptr< osg::StateSet > SSnegz =  Gnegz->getOrCreateStateSet();
  for( unsigned int i = 0; i < 6; ++i )
  {
    Vnegz->push_back( vertices->at ( i ) );
    Tnegz->push_back( tex->at ( i ) );
  }
  Gnegz->setVertexArray( Vnegz.get() );
  Gnegz->setTexCoordArray( 0, Tnegz.get() );
  ImagePtr Inegz = osgDB::readImageFile( _shaderGroups.at( index ).cubeList.at( 5 ) );
  if( true == Inegz.valid() )
  {
    osg::ref_ptr< osg::Texture2D > tex ( new osg::Texture2D );
    tex->setImage( Inegz.get() );

    SSnegz->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    SSnegz->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );
    tex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    tex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    tex->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
    tex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    tex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    SSnegz->setTextureAttributeAndModes ( 0, tex.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
  }
  Gnegz->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLES, 0, Vnegz->size() ) );

   //---------------------------------------------------------------------------------------------------

	GeodePtr geode( new osg::Geode );
  geode->addDrawable( Gposx.get() );
  geode->addDrawable( Gposy.get() );
  geode->addDrawable( Gposz.get() );
  geode->addDrawable( Gnegx.get() );
  geode->addDrawable( Gnegy.get() );
  geode->addDrawable( Gnegz.get() );

  // set the index into the vector of skyboxes for this environment map
  _shaderGroups.at( index ).skyBoxIndex = _skyBoxes.size();
  GroupPtr group ( new osg::Group );
  group->addChild( geode.get() );
  _skyBoxes.push_back( group.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

osg::Geode* GeneralShaderDocument::_createTorusModel()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  GeodePtr geode( new osg::Geode );

  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add to the menu.
//
///////////////////////////////////////////////////////////////////////////////

void GeneralShaderDocument::menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  typedef MenuKit::ToggleButton ToggleButton;
  typedef MenuKit::Button       Button;
  typedef MenuKit::RadioButton  Radio;

  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  // Add the model menu
  {
    for( IndexList::iterator i = _modelList.begin(); i != _modelList.end(); ++i )
    {
      const std::string menuName ( "Models" );
      MenuKit::Menu::RefPtr ModelMenu ( menu.find ( menuName, true ) );
      
      ModelMenu->append ( new Radio ( new SetModelCommand( me.get(), (*i).first ) ) );
     
    
      //ModelMenu->append( ModelSubMenu.get() );
    }
    
  }
  // Add the shaders menu
  {

    MenuKit::Menu::RefPtr simpleMenu ( new MenuKit::Menu ( "Simple", MenuKit::Menu::VERTICAL ) );
    MenuKit::Menu::RefPtr emMenu ( new MenuKit::Menu ( "EnvironmentMap", MenuKit::Menu::VERTICAL ) );
    MenuKit::Menu::RefPtr bmMenu ( new MenuKit::Menu ( "BumpMap", MenuKit::Menu::VERTICAL ) );

    const std::string menuName ( "Shaders" );
    MenuKit::Menu::RefPtr ModelMenu ( menu.find ( menuName, true ) );

    for( IndexList::iterator i = _shaderList.begin(); i != _shaderList.end(); ++i )
    {
  
      if( "Simple" == _shaderGroups.at( (*i).second ).type )
      {
        simpleMenu->append( new Radio ( new SetShaderCommand( me.get(), (*i).first ) ) );
      }
      if( "EnvironmentMap" == _shaderGroups.at( (*i).second ).type )
      {
        emMenu->append( new Radio ( new SetShaderCommand( me.get(), (*i).first ) ) );
      }      
      if( "BumpMap" == _shaderGroups.at( (*i).second ).type )
      {
        bmMenu->append( new Radio ( new SetShaderCommand( me.get(), (*i).first ) ) );
      }
    }
      ModelMenu->append( simpleMenu.get() );
      ModelMenu->append( emMenu.get() );
      ModelMenu->append( bmMenu.get() );
    
  }
  // add the tools menu
  {
    const std::string menuName ( "Tools" );
      MenuKit::Menu::RefPtr ModelMenu ( menu.find ( menuName, true ) );
      
      ModelMenu->append ( new Button ( new ReadSettingsCommand( me.get(), "Reset", _workingDir ) ) );
  }

}

///////////////////////////////////////////////////////////////////////////////
//
// Change the currently displayed model
//
///////////////////////////////////////////////////////////////////////////////


void GeneralShaderDocument::setModel( const std::string &name )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  _currentModel = _modelList[ name ];

  this->_updateScene();
}


///////////////////////////////////////////////////////////////////////////////
//
// Change the currently used shader
//
///////////////////////////////////////////////////////////////////////////////

void GeneralShaderDocument::setShader( const std::string &name )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  _currentShader = _shaderList[ name ];

  this->_updateScene();
}


///////////////////////////////////////////////////////////////////////////////
//
// Change the currently used shader
//
///////////////////////////////////////////////////////////////////////////////

std::string GeneralShaderDocument::getCurrentModel()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  return _modelNames.at( _currentModel );
}


///////////////////////////////////////////////////////////////////////////////
//
// Change the currently used shader
//
///////////////////////////////////////////////////////////////////////////////

std::string GeneralShaderDocument::getCurrentShader()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  return _shaderNames.at( _currentShader );
}


///////////////////////////////////////////////////////////////////////////////
//
// Update the scene
//
///////////////////////////////////////////////////////////////////////////////

void GeneralShaderDocument::_updateScene()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  Usul::System::Directory::ScopedCwd cwd ( _workingDir );

  try
  {
    _scene->removeChildren( 0, _scene->getNumChildren() );

    // get the current shader
    PosAttXformPtr xform ( new osg::PositionAttitudeTransform );
    if( _currentShader < _shaders.size() && _shaders.size() > 0 )
      xform = _shaderGroups.at( _currentShader ).shader;

    // get the current model
    GeodePtr model ( new osg::Geode );
    if( _currentModel < _models.size() && _models.size() > 0 )
      model = _models.at( _currentModel );

    // remove any previous children
    xform->removeChildren( 0, xform->getNumChildren() );
  #if 1
    _modelMatrixTransform->removeChild( 0, _modelMatrixTransform->getNumChildren() );

    // add the model to the matrix transform
    _modelMatrixTransform->addChild( model.get() );
    //_modelMatrixTransform->setCullCallback( new ViewMatrixCallback );

    // add the matrix transform to the shader
    xform->addChild( _modelMatrixTransform.get() );
  #else
    xform->addChild( model.get() );
  #endif
    // Add the model transform matrix Uniform
    osg::ref_ptr< osg::StateSet > ss = xform->getOrCreateStateSet();
    osg::Matrixd inverse = _modelMatrixTransform->getInverseMatrix();
    osg::Matrixd matrix = _modelMatrixTransform->getMatrix();
    Usul::Math::Matrix44f m;
    OsgTools::Convert::matrix ( matrix, m );
    
    ss->addUniform( new osg::Uniform( "modelMatrix", matrix ) );

    _light.x() = static_cast< double > ( ( _radius * sin( ( Usul::Math::PIE / 180 ) * _theta.x() ) * cos( ( Usul::Math::PIE / 180 ) * _theta.y() ) ) );
	  _light.y() = static_cast< double > ( ( _radius * sin( ( Usul::Math::PIE / 180 ) * _theta.x() ) * sin( ( Usul::Math::PIE / 180 ) * _theta.y() ) ) );
	  _light.z() = static_cast< double > ( ( _radius * cos( ( Usul::Math::PIE / 180 ) * _theta.x() ) ) );	

    UniformPtr lightPos = ss->getOrCreateUniform( "lightPosition", osg::Uniform::FLOAT_VEC3 );
    lightPos->set( _light );
    ss->addUniform( lightPos.get() );


    // add the shader/model to the scene group
    _scene->addChild( xform.get() );

    if( true == _shaderGroups.at( _currentShader ).hasSkyBox )
    {
      _scene->addChild( _skyBoxes.at( _shaderGroups.at( _currentShader ).skyBoxIndex ).get() ); 
    }

    // Add additional uniforms read from settings file
    for( unsigned int i = 0; i < _additionalUniforms.size(); ++i )
    {
      UniformPtr uniform = _additionalUniforms.at( i );
      UniformPtr toAdd = ss->getOrCreateUniform( uniform->getName(), uniform->getType() );
      /*toAdd->setName( uniform->getName() );
      toAdd->set( uniform->get( uniform->getType() ) );*/
      std::string type = uniform->getTypename( uniform->getType() );
      toAdd = uniform;
      ss->addUniform( toAdd.get() );
    }
  }
  catch( ... )
  {
    std::cout << "Caught an error in _updateScene()" << std::endl;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Check for key <code> pressed
//
///////////////////////////////////////////////////////////////////////////////

bool GeneralShaderDocument::keyPressed ( int code )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  if( 'j' == code || 'J' == code )
  {
    _modelMatrixTransform->postMult( osg::Matrix::rotate( .1, 1, 0, 0 ) );
    this->_updateScene();
    return true;
  }
  if( 'l' == code || 'L' == code )
  {
    _modelMatrixTransform->postMult( osg::Matrix::rotate( -.1, 1, 0, 0 ) );
    this->_updateScene();
    return true;
  }
  if( 'i' == code || 'I' == code )
  {
    _modelMatrixTransform->postMult( osg::Matrix::rotate( .1, 0, 1, 0 ) );
    this->_updateScene();
    return true;
  }
  if( 'k' == code || 'K' == code )
  {
    _modelMatrixTransform->postMult( osg::Matrix::rotate( -.1, 0, 1, 0 ) );
    this->_updateScene();
    return true;
  }
  if( ',' == code || '<' == code )
  {
    _modelMatrixTransform->postMult( osg::Matrix::rotate( .1, 0, 0, 1 ) );
    this->_updateScene();
    return true;
  }
  if( '.' == code || '>' == code )
  {
    _modelMatrixTransform->postMult( osg::Matrix::rotate( -.1, 0, 0, 1 ) );
    this->_updateScene();
    return true;
  }

  // Light Controls
  if( 'z' == code || 'Z' == code )
  {
    _theta.x() -= 1.0;
    this->_updateScene();
    return true;
  }
  if( 'x' == code || 'X' == code )
  {
    _theta.x() += 1.0;
    this->_updateScene();
    return true;
  }
  if( 'c' == code || 'C' == code )
  {
    _theta.y() -= 1.0;
    this->_updateScene();
    return true;
  }
  if( 'v' == code || 'V' == code )
  {
    _theta.y() += 1.0;
    this->_updateScene();
    return true;
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
// Check for key <code> released
//
///////////////////////////////////////////////////////////////////////////////

bool GeneralShaderDocument::keyReleased ( int code )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );


  
  
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
// Create a normal map texture
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* GeneralShaderDocument::_createNormalMap( const std::string &name, unsigned int index )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  Usul::System::Directory::ScopedCwd cwd ( _workingDir );

  osg::ref_ptr< osg::Image > normalMap ( new osg::Image );
  std::vector< unsigned char > texture;
  
  for( int i = 0; i < _shaderGroups.at( index ).heightMap->s(); i ++ )
  {
    for( int j = 0; j < _shaderGroups.at( index ).heightMap->t(); j++ )
    {
      osg::ref_ptr< osg::Image > hm = _shaderGroups.at( index ).heightMap.get();
      osg::Vec3d normal ( 0.0, 0.0, 1.0 );
      
      if( i < _shaderGroups.at( index ).heightMap->s() && j < _shaderGroups.at( index ).heightMap->t() )
      {
        double IJ          = static_cast< double > ( this->_getHeightFromImageAt( i + 0, j + 0, index ) );
        double IplusJ      = static_cast< double > ( this->_getHeightFromImageAt( i + 1, j + 0, index ) );
        double IJplus      = static_cast< double > ( this->_getHeightFromImageAt( i + 0, j + 1, index ) );

        osg::Vec3d Xij ( 1.0, 0.0, IplusJ - IJ );
        osg::Vec3d Yij ( 0.0, 1.0, IJplus - IJ );

        normal = Xij ^ Yij;
      }
      else if( i == _shaderGroups.at( index ).heightMap->s() && j < _shaderGroups.at( index ).heightMap->t()   )
      {
        double IJ          = static_cast< double > ( this->_getHeightFromImageAt( i + 0, j + 0, index ) );
        double IplusJ      = static_cast< double > ( this->_getHeightFromImageAt( i - 1, j + 0, index ) );
        double IJplus      = static_cast< double > ( this->_getHeightFromImageAt( i + 0, j + 1, index ) );

        osg::Vec3d Xij ( 1.0, 0.0, IplusJ - IJ );
        osg::Vec3d Yij ( 0.0, 1.0, IJplus - IJ );

        normal = Xij ^ Yij;

      }
      else if( j == _shaderGroups.at( index ).heightMap->t() && i < _shaderGroups.at( index ).heightMap->s() )
      {
        double IJ          = static_cast< double > ( this->_getHeightFromImageAt( i + 0, j + 0, index ) );
        double IplusJ      = static_cast< double > ( this->_getHeightFromImageAt( i + 1, j + 0, index ) );
        double IJplus      = static_cast< double > ( this->_getHeightFromImageAt( i + 0, j - 1, index ) );

        osg::Vec3d Xij ( 1.0, 0.0, IplusJ - IJ );
        osg::Vec3d Yij ( 0.0, 1.0, IJplus - IJ );

        normal = Xij ^ Yij;
      }
      else if( j == _shaderGroups.at( index ).heightMap->t() && i == _shaderGroups.at( index ).heightMap->s() )
      {
        double IJ          = static_cast< double > ( this->_getHeightFromImageAt( i + 0, j + 0, index ) );
        double IplusJ      = static_cast< double > ( this->_getHeightFromImageAt( i - 1, j + 0, index ) );
        double IJplus      = static_cast< double > ( this->_getHeightFromImageAt( i + 0, j - 1, index ) );

        osg::Vec3d Xij ( 1.0, 0.0, IplusJ - IJ );
        osg::Vec3d Yij ( 0.0, 1.0, IJplus - IJ );

        normal = Xij ^ Yij;
      }
      normal.normalize();

      // red color value for normal map
      unsigned char red = 0;
      if( normal.x() >= 0.0 && normal.x() <= 1.0 )
        red = static_cast< unsigned char > ( normal.x() * 255 );

      // green color value for normal map
      unsigned char green = 0;
      if( normal.y() >= 0.0 && normal.y() <= 1.0 )
        green = static_cast< unsigned char > ( normal.y() * 255 );

      // blue color value for normal map
      unsigned char blue = 0;
      if( normal.z() >= 0.0 && normal.z() <= 1.0 )
        blue = 255;

      texture.push_back( red );
      texture.push_back( green );
      texture.push_back( blue );
    }
  }
  //_scene->setImage( _imageResolution[0], _imageResolution[1], 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, &_colorVec[0], osg::Image::NO_DELETE );
  //_scene->dirty();
  normalMap->setImage( _shaderGroups.at( index ).heightMap->s(),
                       _shaderGroups.at( index ).heightMap->t(), 
                       1, 
                       GL_RGB, 
                       GL_RGB, 
                       GL_UNSIGNED_BYTE, 
                       &texture[0], 
                       osg::Image::NO_DELETE );
  normalMap->dirty();
  
  osgDB::writeImageFile( *normalMap.get(), name.c_str() );
  return normalMap.release();

}


///////////////////////////////////////////////////////////////////////////////
//
// Set the height map for normal maps
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* GeneralShaderDocument::_setHeightMap( const std::string &filename )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  osg::ref_ptr< osg::Image > image = osgDB::readImageFile( filename );

  return image.release();
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the height value from the height map at pos( x, y )
//
///////////////////////////////////////////////////////////////////////////////

char GeneralShaderDocument::_getHeightFromImageAt( unsigned int x, unsigned int y, unsigned int index )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  char retval;

  osg::ref_ptr< osg::Image > image = _shaderGroups.at( index ).heightMap;
  unsigned int s = static_cast< unsigned int > ( image->s() );
  unsigned int t = static_cast< unsigned int > ( image->t() );

  unsigned char * height = 0x0;
  if( x <= s && y <= t )
  {
    height = image->data( x, y );
  }
  retval = *height;
  return retval;
}




///////////////////////////////////////////////////////////////////////////////
//
// Read a .obj file and create a mesh from the contents.
// ( provided by ??? )
//
///////////////////////////////////////////////////////////////////////////////

GeneralShaderDocument::Mesh GeneralShaderDocument::_loadMesh( const std::string &filename )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  Usul::System::Directory::ScopedCwd cwd ( _workingDir );

  std::ifstream ifs;
  ifs.open( filename.c_str() );
	if(ifs.fail())
	{
    std::cout << "Error opening file: " << filename;
    throw std::runtime_error ( Usul::Strings::format( "Error 3340606906: Error opening file: ", filename ) );
	}

	Mesh mesh;

	char current_line[1024];

	while (!ifs.eof())
	{
		ifs.getline(current_line, 1024);

		switch (current_line[0])
		{		
		case 'v':
			{
				float x, y, z;
				switch(current_line[1])
				{
				case 'n':
					sscanf(current_line, "vn %f %f %f", &x,&y,&z);
					mesh.normals.push_back( osg::Vec3( x, y, z ) );
					break;

				case 't':
					//sscanf(current_line, "vt %f %f %f", &x,&y,&z); // z component is not used
					sscanf(current_line, "vt %f %f", &x,&y); // z component is not used
					mesh.texCoords.push_back( osg::Vec2( x, y ) );
					break;

				default:
					sscanf(current_line, "v %f %f %f", &x,&y,&z);
					mesh.vertices.push_back(osg::Vec3(x, y, z));
					break;
				}
			}
			break;

		case 'f':
			{
				std::vector< std::string > faces = split_string( current_line, " \t.\r\n" );

				int vnt[3][3] = { {-1, -1, -1}, {-1, -1, -1}, {-1, -1, -1} };
				std::string::size_type begin, end;

				for( int i = 0; i < 2; i++ )
				{
					//std::cout<<"f: "<<i<<" "<<faces[i+1]<<endl;
					begin = 0;
					int j = 0;
					do
					{
						end = faces[i+1].find_first_of( "/",  begin );

						if( begin < end )
						{
							//vnt[i][j] = StrToInt( faces[i+1].substr( begin, end-begin ) ) - 1;
              Usul::Strings::fromString( faces[i+1].substr( begin, end-begin ), vnt[i][j] );
              vnt[i][j] -= 1;
						}
						else
							vnt[i][j] = -1;
						begin = end + 1;
						j++;
					}while( end != std::string::npos );
				}

				for( unsigned int i = 3; i < faces.size(); i++ )
				{
					//std::cout<<"f: "<<i-1<<" "<<faces[i]<<endl;
					begin = 0;
					int j = 0;
					do
					{
						end = faces[i].find_first_of( "/", begin );

						if( begin < end )
						{
							//vnt[2][j] = StrToInt( faces[i].substr( begin, end-begin ) ) - 1;
              Usul::Strings::fromString( faces[i].substr( begin, end-begin ), vnt[2][j] );
              vnt[2][j] -= 1;
						}
						else
							vnt[2][j] = -1;
						begin = end + 1;
						j++;

					}while( end != std::string::npos );

					for( int j = 0; j < 3; j++ )
					{
						//std::cout<<"\t"<<vnt[j][0]<<" "<<vnt[j][1]<<" "<<vnt[j][2]<<"\n";
						mesh.indices.push_back( vnt[j][0] );

						if( vnt[j][1] != -1 ) mesh.texIndices.push_back( vnt[j][1] );
						if( vnt[j][2] != -1 ) mesh.normalIndices.push_back( vnt[j][2] );
					}

					memcpy( &vnt[1], &vnt[2], sizeof(int)*3 );
				}
			}
			break;
			
		default: break;
		}
	}

	return mesh;
}


///////////////////////////////////////////////////////////////////////////////
//
// Create a mesh object from file <filename> contents
//
///////////////////////////////////////////////////////////////////////////////

osg::Geode* GeneralShaderDocument::_createMeshModel( const std::string &filename )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  Usul::System::Directory::ScopedCwd cwd ( _workingDir );

  GeodePtr geode ( new osg::Geode );
  GeometryPtr geometry ( new osg::Geometry );

  // load the mesh into a GeneralShaderDocument::Mesh object
  Mesh mesh = this->_loadMesh( filename );


  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
  osg::ref_ptr< osg::Vec3Array > normals ( new osg::Vec3Array );
  osg::ref_ptr< osg::Vec4Array > tangents ( new osg::Vec4Array );
  osg::ref_ptr< osg::Vec2Array > texCoords ( new osg::Vec2Array );
  std::vector< std::vector< osg::Vec3 > > faceNormalsPerVertex;

  faceNormalsPerVertex.resize( mesh.vertices.size() );
  
  
  //Parse mesh object and create a geode
  for( unsigned int i = 0; i < mesh.indices.size(); i += 3 )
  {
    // get the vertices of a triangle in the mesh
    osg::Vec3 v1 = mesh.vertices.at( mesh.indices.at( i + 0 ) );
    osg::Vec3 v2 = mesh.vertices.at( mesh.indices.at( i + 1 ) );
    osg::Vec3 v3 = mesh.vertices.at( mesh.indices.at( i + 2 ) );
    vertices->push_back( v1 );vertices->push_back( v2 );vertices->push_back( v3 );

    // if there are normals provided in the file read and store
    if( mesh.normalIndices.size() > 0 && mesh.normals.size() > 0 )
    {
      osg::Vec3 n1 = mesh.normals.at( mesh.normalIndices.at( i + 0 ) );
      osg::Vec3 n2 = mesh.normals.at( mesh.normalIndices.at( i + 1 ) );
      osg::Vec3 n3 = mesh.normals.at( mesh.normalIndices.at( i + 2 ) );
      normals->push_back( n1 );normals->push_back( n2 );normals->push_back( n3 );

      // create and add tangents for each normal
      osg::Vec3 tVec = v1 - v2;
      tangents->push_back( osg::Vec4f( tVec.x(), tVec.y(), tVec.z(), 0.0 ) );
      tangents->push_back( osg::Vec4f( tVec.x(), tVec.y(), tVec.z(), 0.0 ) );
      tangents->push_back( osg::Vec4f( tVec.x(), tVec.y(), tVec.z(), 0.0 ) );
    }
    // create the normals if none were provided in the file
    else
    {
      FaceNormal fn;

      fn.normal = ( v3 - v2 ) ^ ( v1 - v2 );
      fn.vi1 = mesh.indices.at( i + 0 );
      fn.vi2 = mesh.indices.at( i + 1 );
      fn.vi3 = mesh.indices.at( i + 2 );
      
      faceNormalsPerVertex.at( fn.vi1 ).push_back( fn.normal ); 
      faceNormalsPerVertex.at( fn.vi2 ).push_back( fn.normal ); 
      faceNormalsPerVertex.at( fn.vi3 ).push_back( fn.normal ); 
    }

    // get the texture coordinates of a triangle in the mesh
    if( mesh.texCoords.size() > 0 && mesh.texIndices.size() > 0 )
    {
      osg::Vec2 t1 = mesh.texCoords.at( mesh.texIndices.at( i + 0 ) );
      osg::Vec2 t2 = mesh.texCoords.at( mesh.texIndices.at( i + 1 ) );
      osg::Vec2 t3 = mesh.texCoords.at( mesh.texIndices.at( i + 2 ) );
      texCoords->push_back( t1 );texCoords->push_back( t2 );texCoords->push_back( t3 );
    }

  }


  // add vertex array to geometry
  geometry->setVertexArray( vertices.get() );

  // add normals array to the geometry if there are any
  if( normals->size() > 0 )
  {
    // normals
    geometry->setNormalArray( normals.get() );
    geometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

    // tangents
    geometry->setColorArray( tangents.get() );
    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
  }
  // create per-vertex normals from face normals
  else
  {
    osg::ref_ptr< osg::Vec3Array > perVertexNormals ( new osg::Vec3Array );
    osg::ref_ptr< osg::Vec4Array > perVertexTangents( new osg::Vec4Array );
    for( unsigned int i = 0; i < faceNormalsPerVertex.size(); ++i )
    {
      unsigned int size = faceNormalsPerVertex.at( i ).size();
      switch( size )
      { 
        case 0:
          {
            perVertexNormals->push_back( osg::Vec3( 0.0, 0.0, 1.0 ) );
            perVertexTangents->push_back( osg::Vec4( 1.0, 0.0, 0.0, 0.0 ) );
          }
        case 1:
          {
            osg::Vec3 normal = faceNormalsPerVertex.at( i ).at( 0 );
            normal.normalize();
            perVertexNormals->push_back( normal );
            perVertexTangents->push_back( osg::Vec4( 1.0, 0.0, 0.0, 0.0 ) );
            break;
          };
        case 2:
          {
            osg::Vec3 n1 = faceNormalsPerVertex.at( i ).at( 0 );
            osg::Vec3 n2 = faceNormalsPerVertex.at( i ).at( 1 );
            osg::Vec3 normal = n1 ^ n2;
            normal.normalize();
            perVertexNormals->push_back( normal );

            osg::Vec3 t = n1 - n2;
            t.normalize();
            perVertexTangents->push_back( osg::Vec4( t.x(), t.y(), t.z(), 0.0 ) );
            break;
          };
        default:
          {
            osg::Vec3 normal ( 0.0, 0.0, 0.0 );
            for( unsigned int j = 0; j < faceNormalsPerVertex.at( i ).size(); ++j )
            {
              normal += faceNormalsPerVertex.at( i ).at( j );
            }
            normal /= faceNormalsPerVertex.at( i ).size();
            normal.normalize();
            perVertexNormals->push_back( normal );

            osg::Vec3 t = faceNormalsPerVertex.at( i ).at( 0 ) - faceNormalsPerVertex.at( i ).at( 1 );
            t.normalize();
            perVertexTangents->push_back( osg::Vec4( t.x(), t.y(), t.z(), 0.0 ) );
            break;
          };
      };
    } // end for loop
    for( unsigned int i = 0; i < mesh.indices.size(); ++i ) 
    {
      normals->push_back( perVertexNormals->at( mesh.indices.at( i ) ) );
      tangents->push_back( perVertexTangents->at( mesh.indices.at( i ) ) );
    }
    // normals
    geometry->setNormalArray( normals.get() );
    geometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

    // tangents
    geometry->setColorArray( tangents.get() );
    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

  } // end else

  // add texture coordinates to the geometry if there are any
  if( texCoords->size() > 0 )
  {
    geometry->setTexCoordArray( 0, texCoords.get() );
  }

  geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLES, 0, vertices->size() ) );

  geode->addDrawable( geometry.get() );
  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
// (re)Read the settings file
//
///////////////////////////////////////////////////////////////////////////////

void GeneralShaderDocument::readSettings( const std::string &workingDir )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  std::string filename = Usul::Strings::format( workingDir, "/config/settings.xml" );
  XmlTree::Document::RefPtr document ( new XmlTree::Document );
  document->load ( filename );

  if( "settings" == document->name() )
  {
    this->_parseSettings( *document );
  }

}


void GeneralShaderDocument::_parseSettings( XmlTree::Node &node, Unknown *caller, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

   Attributes& attributes ( node.attributes() );
  for ( Attributes::iterator iter = attributes.begin(); iter != attributes.end(); ++iter )
  {
    //if ( "usetools" == iter->first )
    //{
    //  //Usul::Strings::fromString ( iter->second, value );
    //}
  }

  Children& children ( node.children() );
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    if ( "uniform" == node->name() )
    {
      UniformPtr uniform ( this->_parseUniform( *node, caller, progress ) );
      _additionalUniforms.push_back( uniform.get() );
    }
    
   
  }
 
  this->_updateScene();
}
