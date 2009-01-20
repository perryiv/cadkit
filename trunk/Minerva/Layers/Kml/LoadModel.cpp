
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/Kml/LoadModel.h"
#include "Minerva/Layers/Kml/ModelPostProcess.h"
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
#include "osg/Material" 
#include "osg/TexEnvCombine"

#include "osgDB/ReadFile"

#include "osgUtil/Optimizer"

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
//  Scale and normalize.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct ScaleAndNormalize
  {
    ScaleAndNormalize ( double scale ) : _scale ( scale ), _vertices()
    {
    }
    
    void scale ( osg::Geometry& geometry )
    {
      osg::ref_ptr<osg::Vec3Array> vertices ( dynamic_cast<osg::Vec3Array*> ( geometry.getVertexArray() ) );
      osg::ref_ptr<osg::Vec3Array> normals  ( dynamic_cast<osg::Vec3Array*> ( geometry.getNormalArray() ) );
      
      if ( true == vertices.valid() && 1.0 != _scale && ( _vertices.end() == _vertices.find ( vertices ) ) )
      {
        if ( 2 < vertices->referenceCount() )
        {
          vertices = new osg::Vec3Array ( vertices->begin(), vertices->end() );
          geometry.setVertexArray ( vertices.get() );
        }

        osg::Matrixd m ( osg::Matrixd::scale ( osg::Vec3d ( _scale, _scale, _scale ) ) );
        for ( osg::Vec3Array::iterator iter = vertices->begin(); iter != vertices->end(); ++iter )
        {
          //osg::Vec3Array::reference v ( *iter );
          //v = m.preMult ( v );
          (*iter) *= _scale;
        }

        // Add to the set so we don't move the vertices again.
        _vertices.insert ( vertices );
      }
      
      if ( true == normals.valid() )
      {
        for ( osg::Vec3Array::iterator iter = normals->begin(); iter != normals->end(); ++iter )
        {
          (*iter).normalize();
        }
      }
      
      geometry.dirtyBound();
      geometry.dirtyDisplayList();
    }
    
    void operator() ( osg::Geode * geode )
    {
      if ( 0x0 != geode )
      {
#if 1
        if ( "BackColor" == geode->getName() )
        {
          geode->removeDrawables ( 0, geode->getNumDrawables() );
          return;
        }
#endif
        
#if 0
        unsigned int num ( geode->getNumDrawables() );
        for ( unsigned int i = 0; i < num; ++i )
        {
          osg::ref_ptr<osg::Drawable> drawable ( geode->getDrawable ( i ) );
          if ( osg::Geometry* geometry = dynamic_cast<osg::Geometry*> ( drawable.get() ) )
          {
            this->scale ( *geometry );
          }
        }
#endif
      }
    }
    
  private:
    typedef osg::ref_ptr<osg::Vec3Array> VerticesPtr;
    typedef std::set<VerticesPtr> VerticesProcessed;
    
    double _scale;
    
    VerticesProcessed _vertices;
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
    {
      ModelPostProcess nv;
      osg::ref_ptr<osg::NodeVisitor> visitor ( OsgTools::MakeVisitor<osg::Node>::make ( nv ) );
      node->accept ( *visitor );
    }
#if 1
    // Flatten static transforms
    {
      //osgUtil::Optimizer optimizer;
      //optimizer.optimize ( node.get(), osgUtil::Optimizer::FLATTEN_STATIC_TRANSFORMS );
    }

    {
      Detail::ScaleAndNormalize nv ( _toMeters );
      osg::ref_ptr<osg::NodeVisitor> visitor ( OsgTools::MakeVisitor<osg::Geode>::make ( nv ) );
      node->accept ( *visitor );

      //_toMeters = 1.0;
    }
#endif
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
  // Make sure only one thread reads at a time.
  Guard guard ( Detail::_readMutex );
  
  DAE dae;

  // Open the file.
  daeSmartRef<domCOLLADA> dom ( dae.open ( filename ) );

  // Return if the file was not opened.
  if ( 0x0 == dom )
    return;

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
