
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _menukit_osg_word_h_
#define _menukit_osg_word_h_

#include "DrawableFunctor.h"

#include "osgText/Text"

#include <algorithm>
#include <string>


namespace MenuKit
{
  namespace OSG
  {
    class Word : public DrawableFunctor
    {
    public:
      Word():
        DrawableFunctor(1.0f), _text(), _font(new osgText::Font()), _ar(1.0), _dm(osgText::Text::TEXT) {}

      Word(const Word& w):
        DrawableFunctor(w), _text(w._text), _font(w._font), _ar(w._ar), _dm(w._dm) {}

	  Word(std::string t, osg::ref_ptr<osgText::Font> f):
        DrawableFunctor(1.0f), _text(t), _font(f), _ar(1.0), _dm(osgText::Text::TEXT) {}

      virtual ~Word() {}

      Word& operator =(const Word& w)
      {
        _text = w._text;
        _font = w._font;
        _ar = w._ar;
        _dm = w._dm;
        return *this;
      }

      virtual osg::Drawable* operator() ();

      void aspect_ratio(float a) { _ar = a; }
      float aspect_ratio() const { return _ar; }

      void draw_mode(unsigned int d) { _dm = d; }
      unsigned int draw_mode() const { return _dm; }

      void font(osgText::Font* f)       { _font = f; }
      osgText::Font* font()             { return _font.get(); }
      const osgText::Font* font() const { return _font.get(); }

      void text(const std::string& s) { _text = s; }
      const std::string& text() const { return _text; }

    private:
      std::string _text;
      osg::ref_ptr<osgText::Font> _font;
      float _ar;
      unsigned int _dm;
    };

    inline osg::Drawable* Word::operator() ()
    {
      osgText::Text* text = new osgText::Text();
      text->setDrawMode( _dm );
      text->setAlignment( osgText::Text::LEFT_BASE_LINE );

      #if 1
      std::string s ( _text );
      s = std::string ( s.begin(), std::remove ( s.begin(), s.end(), '&' ) );
      text->setText ( s );
      #else
      text->setText ( _text );
      #endif

      text->setFont( _font.get() );
      text->setColor( color() );
      text->setCharacterSize( height(),_ar );
      return( text );
    }
  };
};

#endif
