
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Generate a tiled image.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Images/TiledImage.h"

#include "Usul/Interfaces/IProjectionMatrix.h"
#include "Usul/Interfaces/IRender.h"
#include "Usul/Interfaces/IViewport.h"

#include "Usul/Exceptions/Thrower.h"
#include "Usul/Math/Math.h"
#include "Usul/Math/Constants.h"

#include "osg/Image"


///////////////////////////////////////////////////////////////////////////////
//
//  Start of namespace.
//
///////////////////////////////////////////////////////////////////////////////

namespace OsgTools {
namespace Images {


///////////////////////////////////////////////////////////////////////////////
//
//  Make the tiled image. Note: the client should save/restore viewport and 
//  projection state, and render again after calling this.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image *makeTiledImage ( unsigned int height, unsigned int width, Usul::Interfaces::IUnknown *caller )
{
  // Handle bad input.
  if ( 0 == height || 0 == width )
    Usul::Exceptions::Thrower<std::runtime_error> 
      ( "Error 1350089255: invalid height (", height, 
        ") and/or width (", width, ") specified while generating tiled image" );

  // Get needed interface.
  Usul::Interfaces::IProjectionMatrix::ValidQueryPtr projection ( caller );
  Usul::Interfaces::IRender::ValidQueryPtr renderer ( caller );
  Usul::Interfaces::IViewport::ValidQueryPtr vp ( caller );

  // Tile height and width
  const unsigned int tileWidth  ( 256 );
  const unsigned int tileHeight ( 256 );

  // Calculate the number of rows and columns we will need
  const unsigned int numRows ( ( height + tileHeight - 1 ) / tileHeight );
  const unsigned int numCols ( ( width + tileWidth - 1 )   / tileWidth  );

  // Set the current tile
  unsigned int currentTile ( 0 );

  // Make the image
  osg::ref_ptr<osg::Image> image ( new osg::Image );

  // Get current projection settings.
  double fovy ( 0 ), aspect ( 0 ), zNear ( 0 ), zFar ( 0 );
  projection->projectionMatrix ( fovy, aspect, zNear, zFar );

  // Reset the aspect ratio. It is defined by the input.
  aspect = ( static_cast < double > ( width ) ) / height;

  // Make enough space
  image->allocateImage ( width, height, 1, GL_RGB, GL_UNSIGNED_BYTE ); 

  // Calculate constant values.
  const double top     ( zNear * Usul::Math::tan ( fovy * Usul::Math::DEG_TO_RAD ) );
  const double bottom  ( -top );
  const double left    ( bottom * aspect );
  const double right   ( top * aspect );

  // Loop through the tiles.
  while ( true )
  {
    // Begin tile 
    const unsigned int currentRow ( currentTile / numCols );
    const unsigned int currentCol ( currentTile % numCols );
    
    // Current tile height and width. Accounts for tiles at end that are not comlete.
    unsigned int currentTileHeight ( ( currentRow < numRows - 1 ) ? tileHeight : ( height - ( ( numRows - 1 ) * tileHeight ) ) );
    unsigned int currentTileWidth  ( ( currentCol < numCols - 1 ) ? tileWidth  : ( width  - ( ( numCols - 1 ) * tileWidth  ) ) );

    // Set the view port to the tile width and height
    vp->viewport ( 0, 0, currentTileWidth, currentTileHeight );

    // Compute projection parameters
    const double currentLeft   ( left          + ( right - left ) *  ( currentCol * tileWidth ) / width );
    const double currentRight  ( currentLeft   + ( right - left ) *            currentTileWidth / width );
    const double currentBottom ( bottom        + ( top - bottom ) * ( currentRow * tileHeight ) / height );
    const double currentTop    ( currentBottom + ( top - bottom ) *           currentTileHeight / height );

    // Set the new frustum
    projection->projectionMatrix ( currentLeft, currentRight, currentBottom, currentTop, zNear, zFar );
    
    // Render the scene
    renderer->render();

    // Previous values
    GLint prevRowLength ( 0 ), prevSkipRows ( 0 ), prevSkipPixels ( 0 );

    // Save current glPixelStore values
    ::glGetIntegerv ( GL_PACK_ROW_LENGTH,  &prevRowLength  );
    ::glGetIntegerv ( GL_PACK_SKIP_ROWS,   &prevSkipRows   );
    ::glGetIntegerv ( GL_PACK_SKIP_PIXELS, &prevSkipPixels );

    // Calculate position in image buffer to write to
    GLint destX ( currentTileWidth  * currentCol );
    GLint destY ( currentTileHeight * currentRow );

    // Setup pixel store for glReadPixels
    // This makes sure that the buffer is read into the correct spot in the image buffer
    ::glPixelStorei ( GL_PACK_ROW_LENGTH,  width );
    ::glPixelStorei ( GL_PACK_SKIP_ROWS,   destY );
    ::glPixelStorei ( GL_PACK_SKIP_PIXELS, destX );

    // Read the tile into the final image
    ::glReadPixels ( 0, 0, currentTileWidth, currentTileHeight, GL_RGB, GL_UNSIGNED_BYTE, image->data() );

    // Restore previous glPixelStore values
    ::glPixelStorei ( GL_PACK_ROW_LENGTH,  prevRowLength  );
    ::glPixelStorei ( GL_PACK_SKIP_ROWS,   prevSkipRows   );
    ::glPixelStorei ( GL_PACK_SKIP_PIXELS, prevSkipPixels );

    // Go the the next tile
    ++currentTile;

    // Are we done?
    if ( currentTile >= numRows * numCols )
      break;
  }

  // Return the image.
  return image.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  End of namespace.
//
///////////////////////////////////////////////////////////////////////////////

} // namespace Images
} // namespace OsgTools
