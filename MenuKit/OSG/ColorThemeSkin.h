#ifndef _menukit_osg_colorthemeskin_h_
#define _menukit_osg_colorthemeskin_h_

#include "ThemeSkin.h"   // base class
#include "ColorTheme.h"  // for tyepdef

#include "../Menu.h"
#include "../Button.h"
#include "../Bits.h"
// TODO: #include "../Item.h"

#include "osgText/Text"
#include "osg/PrimitiveSet"
#include "osg/Geometry"
#include "osg/Geode"
#include "osg/MatrixTransform"
#include "osg/BoundingBox"

#include <string>

namespace MenuKit
{

  namespace OSG
  {

    /** ColorThemeSkin
      * A simple implementation of the ThemeSkin base class.
      * Assumes theme has 2 fields, back and front color
      */
    template<class ThemeType>
    class ColorThemeSkin : public ThemeSkin<ThemeType>
    {
    public:
      typedef ThemeSkin<ThemeType> BaseClass;

      ColorThemeSkin(): BaseClass(),
        _picture(1.0,1.0), _margin(0.2), _text(0.8), _bits(0x0)
      {}

      ColorThemeSkin(const ColorThemeSkin& s): BaseClass(s),
        _picture(s._picture), _margin(s._margin), _text(s._text), _bits(s._bits)
      {}

      ColorThemeSkin& operator= (const ColorThemeSkin& ct)
      {
        BaseClass::operator =(ct);
        _picture = ct._picture;
        _margin = ct._margin;
        _text = ct._text;
        _bits = ct._bits;
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

    protected:
      float width(const std::string& word);
      osg::Node* item_graphic(const std::string& txt);
      osg::Node* separator_graphic();

      enum BITS
      {
        MENU      = 0x00000001,
        CHECKED   = 0x00000002,
        TOGGLE    = 0x00000004,
        RADIO     = 0x00000008,
        DISABLED  = 0x00000010,
        EXPANDED  = 0x00000020,
      };

      void setup_bits(unsigned int menubits);

      void bits(unsigned int b) { _bits = b; }
      unsigned int bits() const { return _bits; }

    private:
      Detail::Box _picture;  // for side graphics
      float _margin;         // distances
      float _text;           // percentages
      unsigned int _bits;            // add on graphic characteristics
    };

    typedef ColorThemeSkin<osgColor> osgSkin;
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

class FlatBox : public DrawableFunctor
{
public:
  FlatBox():
    DrawableFunctor(1.0), _depth(-0.001), _width(1.0), _mode(osg::PrimitiveSet::QUADS)
  {}

  FlatBox(float h, float w, float d=-0.001f, osg::PrimitiveSet::Mode m=osg::PrimitiveSet::QUADS):
    DrawableFunctor(h), _depth(d), _width(w), _mode(m)
  {}

  FlatBox(const FlatBox& b):
    DrawableFunctor(b), _depth(b._depth), _width(b._width), _mode(b._mode)
  {}

  ~FlatBox()
  {}

  void depth(float w) { _depth = w; }
  float depth() const { return _depth; }

  void mode(osg::PrimitiveSet::Mode m) { _mode = m; }
  osg::PrimitiveSet::Mode move() const { return _mode; }

  void width(float w) { _width = w; }
  float width() const { return _width; }

  virtual osg::Drawable* operator() ();

private:
  float _depth, _width;
  osg::PrimitiveSet::Mode _mode;
};

class Word : public DrawableFunctor
{
public:
  Word():
    DrawableFunctor(1.0f), _text(), _font(0x0), _ar(1.0), _dm(osgText::Text::TEXT)
  {}

  Word(const Word& w):
    DrawableFunctor(w), _text(w._text), _font(w._font), _ar(w._ar), _dm(w._dm)
  {}

  ~Word()
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
  Disk(float h,float d=0.0,unsigned int p=80):
    DrawableFunctor(h),_depth(d),_points(p) {}
  ~Disk() {}

  virtual osg::Drawable* operator() ();

  void depth(float d) { _depth = d; }
  float depth() const { return _depth; }

