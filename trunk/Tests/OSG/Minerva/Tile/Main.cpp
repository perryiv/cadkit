
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Program to test a Tile
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/TileEngine/Body.h"
#include "Minerva/Core/TileEngine/Tile.h"
#include "Minerva/Core/Functions/MakeBody.h"

#include "Usul/Jobs/Manager.h"
#include "Usul/Threads/Manager.h"

#include "Threads/OpenThreads/Thread.h"

#include "osgViewer/Viewer"
#include "osgGA/StateSetManipulator"

int main ( int argc, char** argv )
{
  // Set thread factories.
  Usul::Threads::Manager::instance().factory ( &Threads::OT::newOpenThreadsThread );

  // Make the viewer.
  osgViewer::Viewer viewer;

  // This allows 'w' -> wireframe.
  viewer.addEventHandler( new osgGA::StateSetManipulator( viewer.getCamera()->getOrCreateStateSet() ) );

  // Make a body.
  Minerva::Core::TileEngine::Body::RefPtr body ( Minerva::Core::Functions::makeEarth ( &Usul::Jobs::Manager::instance() ) );

  // Make a tile.
  typedef Minerva::Core::TileEngine::Tile Tile;
  Tile::RefPtr tile ( new Tile ( 0x0, Tile::NONE, 0, Tile::Extents ( -90, -45, 0, 45 ), Tile::MeshSize ( 17, 17 ), Tile::ImageSize ( 512, 512 ), body->splitDistance(), body.get() ) );
  
  /*tile->split ( 0x0 );

  body->allowSplit ( false );
  body->keepDetail ( true );*/

  // Add the tile to the viewer.
  viewer.setSceneData ( tile.get() );

  // Run the viewer.
  const int result ( viewer.run() );

  // Wait for all threads.
  //Usul::Threads::Manager::instance().wait();

  // return the result.
  return result;
}
