#ifndef _menukit_osg_word_h_
#define _menukit_osg_word_h_

namespace MenuKit
{

  namespace OSG
  {

    class Word : public DrawableFunctor
    {
    public:
      Word(const std::string& t,osgText::Font* f): DrawableFunctor(),
                                                  _text(t), _font(f) {}
      ~Word() {}

      virtual osg::Drawable* operator() ();

      void font(osgText::Font* f) { _font = f; }
      osgText::Font* font() { return _font.get(); }
      const osgText::Font* font() const { return _font.get(); }

      void text(const std::string& s) { _text = s; }
      const std::string& text() const { return _text; }

    private:
      std::string _text;
      osg::ref_ptr<osgText::Font> _font;
    };

  };

};

#endif
