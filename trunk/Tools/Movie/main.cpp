
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Render/FrameDump.h"
#include "OsgTools/Render/OffScreenRenderer.h"

#include "Usul/File/Temp.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/CommandLine/Parser.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Components/Manager.h"

#include "osgDB/ReadFile"

#include <iomanip>

int main ( int argc, char* argv[] )
{
  const unsigned int width ( 800 );
  const unsigned int height ( 800 );

  Usul::CommandLine::Arguments::instance().set( argc, argv );

  // Make the renderer.
  OsgTools::Render::OffScreenRenderer::RefPtr renderer ( new OsgTools::Render::OffScreenRenderer );
  renderer->init ( width, height );

  // Make a frame dump object.
  boost::shared_ptr<OsgTools::Render::FrameDump> fd ( new OsgTools::Render::FrameDump ( true, Usul::File::Temp::directory(), "filename", ".jpg" ) );
  renderer->frameDump ( fd );

  renderer->render();
  
  return 0;
}

