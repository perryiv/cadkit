#ifndef _menukit_osg_colorthemeskin_h_
#define _menukit_osg_colorthemeskin_h_

#include "ThemeSkin.h"   // base class
#include "BasicTheme.h"  // for tyepdef

#include "../Menu.h"
#include "../Button.h"
// TODO: #include "../Item.h"

#include "osgText/Text"
#include "osg/PrimitiveSet"
#include "osg/Geometry"
#include "osg/Geode"
#include "osg/MatrixTransform"

#include <string>

namespace MenuKit
{

  namespace OSG
  {

    /** ColorThemeSkin
      * An implementation of thye ThemeSkin base class.
      * Assumes the them has 2 fields, back and front color
      */
    template<class ThemeType>
    class ColorThemeSkin : public ThemeSkin<ThemeType>
    {
    public:
      typedef ThemeSkin<ThemeType> BaseClass;

      ColorThemeSkin(): BaseClass(), _picture(0.0,1.0), _margin(0.2), _text(0.8)
      {}

      ColorThemeSkin(const ColorThemeSkin& s): BaseClass(s),
        _picture(s._picture), _margin(s._margin), _text(s._text)
      {}

      ColorThemeSkin& operator= (const ColorThemeSkin& ct)
      {
        BaseClass::operator =(ct);
        _picture = ct._picture;
        _margin = ct._margin;
        _text = ct._text;
        return( *this );
      }

      virtual osg::Node* operator ()(const Menu& m);
      virtual osg::Node* operator ()(const Button& b);
      // TODO: virtual osg::Node* operator =(const Item& b);

      virtual float width(const Menu& m);
      virtual float width(const Button& b);
      // TODO: virtual float width(const Button& b);

      void margin(float m) { _margin=m; }
      float margin() const { return _margin; }

      void picture_box(const Detail::Box& b) { _picture=b; }
      const Detail::Box& picture_box() const { return _picture; }

      void text(float t) { _text=t; }
      float text() const { return _text; }

    private:
      Detail::Box _picture;  // for side graphics
      float _margin;         // distances
      float _text;           // percentages
    };

    typedef ColorThemeSkin<osgColor> BasicSkin;
  };

};

// ------------------------ //
//     HELPER CLASSES
// ------------------------ //

namespace Detail {
class DrawableFunctor
{
public:
  DrawableFunctor(float h=1.0): _color(1.0,0.0,0.0,1.0), _height(h) {}
  ~DrawableFunctor() {}

  void color(const osg::Vec4& c) { _color = c; }
  const osg::Vec4& color() const { return _color; }

  void height(float h) { _height = h; }
  float height() const { return _height; }

  virtual osg::Drawable* operator() ()=0;

private:
  float _height;
  osg::Vec4 _color;
};

class BackgroundBox : public DrawableFunctor
{
public:
  BackgroundBox(float h, float w, float d=-0.001f,
                osg::PrimitiveSet::Mode m=osg::PrimitiveSet::QUADS):
    DrawableFunctor(h), _width(w), _depth(d), _drawmode(m) {}
  ~BackgroundBox() {}

  void width(float w) { _width = w; }
  float width() const { return _width; }

  void depth(float w) { _depth = w; }
  float depth() const { return _depth; }

  void mode(osg::PrimitiveSet::Mode m) { _drawmode = m; }
  osg::PrimitiveSet::Mode move() const { return _drawmode; }

  virtual osg::Drawable* operator() ();

private:
  float _width, _depth;
  osg::PrimitiveSet::Mode _drawmode;
};

class Word : public DrawableFunctor
{
public:
  Word(): DrawableFunctor(), _text(), _font(0x0), _ar(1.0) {}
  Word(float h, const std::string& s, osgText::Font* f,float a=1.0):
    DrawableFunctor(h), _text(s), _font(f), _ar(a) {}
  ~Word() {}

  virtual osg::Drawable* operator() ();

  void aspect_ratio(float a) { _ar = a; }
  float aspect_ratio() const { return _ar; }

  void font(osgText::Font* f) { _font = f; }
  osgText::Font* font() { return _font.get(); }
  const osgText::Font* font() const { return _font.get(); }

  void text(const std::string& s) { _text = s; }
  const std::string& text() const { return _text; }

private:
  std::string _text;
  osg::ref_ptr<osgText::Font> _font;
  float _ar;
};

class Arrow: public DrawableFunctor
{
public:
  Arrow(float h): DrawableFunctor(h) {}
  ~Arrow() {}

  virtual osg::Drawable* operator() ();
};

class Disk: public DrawableFunctor
{
public:
  enum Mode
  {
    HALLOW,
    FILL
  };