  void points(unsigned int p) { _points = p; }
  unsigned int points() const { return _points; }

private:
  unsigned int _points;
  float _depth;
};

// ------------------------ //
//  HELPER IMPLEMENTATION
// ------------------------ //
// TODO: ?make these classes templated for ThemeType?
osg::Drawable* Word::operator() ()
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

// TODO: turn off double sided polygons
osg::Drawable* FlatBox::operator() ()
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

  osg::Geometry* patch = new osg::Geometry();
  patch->setVertexArray( vertices );
  patch->setNormalArray( normals );
  patch->setNormalBinding( osg::Geometry::BIND_OVERALL );
  patch->setColorArray( colors );
  patch->setColorBinding( osg::Geometry::BIND_OVERALL );
  patch->addPrimitiveSet( new osg::DrawArrays(_mode,
                                              0,vertices->size()) );

  return( patch );
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

  osg::Geometry* patch = new osg::Geometry();
  patch->setVertexArray( vertices );
  patch->setNormalArray( normals );
  patch->setNormalBinding( osg::Geometry::BIND_OVERALL );
  patch->setColorArray( colors );
  patch->setColorBinding( osg::Geometry::BIND_OVERALL );
  patch->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,vertices->size()) );

  return( patch );
}

osg::Drawable* Disk::operator() ()
{
  float radius = 0.5*height();
  osg::Vec3Array* vertices = new osg::Vec3Array();
  osg::PrimitiveSet::Mode drawmode = osg::PrimitiveSet::TRIANGLE_FAN;

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

  osg::Vec4Array* colors = new osg::Vec4Array();
  colors->push_back( color() );

  osg::Vec3Array* normals = new osg::Vec3Array();
  normals->push_back( osg::Vec3(0.0,0.0,1.0) );

  osg::Geometry* disk = new osg::Geometry();
  disk->setVertexArray( vertices );
  disk->setNormalArray( normals );
  disk->setColorArray( colors );
  disk->setNormalBinding( osg::Geometry::BIND_OVERALL );
  disk->setColorBinding( osg::Geometry::BIND_OVERALL );
  disk->addPrimitiveSet( new osg::DrawArrays(drawmode,0,vertices->size()) );

  return( disk );
}

};  // end namespace Detail

// ------------------------ //
//   SKIN IMPLEMENTATION
// ------------------------ //

using namespace MenuKit;
using namespace OSG;

template<class ThemeType>
osg::Node* ColorThemeSkin<ThemeType>::operator ()(const Menu& m)
{
  _bits = 0x0;    // reset bits to nothing
  typedef MenuKit::Bits<unsigned int> setter;
  _bits = setter::add( _bits , MENU );

  osg::ref_ptr<osg::Node> node;

  // check for a 'SEPARATOR'  >:-/
  if( m.separator() )
    node = separator_graphic();

  else
  {
    this->setup_bits( m.flags() );
    node = item_graphic( m.text() );
  }

  return( node.release() );
}

template<class ThemeType>
osg::Node* ColorThemeSkin<ThemeType>::operator ()(const Button& b)
{
  _bits = 0x0;    // reset bits to nothing

  osg::ref_ptr<osg::Node> node;

  // check for a 'SEPARATOR'  >:-/
  if( b.separator() )
    node = separator_graphic();

  else
  {
    this->setup_bits( b.flags() );
    node = item_graphic( b.text() );
  }

  return( node.release() );
}

template<class ThemeType>
void ColorThemeSkin<ThemeType>::setup_bits(unsigned int itembits)
{
  // TODO: set up any bits necessary
  typedef MenuKit::Bits<unsigned int> cause;
  typedef MenuKit::Bits<unsigned int> effect;

  if( cause::has(itembits, MenuKit::Item::EXPANDED) )
    _bits = effect::add( _bits , EXPANDED );
  if( cause::has(itembits, MenuKit::Item::TOGGLE) )
    _bits = effect::add( _bits , TOGGLE );
  if( !cause::has(itembits, MenuKit::Item::ENABLED) )
    _bits = effect::add( _bits , DISABLED );
  if( cause::has(itembits, MenuKit::Item::RADIO) )
    _bits = effect::add( _bits , RADIO );
  if( cause::has(itembits, MenuKit::Item::CHECKED) )
    _bits = effect::add( _bits , CHECKED );
}

