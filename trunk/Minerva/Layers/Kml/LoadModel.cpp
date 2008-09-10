
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/Kml/LoadModel.h"
#include "Minerva/Core/Utilities/Download.h"
#include "Minerva/Core/Data/ModelCache.h"

#include "OsgTools/Visitor.h"
#include "OsgTools/State/StateSet.h"

#include "Usul/Convert/Convert.h"
#include "Usul/File/Path.h"
#include "Usul/Strings/Case.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"

#include "osg/BlendFunc"
#include "osg/TexEnvCombine"

#include "osgDB/ReadFile"

#include "dae.h"
#include "dom/domCOLLADA.h"

using namespace Minerva::Layers::Kml;


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef Usul::Threads::Mutex Mutex;
typedef Usul::Threads::Guard<Mutex> Guard;


///////////////////////////////////////////////////////////////////////////////
//
//  Mutex to guard reading of collada files.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  Mutex _readMutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LoadModel::LoadModel() : _toMeters ( 0.0254 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Turn off non-power of two resizing.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct ModelPostProcess
  {
    void operator () ( osg::Node * node )
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
            texture->setResizeNonPowerOfTwoHint ( true );
            
            texture->setInternalFormatMode ( osg::Texture::USE_S3TC_DXT1_COMPRESSION );
            
            //texture->setFilter ( osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_NEAREST );
            //texture->setFilter ( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
            
            //texture->setWrap ( osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE );
            //texture->setWrap ( osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE );
            
            //texture->setUseHardwareMipMapGeneration ( false );
#endif
            
            // Turn off lighting.
            OsgTools::State::StateSet::setLighting ( ss.get(), false );
          }
        }
      }
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the model.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* LoadModel::operator() ( const std::string& filename, ModelCache *cache )
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( filename ) ) );

  // If it's a collada file, pre-process.  (dae stands for Digital Asset Exchange.)
  if ( "dae" == ext )
  {
    this->_preProcessCollada ( filename );
  }
  
  if ( 0x0 != cache && cache->hasModel ( filename ) )
    return cache->model ( filename );

  Guard guard ( Detail::_readMutex );
  osg::ref_ptr<osg::Node> node ( osgDB::readNodeFile ( filename ) );
  if ( node.valid() )
  {
    // Post-process.
    Detail::ModelPostProcess nv;
    osg::ref_ptr<osg::NodeVisitor> visitor ( OsgTools::MakeVisitor<osg::Node>::make ( nv ) );
    node->accept ( *visitor );
    
    osg::ref_ptr<osg::StateSet> ss ( node->getOrCreateStateSet() );
    
    OsgTools::State::StateSet::setTwoSidedLighting ( ss.get(), true );
    //OsgTools::State::StateSet::setNormalize ( ss.get(), true );
  }
  
  if ( 0x0 != cache )
    cache->addModel ( filename, node.get() );

  return node.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Preprocess the file.
//
///////////////////////////////////////////////////////////////////////////////

void LoadModel::_preProcessCollada ( const std::string& filename )
{
  Guard guard ( Detail::_readMutex );
  
  DAE dae;

  // Open the file.
  daeSmartRef<domCOLLADA> dom ( dae.open ( filename ) );

  typedef std::vector<daeElement*> Elements;
  
  // Get all the images.
  Elements elements ( dae.getDatabase()->typeLookup ( domImage::ID() ) );

  for ( Elements::iterator iter = elements.begin(); iter != elements.end(); ++iter )
  {
    daeSmartRef<domImage> image ( daeSafeCast<domImage> ( *iter ) );
    if ( 0x0 != image )
    {
      if ( 0x0 != image->getInit_from() )
      {
        const std::string protocol ( image->getInit_from()->getValue().getProtocol() );
        if ( "http" == protocol )
        {
          const std::string uri ( image->getInit_from()->getValue().str() );
          std::string filename;
          if ( Minerva::Core::Utilities::download ( uri, filename, true ) )
          {
            filename = "file:" + cdom::nativePathToUri ( filename );
            image->getInit_from()->getValue().set ( filename );
          }
        }
      }
    }
  }

  // Get the asset.
  domAssetRef asset ( dom->getAsset() );
  if ( 0x0 != asset )
  {
    domAsset::domUnitRef unit ( asset->getUnit() );
    if ( 0x0 != unit )
    {
      this->toMeters ( unit->getMeter() );
    }
  }

  dae.write ( filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the to meters conversion.
//
///////////////////////////////////////////////////////////////////////////////

void LoadModel::toMeters ( double amount )
{
  _toMeters = amount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the to meters conversion.
//
///////////////////////////////////////////////////////////////////////////////

double LoadModel::toMeters() const
{
  return _toMeters;
}
