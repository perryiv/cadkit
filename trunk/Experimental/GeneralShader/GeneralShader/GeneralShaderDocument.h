
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for mpd files
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERAL_SHADER_DOCUMENT_H_
#define _GENERAL_SHADER_DOCUMENT_H_

#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IAnimatePath.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Interfaces/IMpdNavigator.h"
#include "Usul/Interfaces/IGeneralShader.h"
#include "Usul/Interfaces/IMenuAdd.h"
#include "Usul/Interfaces/IKeyListener.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector4.h"
#include "Usul/Policies/Update.h"

#include "OsgTools/Triangles/TriangleSet.h"

#include "XmlTree/Document.h"

#include "osg/Image"
#include "osg/LineSegment"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/TextureCubeMap"

#include <string>
#include <memory>

namespace osg { class Node; }


class GeneralShaderDocument : public Usul::Documents::Document,
                                  public Usul::Interfaces::IBuildScene,
                                  public Usul::Interfaces::IUpdateListener,
                                  public Usul::Interfaces::IMenuAdd,
                                  public Usul::Interfaces::IGeneralShader,
                                  public Usul::Interfaces::IKeyListener
                                  
{
public:
  
  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef Usul::Documents::Document Document;
  typedef Usul::Documents::Document::RefPtr DocumentPtr;
  typedef Usul::Documents::Manager DocManager;
  typedef DocManager::DocumentInfo Info;
  typedef Usul::Policies::NumberBased UpdatePolicy;
  typedef std::auto_ptr< UpdatePolicy > UpdatePolicyPtr;
  typedef osg::ref_ptr< osg::Group > GroupPtr;
  typedef osg::ref_ptr< osg::Geode > GeodePtr;
  typedef osg::ref_ptr< osg::Geometry > GeometryPtr;
  typedef osg::ref_ptr< osg::Program > ProgramPtr;
  typedef osg::ref_ptr< osg::Image > ImagePtr;
  typedef osg::ref_ptr< osg::Shader > ShaderPtr;
  typedef osg::ref_ptr< osg::Uniform > UniformPtr;
  typedef osg::ref_ptr< osg::PositionAttitudeTransform > PosAttXformPtr;
  typedef osg::ref_ptr< osg::StateSet > StateSetPtr;
  typedef osg::ref_ptr< osg::MatrixTransform > MatrixTransformPtr;
  typedef Usul::Math::Vec2d Vec2d;
  typedef Usul::Math::Vec3d Vec3d;
  typedef Usul::Math::Vec3f Vec3f;
  typedef Usul::Math::Vec4d Color4;
  typedef std::vector< Color4 > ColorTable;
  typedef std::vector< Color4 > Color4Vec;
  typedef std::vector< unsigned char > ColorVec;
  typedef std::vector< unsigned int > Vertices;
  typedef osg::ref_ptr< osg::LineSegment > LineSegmentPtr;
  typedef osg::BoundingBox BoundingBox;
  typedef osg::ref_ptr< osg::MatrixTransform > MatrixTransformPtr;
  typedef XmlTree::Document::Attributes Attributes;
  typedef XmlTree::Document::Children Children;

  // Interface Shortcuts
  typedef Usul::Interfaces::IKeyListener IKeyListener;

  typedef std::vector< PosAttXformPtr > Shaders;
  typedef std::vector< GeodePtr > Models;
  typedef std::map< std::string, unsigned int > IndexList;
  typedef std::vector< std::string > NameList;
  typedef std::vector< UniformPtr > Uniforms;
  typedef std::vector< GroupPtr > SkyBoxes;
 
  // Structs
  struct ShaderGroup
  {
    std::string name;
    std::string vertex;
    std::string fragment;

    unsigned int index;
    PosAttXformPtr shader;
    NameList cubeList;

    bool hasSkyBox;
    unsigned int skyBoxIndex;
    Uniforms uniforms;
    std::string type;
    std::string normalMapPath;
    std::string heightMapPath;
    std::string texturePath;
    osg::ref_ptr< osg::TextureCubeMap > emMap;

    ImagePtr normalMap;
    ImagePtr heightMap;
    ImagePtr texture;
    ImagePtr binormalMap;
    

  };
  typedef std::vector< ShaderGroup > ShaderGroups;

  struct Mesh
  {
    std::vector< osg::Vec3 > vertices;	      // array of vertices
    std::vector< osg::Vec3 > normals;		      // array of normals
	  std::vector< osg::Vec2 > texCoords;	      // array of texture coordinates; assume using 2d textures

	  std::vector< unsigned int > indices;	      // vertex indices
	  std::vector< unsigned int > normalIndices;	// normal indices
	  std::vector< unsigned int > texIndices;	  // texcoord indices
  };
 
  struct FaceNormal
  {
    osg::Vec3 normal;
    unsigned int vi1, vi2, vi3;
  };

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( GeneralShaderDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  GeneralShaderDocument();

  virtual void                addView     ( View   *view   );

  /// Build the scene.
  virtual osg::Node *         buildScene ( const BaseClass::Options &options, Unknown *caller = 0x0 );

  /// Return true if this document can do it.
  virtual bool                canExport ( const std::string &file ) const;
  virtual bool                canInsert ( const std::string &file ) const;
  virtual bool                canOpen   ( const std::string &file ) const;
  virtual bool                canSave  ( const std::string &file ) const;

  /// Clear any existing data.
  virtual void                clear ( Unknown *caller = 0x0 );

  /// Get the filters that correspond to what this document can read and write.
  virtual Filters  filtersOpen()   const;
  virtual Filters  filtersSave()   const;
  virtual Filters  filtersInsert() const;
  virtual Filters  filtersExport() const;
  
  /// Read the document.
  virtual void     read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  /// Write the document to given file name.
  virtual void     write ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0  ) const;

  
  // Usul::Interfaces::IGeneralShader
  // Change the currently displayed model
  virtual void     setModel( const std::string &name );
  virtual void     readSettings( const std::string &workingDir );

  // Change the currently used shader
  virtual void               setShader( const std::string &name );
  virtual std::string        getCurrentModel();
  virtual std::string        getCurrentShader();

  // Usul::Interfaces::IKeyListener
  virtual bool keyPressed ( int code );
  virtual bool keyReleased ( int code );

  
 

protected:

  /// Do not copy.
  GeneralShaderDocument ( const GeneralShaderDocument & );
  GeneralShaderDocument &operator = ( const GeneralShaderDocument & );

  /// Usul::Interfaces::IUpdateListener
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );

  void                        _openDocument ( const std::string &file, Usul::Documents::Document *document, Usul::Interfaces::IUnknown *caller );
  void                        _setStatusBar ( const std::string &text, Usul::Interfaces::IUnknown *caller );

  // Scene Builders
  void                        _buildScene( Unknown *caller );
  osg::Node*                  _buildWindow( Unknown *caller );
    
  //Reads
  void                        _readVertices( const std::string& filename );
  void                        _readColorTable( const std::string& filename );
  void                        _init();
  void                        _setupEnvironment();

  // XML Parsing
  void                        _parseXML( XmlTree::Node &node, Unknown *caller = 0x0, Unknown *progress = 0x0 );
  void                        _parseModels( XmlTree::Node &node, Unknown *caller = 0x0, Unknown *progress = 0x0 );
  void                        _parseSimpleShaders( XmlTree::Node &node, Unknown *caller = 0x0, Unknown *progress = 0x0 );
  void                        _parseEMShaders( XmlTree::Node &node, Unknown *caller = 0x0, Unknown *progress = 0x0 );
  void                        _parseBMShaders( XmlTree::Node &node, Unknown *caller = 0x0, Unknown *progress = 0x0 );
  osg::Uniform*               _parseUniform( XmlTree::Node &node, Unknown *caller = 0x0, Unknown *progress = 0x0 );
  void                        _parseSettings( XmlTree::Node &node, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  // Shader Creation
  void                        _createSimpleShader( unsigned int index, Unknown *caller = 0x0, Unknown *progress = 0x0 );
  void                        _createEMShader( unsigned int index, Unknown *caller = 0x0, Unknown *progress = 0x0 );
  void                        _createBMShader( unsigned int index, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  // Model Creation         
  osg::Geode*                 _createSphereModel();
  osg::Geode*                 _createCubeModel();
  osg::Geode*                 _createTorusModel();
  osg::Geode*                 _createPlaneModel();
  osg::Geode*                 _createMeshModel( const std::string &filename );
  void                        _createSkyBox( unsigned int index );
  Mesh                        _loadMesh( const std::string &filename );

  // Bump Mapping Funcitons
  osg::Image*                 _createNormalMap( const std::string &name, unsigned int index );
  osg::Image*                 _createTangentMap( unsigned int index );
  osg::Image*                 _createBinormalMap( unsigned int index );
  osg::Image*                 _setHeightMap( const std::string &filename );
  char                        _getHeightFromImageAt( unsigned int x, unsigned int y, unsigned int index  );

  // Scene update
  void                        _updateScene();

  //Usul::Interfaces::IMenuAdd
  void                        menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller = 0x0 );

  /// Use reference counting.
  virtual ~GeneralShaderDocument();

private:
  GroupPtr                    _root;
  GroupPtr                    _scene;

  ShaderGroups                _shaderGroups;

  Models                      _models;
  Shaders                     _shaders;

  IndexList                   _modelList;
  IndexList                   _shaderList;

  NameList                    _modelNames;
  NameList                    _shaderNames;

  UniformPtr                  _eyePosition;
  UniformPtr                  _modelMatrixUniform;

  unsigned int                _currentModel;
  unsigned int                _currentShader;

  std::string                 _workingDir;

  osg::Vec3d                  _eye;
  osg::Vec3d                  _light;
  osg::Vec2d                  _theta;
  float                       _radius;
  MatrixTransformPtr          _modelMatrixTransform;     

  ImagePtr                    _heightMapImage;    

  SkyBoxes                    _skyBoxes;

  Uniforms                    _additionalUniforms;

  osg::Vec2                   _nearFarClipPlane;

  float                       _tile;
  float                       _depth;
  osg::Vec3f                  _etaRatio;


    
  
};



#endif // _GENERAL_SHADER_DOCUMENT_H_
