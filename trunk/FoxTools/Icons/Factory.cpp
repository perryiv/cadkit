
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Icon factory class.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef __sgi
#define _CPP_CMATH 1
#endif

#include "FoxTools/Icons/Factory.h"
#include "FoxTools/Icons/Images.h"
#include "FoxTools/Functions/App.h"
#include "FoxTools/Headers/BMPIcon.h"
#include "FoxTools/Headers/GIFIcon.h"
#include "FoxTools/Headers/PNGIcon.h"
#include "FoxTools/Headers/FileStream.h"

#include "Usul/File/Path.h"
#include "Usul/CommandLine/Arguments.h"

#include <stdexcept>

#include "Usul/Headers/SStream.h"

#include <iostream>

using namespace FoxTools;
using namespace FoxTools::Icons;


///////////////////////////////////////////////////////////////////////////////
//
//  Static data member(s).
//
///////////////////////////////////////////////////////////////////////////////

Factory *Factory::_instance ( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Macros to shorten the map-insertion lines.
//
///////////////////////////////////////////////////////////////////////////////

#define MAP_INSERT(image_name,filename) \
  _map.insert ( Map::value_type ( ICON_##image_name, filename ) )


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Factory::Factory() : _map()
{
  // Populate the map.
  MAP_INSERT ( OPEN,                    "open.gif"                    );
  MAP_INSERT ( COPY,                    "copy.gif"                    );
  MAP_INSERT ( CUT,                     "cut.gif"                     );
  MAP_INSERT ( DELETE,                  "delete.gif"                  );
  MAP_INSERT ( ERROR,                   "error.gif"                   );
  MAP_INSERT ( FRONT,                   "Front.gif"                   );
  MAP_INSERT ( BACK,                    "Back.gif"                    );
  MAP_INSERT ( TOP,                     "Top.gif"                     );
  MAP_INSERT ( BOTTOM,                  "Bottom.gif"                  );
  MAP_INSERT ( LEFT,                    "Left.gif"                    );
  MAP_INSERT ( RIGHT,                   "Right.gif"                   );
  MAP_INSERT ( INFO,                    "info.gif"                    );
  MAP_INSERT ( NEW,                     "NewDocument.gif"             );
  MAP_INSERT ( PASTE,                   "paste.gif"                   );
  MAP_INSERT ( PRINT,                   "print.gif"                   );
  MAP_INSERT ( QUESTION,                "question.gif"                );
  MAP_INSERT ( SAVE,                    "Save.gif"                    );
  MAP_INSERT ( WARNING,                 "warning.gif"                 );
  MAP_INSERT ( CAMERA,                  "camera.gif"                  );
  MAP_INSERT ( SEEK,                    "seek.gif"                    );
  MAP_INSERT ( TOOL_SELECTION,          "selection.gif"               );
  MAP_INSERT ( TRIANGLE,                "AddTriangle.gif"             );
  MAP_INSERT ( NO_TRIANGLE,             "DeleteTriangle.gif"          );
  MAP_INSERT ( DELETE_CONNECTED,        "deleteconnected.gif"         );
  MAP_INSERT ( KEEP_CONNECTED,          "keepconnected.gif"           );
  MAP_INSERT ( ARROW,                   "arrow.gif"                   );
  MAP_INSERT ( EYE,                     "eye.gif"                     );
  MAP_INSERT ( HAND,                    "hand.gif"                    );
  MAP_INSERT ( HOME,                    "home.gif"                    );
  MAP_INSERT ( POLYGONS_FILLED,         "filled.gif"                  );
  MAP_INSERT ( POLYGONS_WIRE,           "WireFrame.gif"               );
  MAP_INSERT ( POLYGONS_HIDDEN,         "Hidden.gif"                  );
  MAP_INSERT ( POLYGONS_POINTS,         "points.gif"                  );
  MAP_INSERT ( STOP,                    "stop.gif"                    );
  MAP_INSERT ( PAUSE,                   "pause.gif"                   );
  MAP_INSERT ( PLAY,                    "play.gif"                    );
  MAP_INSERT ( PLAY_BACKWARDS,          "PlayReverse.gif"             );
  MAP_INSERT ( FORWARD_ARROW,           "FastForward.gif"             );
  MAP_INSERT ( FORWARD_ARROW_BAR,       "SkipForward.gif"             );
  MAP_INSERT ( BACKWARD_ARROW,          "Rewind.gif"                  );
  MAP_INSERT ( BACKWARD_ARROW_BAR,      "SkipReverse.gif"             );
  MAP_INSERT ( RED_CIRCLE,              "FrameDump.gif"               );
  MAP_INSERT ( RED_X,                   "redx.gif"                    );
  MAP_INSERT ( WRITING,                 "writing.gif"                 );
  MAP_INSERT ( XYZ,                     "xyz.gif"                     );
  MAP_INSERT ( SMALL_DOT,               "small_dot.gif"               );
  MAP_INSERT ( WRITE,                   "write.gif"                   );
  MAP_INSERT ( LOGO,                    "logo.gif"                    );
  MAP_INSERT ( LIGHTING_OFF,            "lighting_off.gif"            );
  MAP_INSERT ( LIGHTING_ON ,            "lighting_on.gif"             );
  MAP_INSERT ( LIGHTING_TWO_SIDED_OFF,  "lighting_two_sides_off.gif"  );
  MAP_INSERT ( LIGHTING_TWO_SIDED_ON,   "lighting_two_sides_on.gif"   );
  MAP_INSERT ( LIGHTNING_BOLT,          "lightning.gif"               );
  MAP_INSERT ( CURVE,                   "curve.gif"                   );
  MAP_INSERT ( FLAT,                    "FacetLight.gif"              );
  MAP_INSERT ( SMOOTH,                  "VertexLight.gif"             );
  MAP_INSERT ( GLASS_BOX,               "Glass_Box.gif"               );
  MAP_INSERT ( BOUND_BOX,               "BoundBox.gif"                );  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Factory::~Factory()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the one instance. TODO, make thread-safe.
//
///////////////////////////////////////////////////////////////////////////////

Factory *Factory::instance()
{
  if ( 0x0 == _instance )
    _instance = new Factory;
  return _instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the icon. Throws if value not found.
//
///////////////////////////////////////////////////////////////////////////////

FX::FXIcon *Factory::icon ( unsigned int id ) const
{
  Map::const_iterator iter = _map.find ( id );

  std::string filename ( iter != _map.end() ? iter->second : "" );

  filename = Usul::CommandLine::Arguments::instance().directory() + "/icons/" + filename;

  // Make the icon.
  FX::FXApp *app ( FoxTools::Functions::application() );

  FX::FXIcon *icon = 0x0;
  
  if ( Usul::File::extension ( filename ) == "gif" )
    icon = new FX::FXGIFIcon ( app );
  else if ( Usul::File::extension ( filename ) == "png" )
    icon = new FX::FXPNGIcon ( app );
  else if ( Usul::File::extension ( filename ) == "bmp" )
    icon = new FX::FXBMPIcon ( app );

  if ( 0x0 != icon )
  {
    FX::FXFileStream stream;
  
    if ( stream.open ( filename.c_str(), FX::FXStreamLoad ) )
    {
      icon->loadPixels( stream );

      return icon;
    }
  }

  std::ostringstream message;
  message << "Error 2351912630, invalid icon path = " << filename;
  std::cout << message << std::endl;

  return 0x0;
}
