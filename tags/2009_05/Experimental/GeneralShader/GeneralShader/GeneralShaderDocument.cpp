
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
#include "Usul/Interfaces/IClippingDistance.h"
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
#include "Usul/Scope/CurrentDirectory.h"

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
#include "osg/TexEnvCombine"
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
  _currentShader( 0 ),
  _workingDir( "." ),
  _eye( 0.0, 0.0, 0.0 ),
  _light( -50.0, -30.0, 100.0 ),
  _theta( 45.0, 0.0 ),
  _radius( 50.0 ),
  _modelMatrixTransform( new osg::MatrixTransform ),
  _heightMapImage( new osg::Image ),
  _tile( 1.0 ),
  _depth( 0.04 ),
  _etaRatio( 1.01, 1.02, 1.03 )
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

#if 0
    // get needed interface to access the near/far clipping planes
    Usul::Interfaces::IClippingDistance::QueryPtr clip ( caller );

    // set the near/far clip planes
    //planes.x=-far/(far-near); planes.y =-far*near/(far-near);
    float n = 0;
    float f = 0;
    clip->getClippingDistances( n, f );
    _nearFarClipPlane.x() = -f / ( f - n );
    _nearFarClipPlane.y() = ( -f * n ) / ( f - n );
#else
    // get needed interface to access the near/far clipping planes
    //Usul::Interfaces::IClippingDistance::QueryPtr clip ( caller );

    //if( true == clip.valid() )
    //{
    //  float n = 0.001;
    //  float f = 10;
    //  _nearFarClipPlane.x() = -f / ( f - n );
    //  _nearFarClipPlane.y() = ( -f * n ) / ( f - n );
    //  clip->setClippingDistances( n, f );
    //}
#endif

  }
}


void GeneralShaderDocument::addView     ( View   *view   )
{
  BaseClass::addView ( view );

   Usul::Interfaces::IClippingDistance::QueryPtr clip ( view );

    if( true == clip.valid() )
    {
      float n = 0.001;
      float f = 10;
      _nearFarClipPlane.x() = -f / ( f - n );
      _nearFarClipPlane.y() = ( -f * n ) / ( f - n );
      clip->setClippingDistances( n, f );
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

  Usul::Scope::CurrentDirectory cwd ( _workingDir );

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
          if( "plane" == type )
          {
            geode = this->_createPlaneModel();
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

  Usul::Scope::CurrentDirectory cwd ( _workingDir );

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


  Usul::Scope::CurrentDirectory cwd ( _workingDir );

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
      shaderGroup.heightMapPath = hm;
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
    if ( "texture" == iter->first )
    {
      std::string texName = "";
      Usul::Strings::fromString ( iter->second, texName );
      shaderGroup.texturePath = texName;

    }
    if ( "normalMap" == iter->first )
    {
      std::string texName = "";
      Usul::Strings::fromString ( iter->second, texName );
      shaderGroup.normalMapPath = texName;

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

  Usul::Scope::CurrentDirectory cwd ( _workingDir );

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
  if ( "sampler2D" == type )
  {

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

  Usul::Scope::CurrentDirectory cwd ( _workingDir );

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

  Usul::Scope::CurrentDirectory cwd ( _workingDir );

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

  Usul::Scope::CurrentDirectory cwd ( _workingDir );

  if( "" == _shaderGroups.at( index ).normalMapPath )
  {
    std::string name = _shaderGroups.at( index ).name;
    _shaderGroups.at( index ).normalMapPath = Usul::Strings::format( "NormalMaps/", name, ".jpg" );
    _shaderGroups.at( index ).normalMap = this->_createNormalMap( _shaderGroups.at( index ).normalMapPath, index );
  }
  std::string normalMapPath = _shaderGroups.at( index ).normalMapPath;

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
    tex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT );
    tex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT );
    tex->setWrap(osg::Texture::WRAP_R, osg::Texture::REPEAT );
/*    tex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    tex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR); */
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
    Usul::Scope::CurrentDirectory cwd ( _workingDir );

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
    stateset->setTextureAttributeAndModes( 1, map.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );

    // add cube map to stateset
    stateset->addUniform( new osg::Uniform( "map", 1 ) );
    _shaderGroups.at( index ).emMap = map;
  }

  {
    std::string texturePath = _shaderGroups.at( index ).texturePath;
    ImagePtr image = osgDB::readImageFile( texturePath.c_str() );
    if( true == image.valid() )
    {
      osg::ref_ptr< osg::Texture2D > tex ( new osg::Texture2D );

      tex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT );
      tex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT );
      //tex->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
      //tex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
      //tex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
      tex->setImage( image.get() );

      stateset->setTextureAttributeAndModes( 2, tex.get(), osg::StateAttribute::ON );
    }
    // add bump map to stateset
    stateset->addUniform( new osg::Uniform( "texture", 2 ) );
  }

  {
    std::string heightMapPath = _shaderGroups.at( index ).heightMapPath;
    ImagePtr image = osgDB::readImageFile( heightMapPath.c_str() );
    if( true == image.valid() )
    {
      osg::ref_ptr< osg::Texture2D > tex ( new osg::Texture2D );

      tex->setDataVariance( osg::Object::DYNAMIC );
      //tex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
      //tex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
      //tex->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
      //tex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
      //tex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
      tex->setImage( image.get() );

      stateset->setTextureAttributeAndModes( 3, tex.get(), osg::StateAttribute::ON );
    }
    // add bump map to stateset
    stateset->addUniform( new osg::Uniform( "heightMap", 3 ) );
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
#if 0
  geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),1.0f)));
