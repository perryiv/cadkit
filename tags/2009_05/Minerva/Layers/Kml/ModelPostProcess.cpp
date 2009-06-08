
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/Kml/ModelPostProcess.h"

#include "OsgTools/State/StateSet.h"

#include "osg/BlendFunc"
#include "osg/Material" 
#include "osg/Node"
#include "osg/Texture"
#include "osg/TexEnvCombine"

using namespace Minerva::Layers::Kml;


///////////////////////////////////////////////////////////////////////////////
//
//  Set texture state for the node.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPostProcess::operator () ( osg::Node * node )
{
  if ( 0x0 != node )
  {
    osg::ref_ptr<osg::StateSet> ss ( node->getOrCreateStateSet() );
    
    if ( ss.valid() )
    {
      // Add a blend function.
      osg::ref_ptr<osg::BlendFunc> blend ( new osg::BlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) );
      ss->setAttributeAndModes ( blend.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );
      
      ss->setRenderingHint ( osg::StateSet::TRANSPARENT_BIN );
      
      osg::ref_ptr<osg::TexEnvCombine> combine ( new osg::TexEnvCombine );
      combine->setCombine_Alpha ( GL_REPLACE );
      ss->setTextureAttributeAndModes ( 0, combine.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
      ss->setMode ( GL_BLEND, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
      
      // Turn off back face culling.
      OsgTools::State::StateSet::setBackFaceCulling ( ss.get(), false );
      
      if ( osg::Texture* texture = dynamic_cast<osg::Texture*> ( ss->getTextureAttribute ( 0, osg::StateAttribute::TEXTURE ) ) )
      {            
#if 0
        texture->setResizeNonPowerOfTwoHint ( false );
        
        texture->setFilter ( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
        texture->setFilter ( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
#else
        // Only turn on compression if the internal format is rgb.
        if ( GL_RGB == texture->getInternalFormat() )
        {
          // Texture compression needs power of two image sizes.
          texture->setResizeNonPowerOfTwoHint ( true );
          
          texture->setInternalFormatMode ( osg::Texture::USE_S3TC_DXT1_COMPRESSION );
        }
        else if ( GL_RGBA == texture->getInternalFormat() )
        {
          ss->setRenderingHint ( osg::StateSet::TRANSPARENT_BIN );
          ss->setMode ( GL_BLEND, osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );
        }
#endif

        // Turn off lighting.
        OsgTools::State::StateSet::setLighting ( ss.get(), false );
      }

#if 0
      // Check for a material.
      if ( osg::Material* material = dynamic_cast<osg::Material*> ( ss->getAttribute ( osg::StateAttribute::MATERIAL ) ) )
      {
        material->setAmbient ( osg::Material::FRONT, material->getDiffuse ( osg::Material::FRONT ) );
      }
      else
      {
        OsgTools::State::StateSet::setMaterialDefault ( node );
      }
#endif
    }
  }
}
