#ifndef _menukit_osg_word_h_
#define _menukit_osg_word_h_

#include "DrawableFunctor.h"

#include "osgText/Text"

namespace Detail
{

  class Word : public DrawableFunctor
  {
  public:
    Word():
      DrawableFunctor(1.0f), _text(), _font(0x0), _ar(1.0), _dm(osgText::Text::TEXT)
    {}

    Word(const Word& w):
      DrawableFunctor(w), _text(w._text), _font(w._font), _ar(w._ar), _dm(w._dm)
    {}

    virtual ~Word()
    {}

    virtual osg::Drawable* operator() ();

    void aspect_ratio(float a) { _ar = a; }
    float aspect_ratio() const { return _ar; }

    void draw_mode(unsigned int d) { _dm = d; }
    unsigned int draw_mode() const { return _dm; }

    void font(osgText::Font* f) { _font = f; }
    osgText::Font* font() { return _font.get(); }
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
    text->setAlignment( osgText::Text::LEFT_BOTTOM );
    text->setText( _text );
    text->setFont( _font.get() );
    text->setColor( color() );
    text->setCharacterSize( height(),_ar );
    return( text );
  }

};

#endif