#else
  osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );
  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
  osg::ref_ptr< osg::Vec3Array > normals ( new osg::Vec3Array );
  osg::ref_ptr< osg::Vec4Array > tangents ( new osg::Vec4Array );
  osg::ref_ptr< osg::Vec2Array > texCoords ( new osg::Vec2Array );

  unsigned int max_s = 50, max_t = 50;
  float radius = 10.0;
  Usul::Math::Vec3f center( 0.0, 0.0, 0.0 );
  for( unsigned int i = 0; i < max_s; ++i )
  {
    for( unsigned int j = 0; j < max_t; ++j )
    {
      float s0 = static_cast< float > ( i ) / max_s;
      float t0 = static_cast< float > ( j ) / max_t;
      float s1 = static_cast< float > ( i + 1 ) / max_s;
      float t1 = static_cast< float > ( j + 1 ) / max_t;

      float x00 = center[0] + radius * cos( 2.0 * Usul::Math::PIE * t0 ) * sin ( Usul::Math::PIE * s0 );
      float y00 = center[1] + radius * sin( 2.0 * Usul::Math::PIE * t0 ) * sin ( Usul::Math::PIE * s0 );
      float z00 = center[2] + radius * cos( Usul::Math::PIE * s0 );

      float x10 = center[0] + radius * cos( 2.0 * Usul::Math::PIE * t0 ) * sin ( Usul::Math::PIE * s1 );
      float y10 = center[1] + radius * sin( 2.0 * Usul::Math::PIE * t0 ) * sin ( Usul::Math::PIE * s1 );
      float z10 = center[2] + radius * cos( Usul::Math::PIE * s1 );

      float x01 = center[0] + radius * cos( 2.0 * Usul::Math::PIE * t1 ) * sin ( Usul::Math::PIE * s0 );
      float y01 = center[1] + radius * sin( 2.0 * Usul::Math::PIE * t1 ) * sin ( Usul::Math::PIE * s0 );
      float z01 = center[2] + radius * cos( Usul::Math::PIE * s0 );

      float x11 = center[0] + radius * cos( 2.0 * Usul::Math::PIE * t1 ) * sin ( Usul::Math::PIE * s1 );
      float y11 = center[1] + radius * sin( 2.0 * Usul::Math::PIE * t1 ) * sin ( Usul::Math::PIE * s1 );
      float z11 = center[2] + radius * cos( Usul::Math::PIE * s1 );

      // vertex 00
      Usul::Math::Vec3d tangent00( 0.0, 0.0, 0.0 );
      Usul::Math::Vec3d binormal00( 0.0, 0.0, 0.0 );
      Usul::Math::Vec3d normal00( 0.0, 0.0, 0.0 );

      tangent00[0] = -2.0 * Usul::Math::PIE * radius * sin( 2.0 * Usul::Math::PIE * t0 ) * sin ( Usul::Math::PIE * s0 );
      tangent00[1] = 2.0 * Usul::Math::PIE *radius * cos( 2.0 * Usul::Math::PIE * t0 ) * sin ( Usul::Math::PIE * s0 );
      tangent00[2] = 0;

      binormal00[0] = Usul::Math::PIE * radius * cos( 2.0 * Usul::Math::PIE * t0 ) * cos ( Usul::Math::PIE * s0 );
      binormal00[1] = Usul::Math::PIE * radius * sin( Usul::Math::PIE * t0 ) * cos ( Usul::Math::PIE * s0 );
      binormal00[2] = -Usul::Math::PIE * radius * sin( Usul::Math::PIE * s0 );

      normal00 = Usul::Math::Vec3d( center[0] + x00, center[1] + y00, center[2] + z00 );

      // vertex 10
      Usul::Math::Vec3d tangent10( 0.0, 0.0, 0.0 );
      Usul::Math::Vec3d binormal10( 0.0, 0.0, 0.0 );
      Usul::Math::Vec3d normal10( 0.0, 0.0, 0.0 );

      tangent10[0] = -2.0 * Usul::Math::PIE * radius * sin( 2.0 * Usul::Math::PIE * t0 ) * sin ( Usul::Math::PIE * s1 );
      tangent10[1] = 2.0 * Usul::Math::PIE *radius * cos( 2.0 * Usul::Math::PIE * t0 ) * sin ( Usul::Math::PIE * s1 );
      tangent10[2] = 0;

      binormal10[0] = Usul::Math::PIE * radius * cos( 2.0 * Usul::Math::PIE * t0 ) * cos ( Usul::Math::PIE * s1 );
      binormal10[1] = Usul::Math::PIE * radius * sin( Usul::Math::PIE * t0 ) * cos ( Usul::Math::PIE * s1 );
      binormal10[2] = -Usul::Math::PIE * radius * sin( Usul::Math::PIE * s1 );

      normal10 = Usul::Math::Vec3d( center[0] + x10, center[1] + y10, center[2] + z10 );

      // vertex 01
      Usul::Math::Vec3d tangent01( 0.0, 0.0, 0.0 );
      Usul::Math::Vec3d binormal01( 0.0, 0.0, 0.0 );
      Usul::Math::Vec3d normal01( 0.0, 0.0, 0.0 );

      tangent01[0] = -2.0 * Usul::Math::PIE * radius * sin( 2.0 * Usul::Math::PIE * t1 ) * sin ( Usul::Math::PIE * s0 );
      tangent01[1] = 2.0 * Usul::Math::PIE *radius * cos( 2.0 * Usul::Math::PIE * t1 ) * sin ( Usul::Math::PIE * s0 );
      tangent01[2] = 0;

      binormal01[0] = Usul::Math::PIE * radius * cos( 2.0 * Usul::Math::PIE * t1 ) * cos ( Usul::Math::PIE * s0 );
      binormal01[1] = Usul::Math::PIE * radius * sin( Usul::Math::PIE * t1 ) * cos ( Usul::Math::PIE * s0 );
      binormal01[2] = -Usul::Math::PIE * radius * sin( Usul::Math::PIE * s0 );

      normal01 = Usul::Math::Vec3d( center[0] + x01, center[1] + y01, center[2] + z01 );

      // vertex 11
      Usul::Math::Vec3d tangent11( 0.0, 0.0, 0.0 );
      Usul::Math::Vec3d binormal11( 0.0, 0.0, 0.0 );
      Usul::Math::Vec3d normal11( 0.0, 0.0, 0.0 );

      tangent11[0] = -2.0 * Usul::Math::PIE * radius * sin( 2.0 * Usul::Math::PIE * t1 ) * sin ( Usul::Math::PIE * s1 );
      tangent11[1] = 2.0 * Usul::Math::PIE *radius * cos( 2.0 * Usul::Math::PIE * t1 ) * sin ( Usul::Math::PIE * s1 );
      tangent11[2] = 0;

      binormal11[0] = Usul::Math::PIE * radius * cos( 2.0 * Usul::Math::PIE * t1 ) * cos ( Usul::Math::PIE * s1 );
      binormal11[1] = Usul::Math::PIE * radius * sin( Usul::Math::PIE * t1 ) * cos ( Usul::Math::PIE * s1 );
      binormal11[2] = -Usul::Math::PIE * radius * sin( Usul::Math::PIE * s1 );

      normal11 = Usul::Math::Vec3d( center[0] + x11, center[1] + y11, center[2] + z11 );

      vertices->push_back( osg::Vec3f( x00, y00, z00 ) );
      vertices->push_back( osg::Vec3f( x10, y10, z10 ) );
      vertices->push_back( osg::Vec3f( x11, y11, z11 ) );

      vertices->push_back( osg::Vec3f( x00, y00, z00 ) );
      vertices->push_back( osg::Vec3f( x11, y11, z11 ) );
      vertices->push_back( osg::Vec3f( x01, y01, z01 ) );

      tangents->push_back( osg::Vec4f( tangent00[0], tangent00[1], tangent00[2], 0.0 ) );
      tangents->push_back( osg::Vec4f( tangent10[0], tangent10[1], tangent10[2], 0.0 ) );
      tangents->push_back( osg::Vec4f( tangent11[0], tangent11[1], tangent11[2], 0.0 ) );

      tangents->push_back( osg::Vec4f( tangent00[0], tangent00[1], tangent00[2], 0.0 ) );
      tangents->push_back( osg::Vec4f( tangent11[0], tangent11[1], tangent11[2], 0.0 ) );
      tangents->push_back( osg::Vec4f( tangent01[0], tangent01[1], tangent01[2], 0.0 ) );

      normals->push_back( osg::Vec3f( normal00[0], normal00[1], normal00[2] ) );
      normals->push_back( osg::Vec3f( normal10[0], normal10[1], normal10[2] ) );
      normals->push_back( osg::Vec3f( normal11[0], normal11[1], normal11[2] ) );

      normals->push_back( osg::Vec3f( normal00[0], normal00[1], normal00[2] ) );
      normals->push_back( osg::Vec3f( normal11[0], normal11[1], normal11[2] ) );
      normals->push_back( osg::Vec3f( normal01[0], normal01[1], normal01[2] ) );

      texCoords->push_back( osg::Vec2f( s0, t0 ) );
      texCoords->push_back( osg::Vec2f( s1, t0 ) );
      texCoords->push_back( osg::Vec2f( s1, t1 ) );

      texCoords->push_back( osg::Vec2f( s0, t0 ) );
      texCoords->push_back( osg::Vec2f( s1, t1 ) );
      texCoords->push_back( osg::Vec2f( s0, t1 ) );

      //vertices->push_back( osg::Vec3f( x, y, z ) );
      //tangents->push_back( osg::Vec4f( tangent[0], tangent[1], tangent[2], 0.0 ) );
      //normals->push_back( osg::Vec3f( normal[0], normal[1], normal[2] ) );
      //texCoords->push_back( osg::Vec2f( s, t ) );
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
  tangent->push_back( osg::Vec4( 0.5, 0.5, 1.0, 1.0 ) );
  tangent->push_back( osg::Vec4( 0.5, 0.5, 1.0, 1.0 ) );
  tangent->push_back( osg::Vec4( 0.5, 0.5, 1.0, 1.0 ) );
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
  tangent->push_back( osg::Vec4( 0.5, 0.5, 1.0, 1.0 ) );
  tangent->push_back( osg::Vec4( 0.5, 0.5, 1.0, 1.0 ) );
  tangent->push_back( osg::Vec4( 0.5, 0.5, 1.0, 1.0 ) );
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

  tangent->push_back( osg::Vec4( 0.5, 0.5, 0.0, 1.0 ) );
  tangent->push_back( osg::Vec4( 0.5, 0.5, 0.0, 1.0 ) );
  tangent->push_back( osg::Vec4( 0.5, 0.5, 0.0, 1.0 ) );
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

  tangent->push_back( osg::Vec4( 0.5, 0.5, 0.0, 1.0 ) );
  tangent->push_back( osg::Vec4( 0.5, 0.5, 0.0, 1.0 ) );
  tangent->push_back( osg::Vec4( 0.5, 0.5, 0.0, 1.0 ) );
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

osg::Geode* GeneralShaderDocument::_createPlaneModel()
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
  //osg::Vec3d p0( -0.5, -0.5, -0.5 );
	//osg::Vec3d p1( 0.5, -0.5, -0.5 );
	//osg::Vec3d p2( -0.5, 0.5, -0.5 );
	//osg::Vec3d p3( 0.5, 0.5, -0.5 );
	osg::Vec3d p4( -0.5, -0.5, 0.5 );
	osg::Vec3d p5( 0.5, -0.5, 0.5 );
	osg::Vec3d p6( -0.5, 0.5, 0.5 );
	osg::Vec3d p7( 0.5, 0.5, 0.5 );

  // N0
  //vertices->push_back( p0 ); vertices->push_back( p3 ); vertices->push_back( p1 );
	//faceNormals->push_back( ( p1 - p3 ) ^ ( p0 - p3 ) );

  //N1
	//vertices->push_back( p0 ); vertices->push_back( p2 ); vertices->push_back( p3 );
	//faceNormals->push_back( ( p3 - p2 ) ^ ( p0 - p2 ) );

  //N2
	//vertices->push_back( p0 ); vertices->push_back( p6 ); vertices->push_back( p2 );
	//faceNormals->push_back( ( p2 - p6 ) ^ ( p0 - p6 ) );

  //N3
	//vertices->push_back( p0 ); vertices->push_back( p4 ); vertices->push_back( p6 );
	//faceNormals->push_back( ( p6 - p4 ) ^ ( p0 - p4 ) );

  //N4
	//vertices->push_back( p0 ); vertices->push_back( p1 ); vertices->push_back( p5 );
	//faceNormals->push_back( ( p5 - p1 ) ^ ( p0 - p1 ) );

  //N5
	//vertices->push_back( p0 ); vertices->push_back( p5 ); vertices->push_back( p4 );
	//faceNormals->push_back( ( p4 - p5 ) ^ ( p0 - p5 ) );

  //N6
	//vertices->push_back( p7 ); vertices->push_back( p3 ); vertices->push_back( p2 );
	//faceNormals->push_back( ( p2 - p3 ) ^ ( p7 - p3 ) );

  //N7
	//vertices->push_back( p7 ); vertices->push_back( p2 ); vertices->push_back( p6 );
	//faceNormals->push_back( ( p6 - p2 ) ^ ( p7 - p2 ) );

  //N8
	vertices->push_back( p7 ); vertices->push_back( p6 ); vertices->push_back( p4 );
	faceNormals->push_back( ( p4 - p6 ) ^ ( p7 - p6 ) );

  //N9
	vertices->push_back( p7 ); vertices->push_back( p4 ); vertices->push_back( p5 );
	faceNormals->push_back( ( p5 - p4 ) ^ ( p7 - p4 ) );

  //N10
	//vertices->push_back( p7 ); vertices->push_back( p5 ); vertices->push_back( p1 );
	//faceNormals->push_back( ( p1 - p5 ) ^ ( p7 - p5 ) );

  //N11
	//vertices->push_back( p7 ); vertices->push_back( p1 ); vertices->push_back( p3 );
	//faceNormals->push_back( ( p3 - p1 ) ^ ( p7 - p1 ) );

#if 0
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
// -Z Face
//--------------------------------------------------------------------------

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

  #if 1
    osg::Vec4 tng ( 1.0, 0.5, 0.5, 1.0 );
    tangent->push_back( tng );
    tangent->push_back( tng );
    tangent->push_back( tng );
  #else

    //v = vertex
    osg::Vec3 v1 = vertices->at( 0 );
    osg::Vec3 v2 = vertices->at( 1 );
    osg::Vec3 v3 = vertices->at( 2 );

    //w = texcoord
    osg::Vec2 w1 = tex->at( 0 );
    osg::Vec2 w2 = tex->at( 1 );
    osg::Vec2 w3 = tex->at( 2 );

    float x1 = v2.x() - v1.x();
    float x2 = v3.x() - v1.x();
    float y1 = v2.y() - v1.y();
    float y2 = v3.y() - v1.y();
    float z1 = v2.z() - v1.z();
    float z2 = v3.z() - v1.z();

    float s1 = w2.x() - w1.x();
    float s2 = w3.x() - w1.x();
    float t1 = w2.y() - w1.y();
    float t2 = w3.y() - w1.y();

    float r = 1.0f / ( s1 * t2 - s2 * t1 );

    osg::Vec3 sdir( ( t2 * x1 - t1 * x2 ) * r,
                    ( t2 * y1 - t1 * y2 ) * r,
                    ( t2 * z1 - t1 * z2 ) * r );

    osg::Vec3 tdir( ( s1 * x2 - s2 * x1 ) * r,
                    ( s1 * y2 - s2 * y1 ) * r,
                    ( s1 * z2 - s2 * z1 ) * r );

    //osg::Vec3 n = normals->at( 0 );

    // Gram-Schmidt orthogonalize
    //osg::Vec3 tng = ( sdir - n * ( n, sdir ) ).Normalize();
    Usul::Math::Vec3f n ( normals->at( 0 ).x(), normals->at( 0 ).y(), normals->at( 0 ).z() );
    Usul::Math::Vec3f td ( sdir.x(), sdir.y(), sdir.z() );
    Usul::Math::Vec3f tan1 = ( td - n * ( n.dot( td ) ) );
    //tan1.normalize();
    osg::Vec3 t ( tan1[0], tan1[1], tan1[2] );

    tangent->push_back( osg::Vec4( t.x(), t.y(), t.z(), 1.0 ) );
    tangent->push_back( osg::Vec4( t.x(), t.y(), t.z(), 1.0 ) );
    tangent->push_back( osg::Vec4( t.x(), t.y(), t.z(), 1.0 ) );

  #endif

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

  #if 1
    tangent->push_back( tng );
    tangent->push_back( tng );
    tangent->push_back( tng );
  #else

    tangent->push_back( osg::Vec4( t.x(), t.y(), t.z(), 1.0 ) );
    tangent->push_back( osg::Vec4( t.x(), t.y(), t.z(), 1.0 ) );
    tangent->push_back( osg::Vec4( t.x(), t.y(), t.z(), 1.0 ) );

  #endif

#endif

  //--------------------------------------------------------------------------

	geometry->setVertexArray( vertices.get() );
  geometry->setTexCoordArray( 0, tex.get() );
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


void GeneralShaderDocument::_createSkyBox( unsigned int index )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  Usul::Scope::CurrentDirectory cwd ( _workingDir );

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

  double scale = 1000.0;
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

    // set tex env combine
    osg::ref_ptr< osg::TexEnvCombine > texCombine ( new osg::TexEnvCombine );
    texCombine->setCombine_RGB( GL_REPLACE );


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



    //SSposy->setAttributeAndModes( texCombine.get() );
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

    // set tex env combine
    osg::ref_ptr< osg::TexEnvCombine > texCombine ( new osg::TexEnvCombine );
    texCombine->setCombine_RGB( GL_REPLACE );

    SSposz->setAttributeAndModes( texCombine.get() );
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

    // set tex env combine
    osg::ref_ptr< osg::TexEnvCombine > texCombine ( new osg::TexEnvCombine );
    texCombine->setCombine_RGB( GL_REPLACE );

    SSnegx->setAttributeAndModes( texCombine.get() );
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

    // set tex env combine
    osg::ref_ptr< osg::TexEnvCombine > texCombine ( new osg::TexEnvCombine );
    texCombine->setCombine_RGB( GL_REPLACE );

    SSnegy->setAttributeAndModes( texCombine.get() );
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

    // set tex env combine
    osg::ref_ptr< osg::TexEnvCombine > texCombine ( new osg::TexEnvCombine );
    texCombine->setCombine_RGB( GL_REPLACE );

    SSnegz->setAttributeAndModes( texCombine.get() );
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

  // set tex env combine
  osg::ref_ptr< osg::TexEnvCombine > texCombine ( new osg::TexEnvCombine );
  texCombine->setCombine_RGB( GL_REPLACE );
  group->getOrCreateStateSet()->setTextureAttributeAndModes( 0, texCombine.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );

  group->addChild( geode.get() );
  group->setCullingActive( false );
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

  osg::ref_ptr< osg::Vec2Array > texCoords ( new osg::Vec2Array );
  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
  osg::ref_ptr< osg::Vec3Array > normals ( new osg::Vec3Array );
  osg::ref_ptr< osg::Vec4Array > tangents ( new osg::Vec4Array );


  unsigned int max_s = 50, max_t = 50;
  double mRadius = 4, nRadius = 2;
  for( unsigned int i = 0; i < max_s; ++i )
  {
    for( unsigned int j = 0; j < max_t; ++j )
    {
      float s0 = static_cast< float > ( i ) / max_s;
      float t0 = static_cast< float > ( j ) / max_t;
      float s1 = static_cast< float > ( i + 1 ) / max_s;
      float t1 = static_cast< float > ( j + 1 ) / max_t;

      float x00 = ( mRadius + ( nRadius * cos( 2.0 * Usul::Math::PIE * t0 ) ) ) * ( cos( 2.0 * Usul::Math::PIE * s0 ) );
      float y00 = ( mRadius + ( nRadius * cos( 2.0 * Usul::Math::PIE * t0 ) ) ) * ( sin( 2.0 * Usul::Math::PIE * s0 ) );
      float z00 = nRadius * sin( 2.0 * Usul::Math::PIE * t0 ) ;

      float x10 = ( mRadius + ( nRadius * cos( 2.0 * Usul::Math::PIE * t0 ) ) ) * ( cos( 2.0 * Usul::Math::PIE * s1 ) );
      float y10 = ( mRadius + ( nRadius * cos( 2.0 * Usul::Math::PIE * t0 ) ) ) * ( sin( 2.0 * Usul::Math::PIE * s1 ) );
      float z10 = nRadius * sin( 2.0 * Usul::Math::PIE * t0 ) ;

      float x01 = ( mRadius + ( nRadius * cos( 2.0 * Usul::Math::PIE * t1 ) ) ) * ( cos( 2.0 * Usul::Math::PIE * s0 ) );
      float y01 = ( mRadius + ( nRadius * cos( 2.0 * Usul::Math::PIE * t1 ) ) ) * ( sin( 2.0 * Usul::Math::PIE * s0 ) );
      float z01 = nRadius * sin( 2.0 * Usul::Math::PIE * t1 ) ;

      float x11 = ( mRadius + ( nRadius * cos( 2.0 * Usul::Math::PIE * t1 ) ) ) * ( cos( 2.0 * Usul::Math::PIE * s1 ) );
      float y11 = ( mRadius + ( nRadius * cos( 2.0 * Usul::Math::PIE * t1 ) ) ) * ( sin( 2.0 * Usul::Math::PIE * s1 ) );
      float z11 = nRadius * sin( 2.0 * Usul::Math::PIE * t1 ) ;

      // vertex 00
      Usul::Math::Vec3d tangent00( 0.0, 0.0, 0.0 );
      Usul::Math::Vec3d binormal00( 0.0, 0.0, 0.0 );
      Usul::Math::Vec3d normal00( 0.0, 0.0, 0.0 );

      tangent00[0] = -2.0 * Usul::Math::PIE * ( mRadius + nRadius * ( cos( 2.0 * Usul::Math::PIE * t0 ) ) ) * ( sin( 2.0 * Usul::Math::PIE * s0 ) );
      tangent00[1] = -2.0 * Usul::Math::PIE * ( mRadius + nRadius * ( cos( 2.0 * Usul::Math::PIE * t0 ) ) ) * ( cos( 2.0 * Usul::Math::PIE * s0 ) );
      tangent00[2] = 0;

      binormal00[0] = -2.0 * nRadius * Usul::Math::PIE * sin( 2.0 * Usul::Math::PIE * t0 ) * cos( 2.0 * Usul::Math::PIE * s0 );
      binormal00[1] = -2.0 * nRadius * Usul::Math::PIE * cos( 2.0 * Usul::Math::PIE * t0 ) * sin( 2.0 * Usul::Math::PIE * s0 );
      binormal00[2] = 2.0 * nRadius * Usul::Math::PIE * cos( 2.0 * Usul::Math::PIE * t0 );

      normal00 = tangent00.cross( binormal00 );

      // vertex 10
      Usul::Math::Vec3d tangent10( 0.0, 0.0, 0.0 );
      Usul::Math::Vec3d binormal10( 0.0, 0.0, 0.0 );
      Usul::Math::Vec3d normal10( 0.0, 0.0, 0.0 );

      tangent10[0] = -2.0 * Usul::Math::PIE * ( mRadius + nRadius * ( cos( 2.0 * Usul::Math::PIE * t0 ) ) ) * ( sin( 2.0 * Usul::Math::PIE * s1 ) );
      tangent10[1] = -2.0 * Usul::Math::PIE * ( mRadius + nRadius * ( cos( 2.0 * Usul::Math::PIE * t0 ) ) ) * ( cos( 2.0 * Usul::Math::PIE * s1 ) );
      tangent10[2] = 0;

      binormal10[0] = -2.0 * nRadius * Usul::Math::PIE * sin( 2.0 * Usul::Math::PIE * t0 ) * cos( 2.0 * Usul::Math::PIE * s1 );
      binormal10[1] = -2.0 * nRadius * Usul::Math::PIE * cos( 2.0 * Usul::Math::PIE * t0 ) * sin( 2.0 * Usul::Math::PIE * s1 );
      binormal10[2] = 2.0 * nRadius * Usul::Math::PIE * cos( 2.0 * Usul::Math::PIE * t0 );

      normal10 = tangent10.cross( binormal10 );

      // vertex 01
      Usul::Math::Vec3d tangent01( 0.0, 0.0, 0.0 );
      Usul::Math::Vec3d binormal01( 0.0, 0.0, 0.0 );
      Usul::Math::Vec3d normal01( 0.0, 0.0, 0.0 );

      tangent01[0] = -2.0 * Usul::Math::PIE * ( mRadius + nRadius * ( cos( 2.0 * Usul::Math::PIE * t1 ) ) ) * ( sin( 2.0 * Usul::Math::PIE * s0 ) );
      tangent01[1] = -2.0 * Usul::Math::PIE * ( mRadius + nRadius * ( cos( 2.0 * Usul::Math::PIE * t1 ) ) ) * ( cos( 2.0 * Usul::Math::PIE * s0 ) );
      tangent01[2] = 0;

      binormal01[0] = -2.0 * nRadius * Usul::Math::PIE * sin( 2.0 * Usul::Math::PIE * t1 ) * cos( 2.0 * Usul::Math::PIE * s0 );
      binormal01[1] = -2.0 * nRadius * Usul::Math::PIE * cos( 2.0 * Usul::Math::PIE * t1 ) * sin( 2.0 * Usul::Math::PIE * s0 );
      binormal01[2] = 2.0 * nRadius * Usul::Math::PIE * cos( 2.0 * Usul::Math::PIE * t1 );

      normal01 = tangent01.cross( binormal01 );

      // vertex 11
      Usul::Math::Vec3d tangent11( 0.0, 0.0, 0.0 );
      Usul::Math::Vec3d binormal11( 0.0, 0.0, 0.0 );
      Usul::Math::Vec3d normal11( 0.0, 0.0, 0.0 );

      tangent11[0] = -2.0 * Usul::Math::PIE * ( mRadius + nRadius * ( cos( 2.0 * Usul::Math::PIE * t1 ) ) ) * ( sin( 2.0 * Usul::Math::PIE * s1 ) );
      tangent11[1] = -2.0 * Usul::Math::PIE * ( mRadius + nRadius * ( cos( 2.0 * Usul::Math::PIE * t1 ) ) ) * ( cos( 2.0 * Usul::Math::PIE * s1 ) );
      tangent11[2] = 0;

      binormal11[0] = -2.0 * nRadius * Usul::Math::PIE * sin( 2.0 * Usul::Math::PIE * t1 ) * cos( 2.0 * Usul::Math::PIE * s1 );
      binormal11[1] = -2.0 * nRadius * Usul::Math::PIE * cos( 2.0 * Usul::Math::PIE * t1 ) * sin( 2.0 * Usul::Math::PIE * s1 );
      binormal11[2] = 2.0 * nRadius * Usul::Math::PIE * cos( 2.0 * Usul::Math::PIE * t1 );

      normal11 = tangent11.cross( binormal11 );

      vertices->push_back( osg::Vec3f( x00, y00, z00 ) );
      vertices->push_back( osg::Vec3f( x10, y10, z10 ) );
      vertices->push_back( osg::Vec3f( x11, y11, z11 ) );

      vertices->push_back( osg::Vec3f( x00, y00, z00 ) );
      vertices->push_back( osg::Vec3f( x11, y11, z11 ) );
      vertices->push_back( osg::Vec3f( x01, y01, z01 ) );

      tangents->push_back( osg::Vec4f( tangent00[0], tangent00[1], tangent00[2], 0.0 ) );
      tangents->push_back( osg::Vec4f( tangent10[0], tangent10[1], tangent10[2], 0.0 ) );
      tangents->push_back( osg::Vec4f( tangent11[0], tangent11[1], tangent11[2], 0.0 ) );

      tangents->push_back( osg::Vec4f( tangent00[0], tangent00[1], tangent00[2], 0.0 ) );
      tangents->push_back( osg::Vec4f( tangent11[0], tangent11[1], tangent11[2], 0.0 ) );
      tangents->push_back( osg::Vec4f( tangent01[0], tangent01[1], tangent01[2], 0.0 ) );

      normals->push_back( osg::Vec3f( normal00[0], normal00[1], normal00[2] ) );
      normals->push_back( osg::Vec3f( normal10[0], normal10[1], normal10[2] ) );
      normals->push_back( osg::Vec3f( normal11[0], normal11[1], normal11[2] ) );

      normals->push_back( osg::Vec3f( normal00[0], normal00[1], normal00[2] ) );
      normals->push_back( osg::Vec3f( normal11[0], normal11[1], normal11[2] ) );
      normals->push_back( osg::Vec3f( normal01[0], normal01[1], normal01[2] ) );

      texCoords->push_back( osg::Vec2f( s0, t0 ) );
      texCoords->push_back( osg::Vec2f( s1, t0 ) );
      texCoords->push_back( osg::Vec2f( s1, t1 ) );

      texCoords->push_back( osg::Vec2f( s0, t0 ) );
      texCoords->push_back( osg::Vec2f( s1, t1 ) );
      texCoords->push_back( osg::Vec2f( s0, t1 ) );

      //vertices->push_back( osg::Vec3f( x, y, z ) );
      //tangents->push_back( osg::Vec4f( tangent[0], tangent[1], tangent[2], 0.0 ) );
      //normals->push_back( osg::Vec3f( normal[0], normal[1], normal[2] ) );
      //texCoords->push_back( osg::Vec2f( s, t ) );

    }
  }
  // last step to close everything
#if 0
  {
    float s0 = 1.0;
    float t0 = 1.0;
    float s1 = 0.0;
    float t1 = 0.0;

    float x00 = ( mRadius + ( nRadius * cos( 2.0 * Usul::Math::PIE * t0 ) ) ) * ( cos( 2.0 * Usul::Math::PIE * s0 ) );
    float y00 = ( mRadius + ( nRadius * cos( 2.0 * Usul::Math::PIE * t0 ) ) ) * ( sin( 2.0 * Usul::Math::PIE * s0 ) );
    float z00 = nRadius * sin( 2.0 * Usul::Math::PIE * t0 ) ;

    float x10 = ( mRadius + ( nRadius * cos( 2.0 * Usul::Math::PIE * t0 ) ) ) * ( cos( 2.0 * Usul::Math::PIE * s1 ) );
    float y10 = ( mRadius + ( nRadius * cos( 2.0 * Usul::Math::PIE * t0 ) ) ) * ( sin( 2.0 * Usul::Math::PIE * s1 ) );
    float z10 = nRadius * sin( 2.0 * Usul::Math::PIE * t0 ) ;

    float x01 = ( mRadius + ( nRadius * cos( 2.0 * Usul::Math::PIE * t1 ) ) ) * ( cos( 2.0 * Usul::Math::PIE * s0 ) );
    float y01 = ( mRadius + ( nRadius * cos( 2.0 * Usul::Math::PIE * t1 ) ) ) * ( sin( 2.0 * Usul::Math::PIE * s0 ) );
    float z01 = nRadius * sin( 2.0 * Usul::Math::PIE * t1 ) ;

    float x11 = ( mRadius + ( nRadius * cos( 2.0 * Usul::Math::PIE * t1 ) ) ) * ( cos( 2.0 * Usul::Math::PIE * s1 ) );
    float y11 = ( mRadius + ( nRadius * cos( 2.0 * Usul::Math::PIE * t1 ) ) ) * ( sin( 2.0 * Usul::Math::PIE * s1 ) );
    float z11 = nRadius * sin( 2.0 * Usul::Math::PIE * t1 ) ;

    // vertex 00
    Usul::Math::Vec3d tangent00( 0.0, 0.0, 0.0 );
    Usul::Math::Vec3d binormal00( 0.0, 0.0, 0.0 );
    Usul::Math::Vec3d normal00( 0.0, 0.0, 0.0 );

    tangent00[0] = -2.0 * Usul::Math::PIE * ( mRadius + nRadius * ( cos( 2.0 * Usul::Math::PIE * t0 ) ) ) * ( sin( 2.0 * Usul::Math::PIE * s0 ) );
    tangent00[1] = -2.0 * Usul::Math::PIE * ( mRadius + nRadius * ( cos( 2.0 * Usul::Math::PIE * t0 ) ) ) * ( cos( 2.0 * Usul::Math::PIE * s0 ) );
    tangent00[2] = 0;

    binormal00[0] = -2.0 * nRadius * Usul::Math::PIE * sin( 2.0 * Usul::Math::PIE * t0 ) * cos( 2.0 * Usul::Math::PIE * s0 );
    binormal00[1] = -2.0 * nRadius * Usul::Math::PIE * cos( 2.0 * Usul::Math::PIE * t0 ) * sin( 2.0 * Usul::Math::PIE * s0 );
    binormal00[2] = 2.0 * nRadius * Usul::Math::PIE * cos( 2.0 * Usul::Math::PIE * t0 );

    normal00 = tangent00.cross( binormal00 );

    // vertex 10
    Usul::Math::Vec3d tangent10( 0.0, 0.0, 0.0 );
    Usul::Math::Vec3d binormal10( 0.0, 0.0, 0.0 );
    Usul::Math::Vec3d normal10( 0.0, 0.0, 0.0 );

    tangent10[0] = -2.0 * Usul::Math::PIE * ( mRadius + nRadius * ( cos( 2.0 * Usul::Math::PIE * t0 ) ) ) * ( sin( 2.0 * Usul::Math::PIE * s1 ) );
    tangent10[1] = -2.0 * Usul::Math::PIE * ( mRadius + nRadius * ( cos( 2.0 * Usul::Math::PIE * t0 ) ) ) * ( cos( 2.0 * Usul::Math::PIE * s1 ) );
    tangent10[2] = 0;

    binormal10[0] = -2.0 * nRadius * Usul::Math::PIE * sin( 2.0 * Usul::Math::PIE * t0 ) * cos( 2.0 * Usul::Math::PIE * s1 );
    binormal10[1] = -2.0 * nRadius * Usul::Math::PIE * cos( 2.0 * Usul::Math::PIE * t0 ) * sin( 2.0 * Usul::Math::PIE * s1 );
    binormal10[2] = 2.0 * nRadius * Usul::Math::PIE * cos( 2.0 * Usul::Math::PIE * t0 );

    normal10 = tangent10.cross( binormal10 );

    // vertex 01
    Usul::Math::Vec3d tangent01( 0.0, 0.0, 0.0 );
    Usul::Math::Vec3d binormal01( 0.0, 0.0, 0.0 );
    Usul::Math::Vec3d normal01( 0.0, 0.0, 0.0 );

    tangent01[0] = -2.0 * Usul::Math::PIE * ( mRadius + nRadius * ( cos( 2.0 * Usul::Math::PIE * t1 ) ) ) * ( sin( 2.0 * Usul::Math::PIE * s0 ) );
    tangent01[1] = -2.0 * Usul::Math::PIE * ( mRadius + nRadius * ( cos( 2.0 * Usul::Math::PIE * t1 ) ) ) * ( cos( 2.0 * Usul::Math::PIE * s0 ) );
    tangent01[2] = 0;

    binormal01[0] = -2.0 * nRadius * Usul::Math::PIE * sin( 2.0 * Usul::Math::PIE * t1 ) * cos( 2.0 * Usul::Math::PIE * s0 );
    binormal01[1] = -2.0 * nRadius * Usul::Math::PIE * cos( 2.0 * Usul::Math::PIE * t1 ) * sin( 2.0 * Usul::Math::PIE * s0 );
    binormal01[2] = 2.0 * nRadius * Usul::Math::PIE * cos( 2.0 * Usul::Math::PIE * t1 );

    normal01 = tangent01.cross( binormal01 );

    // vertex 11
    Usul::Math::Vec3d tangent11( 0.0, 0.0, 0.0 );
    Usul::Math::Vec3d binormal11( 0.0, 0.0, 0.0 );
    Usul::Math::Vec3d normal11( 0.0, 0.0, 0.0 );

    tangent11[0] = -2.0 * Usul::Math::PIE * ( mRadius + nRadius * ( cos( 2.0 * Usul::Math::PIE * t1 ) ) ) * ( sin( 2.0 * Usul::Math::PIE * s1 ) );
    tangent11[1] = -2.0 * Usul::Math::PIE * ( mRadius + nRadius * ( cos( 2.0 * Usul::Math::PIE * t1 ) ) ) * ( cos( 2.0 * Usul::Math::PIE * s1 ) );
    tangent11[2] = 0;

    binormal11[0] = -2.0 * nRadius * Usul::Math::PIE * sin( 2.0 * Usul::Math::PIE * t1 ) * cos( 2.0 * Usul::Math::PIE * s1 );
    binormal11[1] = -2.0 * nRadius * Usul::Math::PIE * cos( 2.0 * Usul::Math::PIE * t1 ) * sin( 2.0 * Usul::Math::PIE * s1 );
    binormal11[2] = 2.0 * nRadius * Usul::Math::PIE * cos( 2.0 * Usul::Math::PIE * t1 );

    normal11 = tangent11.cross( binormal11 );

    vertices->push_back( osg::Vec3f( x00, y00, z00 ) );
    vertices->push_back( osg::Vec3f( x10, y10, z10 ) );
    vertices->push_back( osg::Vec3f( x11, y11, z11 ) );

    vertices->push_back( osg::Vec3f( x00, y00, z00 ) );
    vertices->push_back( osg::Vec3f( x11, y11, z11 ) );
    vertices->push_back( osg::Vec3f( x01, y01, z01 ) );

    tangents->push_back( osg::Vec4f( tangent00[0], tangent00[1], tangent00[2], 0.0 ) );
    tangents->push_back( osg::Vec4f( tangent10[0], tangent10[1], tangent10[2], 0.0 ) );
    tangents->push_back( osg::Vec4f( tangent11[0], tangent11[1], tangent11[2], 0.0 ) );

    tangents->push_back( osg::Vec4f( tangent00[0], tangent00[1], tangent00[2], 0.0 ) );
    tangents->push_back( osg::Vec4f( tangent11[0], tangent11[1], tangent11[2], 0.0 ) );
    tangents->push_back( osg::Vec4f( tangent01[0], tangent01[1], tangent01[2], 0.0 ) );

    normals->push_back( osg::Vec3f( normal00[0], normal00[1], normal00[2] ) );
    normals->push_back( osg::Vec3f( normal10[0], normal10[1], normal10[2] ) );
    normals->push_back( osg::Vec3f( normal11[0], normal11[1], normal11[2] ) );

    normals->push_back( osg::Vec3f( normal00[0], normal00[1], normal00[2] ) );
    normals->push_back( osg::Vec3f( normal11[0], normal11[1], normal11[2] ) );
    normals->push_back( osg::Vec3f( normal01[0], normal01[1], normal01[2] ) );

    texCoords->push_back( osg::Vec2f( s0, t0 ) );
    texCoords->push_back( osg::Vec2f( s1, t0 ) );
    texCoords->push_back( osg::Vec2f( s1, t1 ) );

    texCoords->push_back( osg::Vec2f( s0, t0 ) );
    texCoords->push_back( osg::Vec2f( s1, t1 ) );
    texCoords->push_back( osg::Vec2f( s0, t1 ) );
  }
#endif
  osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );

  geometry->setVertexArray( vertices.get() );

  geometry->setNormalArray( normals.get() );
  geometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

  geometry->setTexCoordArray( 0, texCoords.get() );
  geometry->setColorArray( tangents.get() );
  geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

  geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLES, 0, vertices->size() ) );

  geode->addDrawable( geometry.get() );
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

  Usul::Scope::CurrentDirectory cwd ( _workingDir );

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

    // add the etaRatio vec3
    UniformPtr eta = ss->getOrCreateUniform( "etaRatio", osg::Uniform::FLOAT_VEC3 );
    eta->set( _etaRatio );
    ss->addUniform( eta.get() );

    // add the near/far plane uniform
    UniformPtr nearfar = ss->getOrCreateUniform( "planes", osg::Uniform::FLOAT_VEC2 );
    nearfar->set( _nearFarClipPlane );
    ss->addUniform( nearfar.get() );

    //update the depth
    UniformPtr tile = ss->getOrCreateUniform( "tile", osg::Uniform::FLOAT );
    tile->set( _tile );
    ss->addUniform( tile.get() );

    //update the tile
    UniformPtr depth = ss->getOrCreateUniform( "depth", osg::Uniform::FLOAT );
    depth->set( _depth );
    ss->addUniform( depth.get() );

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

  // Depth value manipulation
  if( 'd' == code || 'D' == code )
  {
    _depth -= 0.01;
    this->_updateScene();
    return true;
  }
  if( 'c' == code || 'C' == code )
  {
    _depth += 0.01;
    this->_updateScene();
    return true;
  }

 // Tile value manipulation
  if( 't' == code || 'T' == code )
  {
    _tile *= 0.5;
    this->_updateScene();
    return true;
  }
  if( 'g' == code || 'G' == code )
  {
    _tile *= 2.0;
    this->_updateScene();
    return true;
  }

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
  if( 'r' == code || 'R' == code )
  {
    _modelMatrixTransform->setMatrix( osg::Matrix::identity() );
    _theta.x() = 45.0;
    _theta.y() = 0.0;
    _depth = 0.04;
    _tile = 1.0;
    _etaRatio = osg::Vec3f( 1.01, 1.02, 1.03 );
    this->_updateScene();
    return true;
  }

  if( '1' == code  )
  {
    _etaRatio.x() += 0.001;
    this->_updateScene();
    return true;
  }
  if( '2' == code  )
  {
    _etaRatio.x() -= 0.001;
    this->_updateScene();
    return true;
  }
  if( '4' == code  )
  {
    _etaRatio.y() += 0.001;
    this->_updateScene();
    return true;
  }
  if( '5' == code  )
  {
    _etaRatio.y() -= 0.001;
    this->_updateScene();
    return true;
  }
  if( '7' == code  )
  {
    _etaRatio.z() += 0.001;
    this->_updateScene();
    return true;
  }
  if( '8' == code  )
  {
    _etaRatio.z() -= 0.001;
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

  Usul::Scope::CurrentDirectory cwd ( _workingDir );

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

      // Alpha chanel
      //unsigned char alpha = this->_getHeightFromImageAt( i, j, index );

      texture.push_back( red );
      texture.push_back( green );
      texture.push_back( blue );
      //texture.push_back( alpha );
    }
  }
  std::vector< unsigned char > tex;
  {
    //unsigned int i = 0, j = 0;
    unsigned int s = static_cast< unsigned int > ( _shaderGroups.at( index ).heightMap->s() );
    unsigned int t = static_cast< unsigned int > ( _shaderGroups.at( index ).heightMap->t() );

    unsigned int ceiling = 0;
    for( unsigned int i = 0; i < t; ++i )
    {
      ceiling = ( t - 1 ) - i;
      for( unsigned int j = 0; j < s; ++j )
      {
        unsigned int index = ( j * s ) + ceiling;
        tex.push_back( texture.at( index ) );
      }

    }

  }

  normalMap->allocateImage ( _shaderGroups.at( index ).heightMap->s(),
                       _shaderGroups.at( index ).heightMap->t(),
                       1,
                       GL_RGB,
                       GL_UNSIGNED_BYTE );

  /*normalMap->setImage( _shaderGroups.at( index ).heightMap->s(),
                       _shaderGroups.at( index ).heightMap->t(),
                       1,
                       GL_RGB,
                       GL_RGB,
                       GL_UNSIGNED_BYTE,
                       &tex[0],
                       osg::Image::NO_DELETE );*/

  std::copy ( tex.begin(), tex.end(), normalMap->data() );
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

  Usul::Scope::CurrentDirectory cwd ( _workingDir );

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

  Usul::Scope::CurrentDirectory cwd ( _workingDir );

  GeodePtr geode ( new osg::Geode );
  GeometryPtr geometry ( new osg::Geometry );

  // load the mesh into a GeneralShaderDocument::Mesh object
  Mesh mesh = this->_loadMesh( filename );


  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
  osg::ref_ptr< osg::Vec3Array > normals ( new osg::Vec3Array );
  osg::ref_ptr< osg::Vec4Array > tangents ( new osg::Vec4Array );
  osg::ref_ptr< osg::Vec2Array > texCoords ( new osg::Vec2Array );
  std::vector< std::vector< osg::Vec3 > > faceNormalsPerVertex;
  std::vector< std::vector< osg::Vec3 > > faceTangentsPerVertex;

  faceNormalsPerVertex.resize( mesh.vertices.size() );
  faceTangentsPerVertex.resize( mesh.vertices.size() );

  //scale all vertices by 10
