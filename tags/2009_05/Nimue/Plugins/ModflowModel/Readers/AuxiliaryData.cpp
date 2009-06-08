
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Auxiliary data reader.
//
///////////////////////////////////////////////////////////////////////////////

#include "ModflowModel/Readers/AuxiliaryData.h"
#include "ModflowModel/Attributes/AuxiliaryData.h"
#include "ModflowModel/Constants.h"
#include "ModflowModel/ModflowDocument.h"

#include "OsgTools/Callbacks/SortBackToFront.h"
#include "OsgTools/DisplayLists.h"
#include "OsgTools/State/StateSet.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Documents/Manager.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Math/Constants.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Registry/Constants.h"
#include "Usul/Registry/Database.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Strings/Convert.h"
#include "Usul/Strings/Split.h"
#include "Usul/System/Directory.h"

#include "osgDB/ReadFile"

#include "osg/Image"
#include "osg/MatrixTransform"
#include "osg/Material"
#include "osg/ref_ptr"
#include "osg/Texture2D"
#include "osg/Point"
#include "osg/Geometry"

#include "ogr_spatialref.h"

#include <stack>

using namespace Modflow::Readers;

USUL_IMPLEMENT_TYPE_ID ( AuxiliaryData );

///////////////////////////////////////////////////////////////////////////////
//
//  Transform vertices.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  class TransformVertices : public osg::NodeVisitor
  {
  public:
    typedef osg::NodeVisitor BaseClass;
    typedef std::stack<osg::Matrix> MatrixStack;

    TransformVertices ( OGRCoordinateTransformation* transform ) : 
      BaseClass ( BaseClass::TRAVERSE_ALL_CHILDREN ),
      _transform ( transform ),
      _stack()
    {
      // Always hit every node.
      const unsigned int all ( 0xffffffff );
      this->setTraversalMask    ( all );
      this->setNodeMaskOverride ( all );
    }

    void apply ( osg::MatrixTransform& mt )
    {
      osg::Matrix m ( mt.getMatrix() );
      osg::Matrix top ( ( false == _stack.empty() ) ? _stack.top() : osg::Matrix::identity() );

      top.postMult ( m );

      _stack.push ( m );

      mt.traverse ( *this );

      _stack.pop();
      mt.setMatrix ( osg::Matrix::identity() );
    }

    void apply ( osg::Geode& geode )
    {
      osg::Matrix top ( ( false == _stack.empty() ) ? _stack.top() : osg::Matrix::identity() );

      typedef osg::Geode::DrawableList Drawables;
      Drawables drawables ( geode.getDrawableList() );
      
      for ( Drawables::iterator iter = drawables.begin(); iter != drawables.end(); ++iter )
      {
        if ( iter->valid() )
        {
          // See if it's a geometry.
          osg::ref_ptr<osg::Geometry> geometry ( (*iter)->asGeometry() );
          if ( geometry.valid() )
          {
            // Transform the vertices.
            osg::ref_ptr<osg::Vec3Array> vertices ( dynamic_cast<osg::Vec3Array*> ( geometry->getVertexArray() ) );
            if ( vertices.valid() )
            {
              typedef osg::Vec3f::value_type ValueType;
              
              for ( osg::Vec3Array::iterator vertexIter = vertices->begin(); vertexIter != vertices->end(); ++vertexIter )
              {
                osg::Vec3Array::reference v ( *vertexIter );
                v = top.preMult ( v );

                double x ( v[0] ), y ( v[1] ), z ( v[2] );
                
                if ( 0x0 != _transform && TRUE == _transform->Transform ( 1, &x, &y, &z ) )
                {
                  v.set ( static_cast<ValueType> ( x ), static_cast<ValueType> ( y ), static_cast<ValueType> ( z ) );
                }
              }
            }

            // Transform the normals.
            osg::ref_ptr<osg::Vec3Array> normals ( dynamic_cast<osg::Vec3Array*> ( geometry->getNormalArray() ) );
            if ( normals.valid() )
            {
              for ( osg::Vec3Array::iterator nIter = normals->begin(); nIter != normals->end(); ++nIter )
              {
                osg::Vec3Array::reference n ( *nIter );
                n = osg::Matrix::transform3x3 ( n, top );
                //n.normalize();
              }
            }
          }
        }
      }
    }

  protected:
    virtual ~TransformVertices()
    {
      _transform = 0x0;
    }
    
  private:
    OGRCoordinateTransformation* _transform;
    MatrixStack _stack;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

AuxiliaryData::AuxiliaryData() : BaseClass()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

AuxiliaryData::~AuxiliaryData()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void AuxiliaryData::read ( Modflow::ModflowDocument *doc, const std::string &file, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCallV1V2R3 ( Usul::Adaptors::memberFunction ( this, &AuxiliaryData::_read ), doc, progress, file, "1775994232" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void AuxiliaryData::_read ( Modflow::ModflowDocument *doc, Unknown *progress, const std::string &file )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Ignore empty strings and null documents.
  if ( true == file.empty() || 0x0 == doc )
    return;

  std::cout << "Reading: " << file << std::endl;

  // Set the members.
  this->_set ( doc, progress );

  // Make the transform.
  osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );

  // Get needed information for the filename.
  Usul::Documents::Manager::DocumentInfo info ( Usul::Documents::Manager::instance().find ( file, 0x0 ) );

  // Make sure a document was found.
  if ( false == info.document.valid() )
    throw std::runtime_error ( Usul::Strings::format ( "Error 2638662249: failed to find document for auxiliary file: ", Usul::File::fullPath ( file ) ) );

  // Open the document.
  info.document->open ( file, 0x0, progress );

  // Build the scene.
  Usul::Interfaces::IBuildScene::QueryPtr build ( info.document );
  osg::ref_ptr<osg::Node> node ( ( true == build.valid() ) ? build->buildScene ( Usul::Interfaces::IBuildScene::Options(), progress ) : 0x0 );
  if ( false == node.valid() )
    throw std::runtime_error ( Usul::Strings::format ( "Error 3368131465: failed to build scene for auxiliary file: ", Usul::File::fullPath ( file ) ) );

  // Root of this sub-graph.
  osg::ref_ptr<osg::Group> root ( new osg::Group );
  root->addChild ( mt.get() );
  
  // Add the node to the transform.
  mt->addChild ( node.get() );

  // Set the label for this data.
  const std::string name ( this->fileAttribute ( Modflow::Attributes::LABEL ) );
  this->name ( ( true == name.empty() ) ? Usul::File::base ( file ) : name );

  // Are we supposed to rotate?
  {
    typedef std::vector<std::string> Rotation;
    Rotation rotation;
    Usul::Strings::split ( this->fileAttribute ( Modflow::Attributes::ROTATE ), ' ', false, rotation );
    if ( 4 == rotation.size() )
    {
      const double a ( Usul::Strings::fromString<double> ( rotation.at(0) ) );
      const double x ( Usul::Strings::fromString<double> ( rotation.at(1) ) );
      const double y ( Usul::Strings::fromString<double> ( rotation.at(2) ) );
      const double z ( Usul::Strings::fromString<double> ( rotation.at(3) ) );
      mt->postMult ( osg::Matrixd::rotate ( Usul::Math::DEG_TO_RAD * a, x, y, z ) );
    }
  }

  // Are we supposed to translate?
  {
    typedef std::vector<std::string> Translation;
    Translation translation;
    Usul::Strings::split ( this->fileAttribute ( Modflow::Attributes::TRANSLATE ), ' ', false, translation );
    if ( 3 == translation.size() )
    {
      const double x ( Usul::Strings::fromString<double> ( translation.at(0) ) );
      const double y ( Usul::Strings::fromString<double> ( translation.at(1) ) );
      const double z ( Usul::Strings::fromString<double> ( translation.at(2) ) );
      mt->postMult ( osg::Matrixd::translate ( x, y, z ) );
    }
  }

  // Are we supposed to add color?
  {
    typedef std::vector<std::string> Color;
    Color color;
    Usul::Strings::split ( this->fileAttribute ( Modflow::Attributes::COLOR ), ' ', false, color );
    if ( 4 == color.size() )
    {
      osg::Vec4 c ( ( Usul::Strings::fromString<double> ( color.at(0) ) / 255.0 ),
                    ( Usul::Strings::fromString<double> ( color.at(1) ) / 255.0 ),
                    ( Usul::Strings::fromString<double> ( color.at(2) ) / 255.0 ),
                    ( Usul::Strings::fromString<double> ( color.at(3) ) / 255.0 ) );
      osg::ref_ptr<osg::Material> mat ( new osg::Material );
      OsgTools::State::StateSet::setMaterial ( mt.get(), c, c, c[3] );

      // Are we supposed to sort triangles?
      if ( this->fileAttribute ( Modflow::Attributes::SORT_TRIANGLES ) == "true" )
      {
        OsgTools::DisplayLists displayLists ( false );
        displayLists ( mt.get() );
        osg::ref_ptr< OsgTools::Callbacks::SetSortToFrontCallback > visitor ( new OsgTools::Callbacks::SetSortToFrontCallback );
        mt->accept ( *visitor );
      }
    }
  }

  // Are we supposed to add textures?
  {
    typedef std::vector<std::string> Images;
    Images images;
    const std::string file ( this->fileAttribute ( Modflow::Attributes::TEXTURE_IMAGES ) );
    if ( true == Usul::Predicates::FileExists::test ( file ) )
    {
      // Read the image.
      osg::ref_ptr<osg::Image> image ( osgDB::readImageFile ( file ) );
      if ( true == image.valid() )
      {
        // Create the texture.
        osg::ref_ptr<osg::Texture2D> texture ( new osg::Texture2D );

        // Set the texture's state.
        texture->setImage ( image.get() );
        texture->setFilter ( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
        texture->setFilter ( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
        texture->setWrap ( osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE );
        texture->setWrap ( osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE );

        // Get the state set.
        osg::ref_ptr< osg::StateSet > ss ( root->getOrCreateStateSet() );
        ss->setTextureAttributeAndModes ( 0, texture.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );

        // Turn off lighting.
        OsgTools::State::StateSet::setLighting ( root.get(), false );
      }
    }
  }

  // Add two-sided lighting.
  OsgTools::State::StateSet::setTwoSidedLighting ( root.get(), true );

  // Get registry path.
  typedef Usul::Registry::Node::Path RegPath;
  RegPath regPath ( this->registryPath() );

  // Are we visible?
  bool visible ( true );
  {
    // File attribute overrides registry.
    const std::string temp ( this->fileAttribute ( "visible" ) );
    if ( true == temp.empty() )
    {
      Usul::Registry::Node &reg ( Usul::Registry::Database::instance()[regPath]["visible"] );
      visible = reg.get<bool> ( visible );
    }
    else
    {
      visible = Usul::Strings::fromString<bool> ( temp );
    }
  }
  
  // Look for point size.
  {
    const std::string size ( this->fileAttribute ( Modflow::Attributes::POINT_SIZE ) );
    if ( false == size.empty() )
    {
      osg::ref_ptr < osg::Point > ps ( new osg::Point );
      ps->setSize( Usul::Convert::Type<std::string,float>::convert ( size ) );
      osg::ref_ptr < osg::StateSet > ss ( root->getOrCreateStateSet() );
      ss->setAttributeAndModes( ps.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );
    }
  }
  
  // Look for line width.
  {
    const std::string width ( this->fileAttribute ( Modflow::Attributes::LINE_WIDTH ) );
    if ( false == width.empty() )
    {
      OsgTools::State::StateSet::setLineWidth ( root.get(), Usul::Convert::Type<std::string,float>::convert ( width ) );
    }
  }
  
  // Look for coordinate system.
  {
    // Make a new spatial reference system.
    std::auto_ptr<OGRSpatialReference> source ( this->_coordinateSystem() );
    std::auto_ptr<OGRSpatialReference> destination ( 0x0 != doc->destinationCoordinateSystem() ? doc->destinationCoordinateSystem()->Clone() : 0x0 );
    if ( 0x0 != source.get() && 0x0 != destination.get() )
    {
      std::auto_ptr<OGRCoordinateTransformation> transform ( ::OGRCreateCoordinateTransformation ( source.get(), destination.get() ) );
      
      if ( 0x0 != transform.get() )
      {
        // Visit the scene.
        osg::ref_ptr<Detail::TransformVertices> visitor ( new Detail::TransformVertices ( transform.get() ) );
        root->accept ( *visitor );      
      }
    }
  }
  
  // Add the attribute.
  {
    Modflow::Attributes::Attribute::RefPtr attribute 
      ( new Modflow::Attributes::AuxiliaryData ( this->name(), Unknown::QueryPtr ( doc ), root.get(), this->registryPath() ) );
    attribute->initVisibleState();
    attribute->registryPath ( regPath );
    attribute->initVisibleState();
    doc->addAttribute ( attribute.get() );
  }
}
