
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Struct for working with fonts.
//
///////////////////////////////////////////////////////////////////////////////

#include "Font.h"

#include "Usul/Threads/Guard.h"

#include "osg/Version"

#include "osgText/Font"
#include "osgText/Text"

#include "osgDB/Registry"
#include "osgDB/FileUtils"
#include "osgDB/ReadFile"

#include <fstream>

using namespace OsgTools;

///////////////////////////////////////////////////////////////////////////////
//
//  Initialize static data members.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Threads::Mutex Font::_mutex;
osg::ref_ptr<osgText::Font> Font::_font;


#ifdef __APPLE__

///////////////////////////////////////////////////////////////////////////////
//
//  On apple, add to the locations that OSG looks for data files (including font files).
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct AddDataFilePaths
  {
    AddDataFilePaths()
    {
      // Get the existing high level path list.
      osgDB::FilePathList wFilePathList ( osgDB::Registry::instance()->getDataFilePathList() );
    
      // Add additional paths of interest to the path list
      wFilePathList.push_back( "../Resources/fonts" ); // Look in the font directory in the Application bundle
      wFilePathList.push_back( "/Library/Fonts" ); //Look in the Global Fonts
      wFilePathList.push_back( "./fonts" ); //Look for a "fonts" Folder in the working directory
      wFilePathList.push_back( "/System/Library/Fonts" ); //Look in the System Fonts
      
      // re-assign the expanded path list
      osgDB::Registry::instance()->setDataFilePathList( wFilePathList );
    }
    
  } addDataFilePaths;
}

#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Return the font filename.
//
///////////////////////////////////////////////////////////////////////////////

std::string Font::fontfile ( const std::string &name )
{
#ifdef _WIN32
  return name + ".ttf";
#elif __APPLE__
  return std::string ( "/Library/Fonts/" ) + name + std::string ( ".ttf" ); 
#else
  return std::string ( "fonts/" ) + name + std::string ( ".ttf" );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the font file.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  Font::FontPtr readFontFile()
  {
#ifdef __APPLE__
    return osgText::readRefFontFile( "fudd.ttf" );
#else
    return osgText::readRefFontFile( "arial.ttf" );
#endif
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the default font for the platform.
//
///////////////////////////////////////////////////////////////////////////////

Font::FontPtr Font::defaultFont() 
{
  Usul::Threads::Guard<Usul::Threads::Mutex> guard ( _mutex );

  if ( false == _font.valid() )
    _font = Detail::readFontFile();
  
  return _font;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the esimated width for the font and text.
//  Adapted from osgText::Text::computeLastCharacterOnLine
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Font::estimateTextWidth( osgText::Text* text )
{
  if ( 0x0 == text )
    return 0;

#if OPENSCENEGRAPH_MAJOR_VERSION >= 2 && OPENSCENEGRAPH_MINOR_VERSION >= 3
  
  typedef osgText::Text::TextureGlyphQuadMap TextureGlyphQuadMap;
  typedef osgText::Text::GlyphQuads GlyphQuads;
  const TextureGlyphQuadMap &map ( text->getTextureGlyphQuadMap() );

  double width ( 0 );
  unsigned int number ( 0 );
  
  for( TextureGlyphQuadMap::const_iterator iter = map.begin(); iter != map.end(); ++iter )
  {
    const GlyphQuads& glyphquad ( iter->second );
    const GlyphQuads::Coords2& coords2 = glyphquad._coords;

    for( unsigned int i = 0; i < coords2.size(); i += 4 )
    {
      width += coords2[i+2].x() - coords2[i].x();
      ++number;
    }
  }

  if ( number > 0 )
    return static_cast<unsigned int> ( ( width / static_cast<double> ( number ) ) * text->getText().size() );
  
  return 0;

#else
  osg::ref_ptr < osgText::Font > font ( const_cast < osgText::Font *> ( text->getFont() ) );
  
  if( false == font.valid() )
    return 0;
  
  float characterHeight       ( text->getCharacterHeight() );
  float characterAspectRatio  ( text->getCharacterAspectRatio() );

  float hr ( characterHeight/ (float) font->getFontHeight() );
  float wr ( hr / characterAspectRatio );

  const osgText::String string ( text->getText() );

  osgText::String::const_iterator lastChar ( string.begin() );
  unsigned int previous_charcode ( 0 );

  osgText::Text::Layout layout ( text->getLayout() );

  osgText::KerningType kerningType ( text->getKerningType() );

  float width ( 0 );

  for( osgText::String::const_iterator iter = string.begin(); iter != string.end(); ++iter )
  {
    unsigned int charcode ( *iter );

    osgText::Font::Glyph* glyph = font->getGlyph( charcode );
    if ( glyph )
    {
      //float width = (float)( glyph->s() - 2 * font->getGlyphImageMargin() ) * wr;

      if ( layout == osgText::Text::RIGHT_TO_LEFT )
      {
        width -= glyph->getHorizontalAdvance() * wr;
      }

      // adjust cursor position w.r.t any kerning.
      if ( previous_charcode )
      {
        osg::Vec2 delta( font->getKerning( previous_charcode, charcode, kerningType ) );

        switch( layout )
        {
          case osgText::Text::LEFT_TO_RIGHT:
          {  
            width += delta.x() * wr;
            break;
          }
          case osgText::Text::RIGHT_TO_LEFT:
          {
            width -= delta.x() * wr;
            break;
          }
        }
      }

      // move the cursor onto the next character.
      if( layout == osgText::Text::LEFT_TO_RIGHT )
      {
        width += glyph->getHorizontalAdvance() * wr;
      }

      previous_charcode = charcode;
    }
  }

  return static_cast < unsigned int > ( width );
#endif
}