#if 0
  for( unsigned int i = 0; i < mesh.vertices.size(); ++i )
  {
    mesh.vertices.at( i ) *= 10.0;
  }
#endif
  //Parse mesh object and create a geode
  for( unsigned int i = 0; i < mesh.indices.size(); i += 3 )
  {
    // get the vertices of a triangle in the mesh
    osg::Vec3 v1 = mesh.vertices.at( mesh.indices.at( i + 0 ) ) * 100.0;
    osg::Vec3 v2 = mesh.vertices.at( mesh.indices.at( i + 1 ) ) * 100.0;
    osg::Vec3 v3 = mesh.vertices.at( mesh.indices.at( i + 2 ) ) * 100.0;
    vertices->push_back( v1 );vertices->push_back( v2 );vertices->push_back( v3 );

    // if there are normals provided in the file read and store
    if( mesh.normalIndices.size() > 0 && mesh.normals.size() > 0 )
    {
      osg::Vec3 n1 = mesh.normals.at( mesh.normalIndices.at( i + 0 ) );
      osg::Vec3 n2 = mesh.normals.at( mesh.normalIndices.at( i + 1 ) );
      osg::Vec3 n3 = mesh.normals.at( mesh.normalIndices.at( i + 2 ) );
      normals->push_back( n1 );normals->push_back( n2 );normals->push_back( n3 );

      // create and add tangents for each normal
      //osg::Vec3 tVec = v1 - v2;
      //tangents->push_back( osg::Vec4f( tVec.x(), tVec.y(), tVec.z(), 0.0 ) );
      //tangents->push_back( osg::Vec4f( tVec.x(), tVec.y(), tVec.z(), 0.0 ) );
      //tangents->push_back( osg::Vec4f( tVec.x(), tVec.y(), tVec.z(), 0.0 ) );
    }
    // create the normals if none were provided in the file
    else
    {
     // get the texture coordinates of a triangle in the mesh
      osg::Vec2 t1 ( 0.0, 0.0 );
      osg::Vec2 t2 ( 1.0, 0.0 );
      osg::Vec2 t3 ( 1.0, 1.0 );

      if( mesh.texCoords.size() > 0 && mesh.texIndices.size() > 0 )
      {
        t1 = mesh.texCoords.at( mesh.texIndices.at( i + 0 ) );
        t2 = mesh.texCoords.at( mesh.texIndices.at( i + 1 ) );
        t3 = mesh.texCoords.at( mesh.texIndices.at( i + 2 ) );
        texCoords->push_back( t1 );texCoords->push_back( t2 );texCoords->push_back( t3 );
      }

      FaceNormal fn;
      fn.vi1 = mesh.indices.at( i + 0 );
      fn.vi2 = mesh.indices.at( i + 1 );
      fn.vi3 = mesh.indices.at( i + 2 );

#if 0
      fn.normal = ( v3 - v2 ) ^ ( v1 - v2 );
      fn.vi1 = mesh.indices.at( i + 0 );
      fn.vi2 = mesh.indices.at( i + 1 );
      fn.vi3 = mesh.indices.at( i + 2 );
      faceNormalsPerVertex.at( fn.vi1 ).push_back( fn.normal );
      faceNormalsPerVertex.at( fn.vi2 ).push_back( fn.normal );
      faceNormalsPerVertex.at( fn.vi3 ).push_back( fn.normal );
#endif
      /// tangent calculations per face
#if 0
      float x1 = v2.x() - v1.x();
      float x2 = v3.x() - v1.x();
      float y1 = v2.y() - v1.y();
      float y2 = v3.y() - v1.y();
      float z1 = v2.z() - v1.z();
      float z2 = v3.z() - v1.z();

      float s1 = t2.x() - t1.x();
      float s2 = t3.x() - t1.x();
      float tn1 = t2.y() - t1.y();
      float tn2 = t3.y() - t1.y();

      float r = 1.0f / ( s1 * tn2 - s2 * tn1 );
      osg::Vec3 sdir( ( tn2 * x1 - tn1 * x2 ) * r, ( tn2 * y1 - tn1 * y2 ) * r, ( tn2 * z1 - tn1 * z2 ) * r );
      osg::Vec3 tdir( ( s1 * x2 - s2 * x1 ) * r, ( s1 * y2 - s2 * y1) * r, ( s1 * z2 - s2 * z1 ) * r );

      //// Gram-Schmidt orthogonalize
      //tangent[a] = (t - n * Dot(n, t)).Normalize();
      //
      //// Calculate handedness
      //tangent[a].w = (Dot(Cross(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;

      Usul::Math::Vec3f t ( sdir.x(), sdir.y(), sdir.z() );
      Usul::Math::Vec3f n ( fn.normal.x(), fn.normal.y(), fn.normal.z() );
      Usul::Math::Vec3f temp_tan = ( t - n * n.dot( t ) );
      temp_tan.normalize();

      osg::Vec3 tangent ( temp_tan[0], temp_tan[1], temp_tan[2] );
      faceTangentsPerVertex.at( fn.vi1 ).push_back( tangent );
      faceTangentsPerVertex.at( fn.vi2 ).push_back( tangent );
      faceTangentsPerVertex.at( fn.vi3 ).push_back( tangent );
#else
      Usul::Math::Vec3f xst0 ( v1.x(), t1.x(), t1.y() );
      Usul::Math::Vec3f xst1 ( v2.x(), t2.x(), t2.y() );
      Usul::Math::Vec3f xst2 ( v3.x(), t3.x(), t3.y() );

      Usul::Math::Vec3f yst0 ( v1.y(), t1.x(), t1.y() );
      Usul::Math::Vec3f yst1 ( v2.y(), t2.x(), t2.y() );
      Usul::Math::Vec3f yst2 ( v3.y(), t3.x(), t3.y() );

      Usul::Math::Vec3f zst0 ( v1.z(), t1.x(), t1.y() );
      Usul::Math::Vec3f zst1 ( v2.z(), t2.x(), t2.y() );
      Usul::Math::Vec3f zst2 ( v3.z(), t3.x(), t3.y() );

      Usul::Math::Vec3f xst01 ( xst0 - xst1 );
      Usul::Math::Vec3f xst02 ( xst0 - xst2 );

      Usul::Math::Vec3f yst01 ( yst0 - yst1 );
      Usul::Math::Vec3f yst02 ( yst0 - yst2 );

      Usul::Math::Vec3f zst01 ( zst0 - zst1 );
      Usul::Math::Vec3f zst02 ( zst0 - zst2 );

      Usul::Math::Vec3f ABC0 ( xst01.cross( xst02 ) );
      Usul::Math::Vec3f ABC1 ( yst01.cross( yst02 ) );
      Usul::Math::Vec3f ABC2 ( zst01.cross( zst02 ) );

      Usul::Math::Vec3f ut ( - ( ABC0[1] / ABC0[0] ),
                             - ( ABC1[1] / ABC1[0] ),
                             - ( ABC2[1] / ABC2[0] ) );
      Usul::Math::Vec3f ub ( - ( ABC0[2] / ABC0[0] ),
                             - ( ABC1[2] / ABC1[0] ),
                             - ( ABC2[2] / ABC2[0] ) );

      osg::Vec3f tangent ( ut[0], ut[1], ut[2] );

      faceTangentsPerVertex.at( fn.vi1 ).push_back( tangent );
      faceTangentsPerVertex.at( fn.vi2 ).push_back( tangent );
      faceTangentsPerVertex.at( fn.vi3 ).push_back( tangent );

      Usul::Math::Vec3f tn ( ut.cross( ub ) );

      fn.normal = osg::Vec3f( tn[0], tn[1], tn[2] );

      faceNormalsPerVertex.at( fn.vi1 ).push_back( fn.normal );
      faceNormalsPerVertex.at( fn.vi2 ).push_back( fn.normal );
      faceNormalsPerVertex.at( fn.vi3 ).push_back( fn.normal );


#endif

      /// ---------------------------------------------------------------
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
#if 0
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
            osg::Vec3 tangent ( 0.0, 0.0, 0.0 );
            for( unsigned int j = 0; j < faceNormalsPerVertex.at( i ).size(); ++j )
            {
              normal += faceNormalsPerVertex.at( i ).at( j );
              tangent += faceTangentsPerVertex.at( i ).at( j );
            }
            normal.normalize();
            perVertexNormals->push_back( normal );

            tangent.normalize();
            perVertexTangents->push_back( osg::Vec4( ( tangent.x() + 1.0 ) * 0.5, ( tangent.y() + 1.0 ) * 0.5, ( tangent.z() + 1.0 ) * 0.5, 0.0 ) );
            break;
          };
      };
#else
      osg::Vec3 normal ( 0.0, 0.0, 0.0 );
      osg::Vec3 tangent ( 0.0, 0.0, 0.0 );
      for( unsigned int j = 0; j < faceNormalsPerVertex.at( i ).size(); ++j )
      {
        normal += faceNormalsPerVertex.at( i ).at( j );
        tangent += faceTangentsPerVertex.at( i ).at( j );
      }
      normal.normalize();
      perVertexNormals->push_back( normal );

      tangent.normalize();
      perVertexTangents->push_back( osg::Vec4( ( tangent.x() * 0.5 ) + 0.5, ( tangent.y() * 0.5 ) + 0.5, ( tangent.z() * 0.5 ) + 0.5, 0.0 ) );

#endif
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
