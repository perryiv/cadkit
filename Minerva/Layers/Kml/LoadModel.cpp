
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/Kml/LoadModel.h"
#include "Minerva/Layers/Kml/Download.h"

#include "OsgTools/Visitor.h"
#include "OsgTools/State/StateSet.h"

#include "Usul/Convert/Convert.h"
#include "Usul/File/Path.h"
#include "Usul/Strings/Case.h"

#include "osgDB/ReadFile"

#include "dae.h"
#include "dom/domCOLLADA.h"

using namespace Minerva::Layers::Kml;


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
          if ( osg::Texture* texture = dynamic_cast<osg::Texture*> ( ss->getTextureAttribute ( 0, osg::StateAttribute::TEXTURE ) ) )
          {
            texture->setResizeNonPowerOfTwoHint ( false );
            texture->setFilter ( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
            texture->setFilter ( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
            
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

osg::Node* LoadModel::operator() ( const std::string& filename )
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( filename ) ) );

  // If it's a collada file, pre-process.  (dae stands for Digital Asset Exchange.)
  if ( "dae" == ext )
  {
    this->_preProcessCollada ( filename );
  }

  osg::ref_ptr<osg::Node> node ( osgDB::readNodeFile ( filename ) );

  if ( node.valid() )
  {
    // Post-process.
    Detail::ModelPostProcess nv;
    osg::ref_ptr<osg::NodeVisitor> visitor ( OsgTools::MakeVisitor<osg::Node>::make ( nv ) );
    node->accept ( *visitor );
    
    osg::ref_ptr<osg::StateSet> ss ( node->getOrCreateStateSet() );
    
    //OsgTools::State::StateSet::setTwoSidedLighting ( ss.get(), true );
    OsgTools::State::StateSet::setNormalize ( ss.get(), true );
  }

  return node.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Preprocess the file.
//
///////////////////////////////////////////////////////////////////////////////

void LoadModel::_preProcessCollada ( const std::string& filename )
{
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
      const std::string protocol ( image->getInit_from()->getValue().getProtocol() );
      if ( "http" == protocol )
      {
        const std::string uri ( image->getInit_from()->getValue().str() );
        const std::string filename ( "file:///" + Minerva::Layers::Kml::download ( uri ) );
        image->getInit_from()->getValue().set ( filename );
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