  Disk(float h,Mode m=HALLOW,unsigned int p=80,float d=0.0):
    DrawableFunctor(h),_mode(m),_points(p),_depth(d) {}
  ~Disk() {}

  virtual osg::Drawable* operator() ();

  void depth(float d) { _depth = d; }
  float depth() const { return _depth; }

  void mode(Mode m) { _mode = m; }
  Mode mode() const { return _mode; }

  void points(unsigned int p) { _points = p; }
  unsigned int points() const { return _points; }

private:
  Mode _mode;
  unsigned int _points;
  float _depth;
};

};  // end namespace Detail

using namespace Detail;
// ------------------------ //
//  HELPER IMPLEMENTATION
// ------------------------ //
osg::Drawable* Word::operator() ()
{
  osg::ref_ptr<osgText::Text> text = new osgText::Text;
  text->setDrawMode(osgText::Text::TEXT);
  text->setAlignment( osgText::Text::LEFT_BASE_LINE );
  text->setText( _text );
  text->setFont( _font.get() );
  text->setColor( color() );
  text->setCharacterSize( height(),_ar );  // height, AR
  return( text.release() );
}

osg::Drawable* BackgroundBox::operator() ()
{
  float width_2  = 0.5f * _width;
  float height_2 = 0.5f * height();

  osg::Vec3Array* vertices = new osg::Vec3Array;
  vertices->push_back ( osg::Vec3 (  width_2,  height_2, _depth ) );
  vertices->push_back ( osg::Vec3 ( -width_2,  height_2, _depth ) );
  vertices->push_back ( osg::Vec3 ( -width_2, -height_2, _depth ) );
  vertices->push_back ( osg::Vec3 (  width_2, -height_2, _depth ) );

  osg::Vec3Array* normals = new osg::Vec3Array;
  normals->push_back( osg::Vec3(0.0,0.0,1.0) );

  osg::Vec4Array* colors = new osg::Vec4Array;
  colors->push_back( color() );

  osg::ref_ptr<osg::Geometry> patch = new osg::Geometry;
  patch->setVertexArray( vertices );
  patch->setNormalArray( normals );
  patch->setNormalBinding( osg::Geometry::BIND_OVERALL );
  patch->setColorArray( colors );
  patch->setColorBinding( osg::Geometry::BIND_OVERALL );
  patch->addPrimitiveSet( new osg::DrawArrays(_drawmode,
                                              0,vertices->size()) );

  return( patch.release() );
}

osg::Drawable* Arrow::operator () ()
{
  osg::Vec3Array* vertices = new osg::Vec3Array();
  float h_2 = 0.5f * height();
  vertices->push_back( osg::Vec3(0.0, h_2 , 0.0) );
  vertices->push_back( osg::Vec3(0.0,-h_2 , 0.0) );
  vertices->push_back( osg::Vec3(h_2, 0.0 , 0.0) );

  osg::Vec3Array* normals = new osg::Vec3Array();
  normals->push_back( osg::Vec3(0.0,0.0,1.0) );

  osg::Vec4Array* colors = new osg::Vec4Array();
  colors->push_back( color() );

  osg::ref_ptr<osg::Geometry> patch = new osg::Geometry();
  patch->setVertexArray( vertices );
  patch->setNormalArray( normals );
  patch->setNormalBinding( osg::Geometry::BIND_OVERALL );
  patch->setColorArray( colors );
  patch->setColorBinding( osg::Geometry::BIND_OVERALL );
  patch->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,vertices->size()) );

  return( patch.release() );
}

osg::Drawable* Disk::operator() ()
{
  float radius = 0.5*height();

  osg::Vec3Array* vertices = new osg::Vec3Array();

  osg::PrimitiveSet::Mode drawmode;
  switch( _mode )
  {
  case HALLOW:
    {
      drawmode = osg::PrimitiveSet::LINE_LOOP;

      // define the circle
      float da = 2.0*osg::PI / float(_points);
      for(unsigned int i=0; i<_points; i++)
        {
          float angle = float(i)*da;
          float x = radius*cosf(angle);
          float y = radius*sinf(angle);
          vertices->push_back( osg::Vec3(x,y,_depth) );
        } break;
    }

  case FILL:
    {
      drawmode = osg::PrimitiveSet::TRIANGLE_FAN;

      // define the circle
      float da = 2.0*osg::PI / float(_points);
      for(unsigned int i=0; i<_points; i++)
      {
        vertices->push_back( osg::Vec3(0.0,0.0,_depth) );

        float angle = float(i)*da;
        float x = radius*cosf(angle);
        float y = radius*sinf(angle);
        vertices->push_back( osg::Vec3(x,y,_depth) );

        float nextangle;
        if( i==(_points-1) )
          nextangle = 0.0;
        else
          nextangle = float(i+1)*da;
        float nx = radius*cosf(nextangle);
        float ny = radius*sinf(nextangle);
        vertices->push_back( osg::Vec3(nx,ny,_depth) );
      }
    } break;
  }

  osg::Vec4Array* colors = new osg::Vec4Array();
  colors->push_back( color() );

  osg::Vec3Array* normals = new osg::Vec3Array();
  normals->push_back( osg::Vec3(0.0,0.0,1.0) );

  osg::ref_ptr<osg::Geometry> disk = new osg::Geometry();
  disk->setVertexArray( vertices );
  disk->setNormalArray( normals );
  disk->setColorArray( colors );
  disk->setNormalBinding( osg::Geometry::BIND_OVERALL );
  disk->setColorBinding( osg::Geometry::BIND_OVERALL );
  disk->addPrimitiveSet( new osg::DrawArrays(drawmode,0,vertices->size()) );

  return( disk.release() );
}