template<class ThemeType>
osg::Node* ColorThemeSkin<ThemeType>::item_graphic(const std::string& txt)
{
  const ThemeType& scheme = this->theme();

  // make the word graphic functor
  Detail::Word word;
  word.height( _text*(this->box().height()) );
  word.text( txt );
  word.font( font() );
  word.color( scheme.front() );
  word.draw_mode( osgText::Text::TEXT|osgText::Text::BOUNDINGBOX );

  // use the functor to make the graphics
  osg::ref_ptr<osg::Drawable> drawable = word();

  // make a box for convenience, used below to move the word
  osg::BoundingBox bb = drawable->getBound();
  float wordheight = bb.yMax() - bb.yMin();
  osg::Vec3 move(-_picture.width()-0.5*_margin, -0.5*wordheight, 0.0);

  // add the drawable to a node, for movement
  osg::ref_ptr<osg::Geode> wordgeode = new osg::Geode();
  wordgeode->addDrawable( drawable.get() );

  // move the word's node
  osg::ref_ptr<osg::MatrixTransform> wordmt = new osg::MatrixTransform();
  wordmt->addChild( wordgeode.get() );
  wordmt->setMatrix( osg::Matrix::translate(move) );

  // make the background box
  Detail::FlatBox backbox(this->box().height(),this->box().width(),-0.002);
  backbox.color( scheme.back() );
  osg::ref_ptr<osg::Geode> backgeode = new osg::Geode();
  backgeode->addDrawable( backbox() );
  osg::ref_ptr<osg::MatrixTransform> backmt = new osg::MatrixTransform();
  backmt->addChild( backgeode.get() );

  // TODO: make other graphic boxes
  Detail::FlatBox sidebox(_picture.height(),_picture.width(),-0.001);
  sidebox.color( scheme.back() );
  osg::ref_ptr<osg::Drawable> rd = sidebox();
  osg::ref_ptr<osg::Geode> rg = new osg::Geode();
  rg->addDrawable( rd.get() );
  osg::ref_ptr<osg::MatrixTransform> rgmt = new osg::MatrixTransform();
  rgmt->addChild( rg.get() );
  float rxmov = 0.5*backbox.width() - 0.5*sidebox.width();
  rgmt->setMatrix( osg::Matrix::translate( osg::Vec3(rxmov,0.0,0.0) ) );

  sidebox.color( scheme.middle() );
  osg::ref_ptr<osg::Drawable> ld = sidebox();
  osg::ref_ptr<osg::Geode> lg = new osg::Geode();
  lg->addDrawable( ld.get() );
  osg::ref_ptr<osg::MatrixTransform> lgmt = new osg::MatrixTransform();
  lgmt->addChild( lg.get() );
  float lxmov = -0.5*backbox.width() + 0.5*sidebox.width();
  lgmt->setMatrix( osg::Matrix::translate( osg::Vec3(lxmov,0.0,0.0) ) );

  osg::Group* group = new osg::Group();
  group->addChild( backmt.get() );
  group->addChild( wordmt.get() );
  group->addChild( rgmt.get() );
  group->addChild( lgmt.get() );

  // TODO: add graphics for supported bits!!!
  typedef MenuKit::Bits<unsigned int> checker;
  if( checker::has(this->bits(), TOGGLE) )
  {
    Detail::FlatBox markbox(_text*_picture.height(),_text*_picture.width(),0.0);
    markbox.color( this->theme().front() );
    osg::ref_ptr<osg::Drawable> mbdraw = markbox();

    Detail::FlatBox markopen(_text*markbox.height(),_text*markbox.width(),0.001);
    markopen.color( this->theme().middle() );
    osg::ref_ptr<osg::Drawable> modraw = markopen();

    osg::ref_ptr<osg::Geode> mbgeo = new osg::Geode();
    mbgeo->addDrawable( mbdraw.get() );
    mbgeo->addDrawable( modraw.get() );

    if( checker::has(this->bits(), CHECKED) )
    {
      Detail::FlatBox markclosed(_text*markopen.height(),_text*markopen.width(),0.002);
      markclosed.color( this->theme().front() );
      osg::ref_ptr<osg::Drawable> mcdraw = markclosed();
      mbgeo->addDrawable( mcdraw.get() );
    }

    lgmt->addChild( mbgeo.get() );
  }  // end of TOGGLE add-on

  if( checker::has(this->bits(), RADIO) )
  {
    Detail::Disk markdisc(_text*this->box().height(),0.0);
    markdisc.color( this->theme().front() );
    osg::ref_ptr<osg::Drawable> mddraw = markdisc();

    Detail::Disk markopen(_text*markdisc.height(),0.001);
    markopen.color( this->theme().middle() );
    osg::ref_ptr<osg::Drawable> modraw = markopen();

    osg::ref_ptr<osg::Geode> mdgeo = new osg::Geode();
    mdgeo->addDrawable( mddraw.get() );
    mdgeo->addDrawable( modraw.get() );

    if( checker::has(this->bits(), CHECKED) )
    {
      Detail::Disk markclosed(_text*markopen.height(),0.002);
      markclosed.color( this->theme().front() );
      osg::ref_ptr<osg::Drawable> mcdraw = markclosed();
      mdgeo->addDrawable( mcdraw.get() );
    }

    lgmt->addChild( mdgeo.get() );
  }

  if( checker::has(this->bits(), MENU) )
  {
    Detail::Arrow mark(_text*this->box().height());
    mark.color( this->theme().front() );
    osg::ref_ptr<osg::Drawable> md = mark();
    osg::ref_ptr<osg::Geode> mgeo = new osg::Geode();
    mgeo->addDrawable( md.get() );

    if( checker::has(this->bits(), EXPANDED) )
    {
      Detail::Arrow expander(_text*mark.height());
      expander.color( this->theme().back() );
      osg::ref_ptr<osg::Drawable> exdraw = expander();
      mgeo->addDrawable( exdraw.get() );
    }

    rgmt->addChild( mgeo.get() );
  }

  return( group );
}

template<class ThemeType>
osg::Node* ColorThemeSkin<ThemeType>::separator_graphic()
{
  Detail::FlatBox bgbox(this->box().height(),
                        this->box().width(),
                        -0.002);
  bgbox.color( this->theme().back() );
  osg::ref_ptr<osg::Drawable> bgdraw = bgbox();

  osg::ref_ptr<osg::Geode> bggeo = new osg::Geode();
  bggeo->addDrawable( bgdraw.get() );

  Detail::FlatBox stripe(_text*(this->box().height()),
                          this->box().width()-_picture.width()-_margin,
                          -0.001);
  stripe.color( this->theme().front() );
  osg::ref_ptr<osg::Drawable> fgdraw = stripe();

  // add the parts
  osg::Group* group = new osg::Group();
  group->addChild( bggeo.get() );

  return group;
}

template<class ThemeType>
float ColorThemeSkin<ThemeType>::width(const Menu& m)
{
  // equation for graphic width:
  // 2 graphic boxes, 2 x _box's width
  // 1 text box,      1 x  boundingbox's width
  // 2 margins,       2 x _margin
  float wordwidth = this->width( m.text() );

  // the equation, implemented
  float w = 2.0*_picture.width() + wordwidth + 2.0*_margin;
  return( w );
}

template<class ThemeType>
float ColorThemeSkin<ThemeType>::width(const Button& b)
{
  // equation for graphic width:
  // 2 graphic boxes, 2 x _box's width
  // 1 text box,      1 x  boundingbox's width
  // 2 margins,       2 x _margin
  float wordwidth = this->width( b.text() );

  // the equation, implemented
  float w = 2.0*_picture.width() + wordwidth + 2.0*_margin;
  return( w );
}

template<class ThemeType>
float ColorThemeSkin<ThemeType>::width(const std::string& s)
{
  Detail::Word word;
  word.height( _text*(this->box().height()) );
  word.text( s );
  word.font( font() );
  word.draw_mode( osgText::Text::BOUNDINGBOX );

  // use the functor to make the graphics
  osg::ref_ptr<osg::Drawable> drawable = word();

  // make a box for convenience, used below to move the word
  osg::BoundingBox bb = drawable->getBound();
  return( bb.xMax() - bb.xMin() );
}

#endif