// ------------------------ //
//   SKIN IMPLEMENTATION
// ------------------------ //

using namespace MenuKit;
using namespace OSG;

template<class ThemeType>
osg::Node* ColorThemeSkin<ThemeType>::operator ()(const Menu& m)
{
  const ThemeType& scheme = this->theme();

  Detail::Word word(_text*box().height(), m.text(), font());
  word.color( scheme.front() );

  Detail::BackgroundBox backbox(box().height(), box().width(), -0.02);
  backbox.color( scheme.front() );

  //osg::ref_ptr<osg::Geode> wordgeode = new osg::Geode();
  //wordgeode->addDrawable( word() );
  osg::ref_ptr<osg::MatrixTransform> wordmt = new osg::MatrixTransform();
  //wordmt->addChild( wordgeode.get() );

  //osg::ref_ptr<osg::Geode> backgeode = new osg::Geode();
  //backgeode->addDrawable( backbox() );
  osg::ref_ptr<osg::MatrixTransform> backmt = new osg::MatrixTransform();
  //backmt->addChild( backgeode.get() );

  osg::ref_ptr<osg::Group> group = new osg::Group();
  group->addChild( backmt.get() );
  group->addChild( wordmt.get() );

  return( group.get() );
}

template<class ThemeType>
osg::Node* ColorThemeSkin<ThemeType>::operator ()(const Button& b)
{
  const ThemeType& scheme = this->theme();

  Detail::Word word(_text*box().height(), b.text(), font());
  word.color( scheme.front() );

  Detail::BackgroundBox backbox(box().height(), box().width(), -0.02);
  backbox.color( scheme.front() );

  osg::ref_ptr<osg::Geode> wordgeode = new osg::Geode();
  wordgeode->addDrawable( word() );
  osg::ref_ptr<osg::MatrixTransform> wordmt = new osg::MatrixTransform();
  wordmt->addChild( wordgeode.get() );

  osg::ref_ptr<osg::Geode> backgeode = new osg::Geode();
  backgeode->addDrawable( backbox() );
  osg::ref_ptr<osg::MatrixTransform> backmt = new osg::MatrixTransform();
  backmt->addChild( backgeode.get() );

  osg::ref_ptr<osg::Group> group = new osg::Group();
  group->addChild( backmt.get() );
  group->addChild( wordmt.get() );

  return( group.get() );
}

template<class ThemeType>
float ColorThemeSkin<ThemeType>::width(const Menu& m)
{
  // equation for graphic width:
  // 2 graphic boxes, 2 x _box's width
  // 1 text box,      1 x  boundingbox's width
  // 2 margins,       2 x _margin
  Word word;
  word.text( m.text() );
  word.font( font() );
  word.height( _text*box().height() );
  osg::ref_ptr<osg::Drawable> d = word();
  osg::BoundingBox bb = d->getBound();
  float word_width = bb.xMax() - bb.xMin();

  float w = 2.0*_picture.width() + word_width + 2.0*_margin;
  return( w );
}

template<class ThemeType>
float ColorThemeSkin<ThemeType>::width(const Button& b)
{
  // equation for graphic width:
  // 2 graphic boxes, 2 x _box's width
  // 1 text box,      1 x  boundingbox's width
  // 2 margins,       2 x _margin
  Word word;
  word.text( b.text() );
  word.font( font() );
  word.height( _text*box().height() );
  osg::ref_ptr<osg::Drawable> d = word();
  osg::BoundingBox bb = d->getBound();
  float word_width = bb.xMax() - bb.xMin();

  float w = 2.0*_picture.width() + word_width + 2.0*_margin;
  return( w );
}

#endif
