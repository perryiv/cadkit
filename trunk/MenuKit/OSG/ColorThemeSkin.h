#ifndef _menukit_osg_colorthemeskin_h_
#define _menukit_osg_colorthemeskin_h_

#include "ThemeSkin.h"   // base class
#include "ColorTheme.h"  // for tyepdef

#include "../Bits.h"
#include "../Box.h"
#include "../Menu.h"
#include "../Button.h"
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
      */
    template<class ThemeType>
    class ColorThemeSkin : public ThemeSkin<ThemeType>
    {
    public:
      typedef ThemeSkin<ThemeType> BaseClass;

      ColorThemeSkin(): BaseClass(),
        _margin(0.2), _text(0.8), _border(0.02), _bits(0x0)
      {}

      ColorThemeSkin(const ColorThemeSkin& s): BaseClass(s),
        _margin(s._margin), _text(s._text), _border(s._border), _bits(s._bits)
      {}

      ColorThemeSkin& operator= (const ColorThemeSkin& ct)
      {
        BaseClass::operator =(ct);
        _margin = ct._margin;
        _text = ct._text;
        _border = ct._border;
        _bits = ct._bits;
        return( *this );
      }

      virtual osg::Node* operator ()(const Menu& m);
      virtual osg::Node* operator ()(const Button& b);
      // TODO: virtual osg::Node* operator =(const Item& b);

      virtual float height(const Menu& m);
      virtual float height(const Button& b);

      virtual float width(const Menu& m);
      virtual float width(const Button& b);
      virtual float width(const Item* i);
      // TODO: virtual float width(const Button& b);

      void border(float m) { _border=m; }
      float border() const { return _border; }

      void margin(float m) { _margin=m; }
      float margin() const { return _margin; }

      void text_ratio(float t) { _text=t; }
      float text_ratio() const { return _text; }

    protected:
      float word_width(const std::string& word);
      float item_width(const std::string& w,const Menu* m);
      osg::Node* item_graphic(const std::string& txt, const Menu* parent);
      osg::Node* separator_graphic();

      enum BITS
      {
        MENU      = 0x00000001,
        CHECKED   = 0x00000002,
        TOGGLE    = 0x00000004,
        RADIO     = 0x00000008,
        DISABLED  = 0x00000010,
        EXPANDED  = 0x00000020,
        MARKED    = 0x00000040,
      };

      void gather_bits(unsigned int menubits);

      void bits(unsigned int b) { _bits = b; }
      unsigned int bits() const { return _bits; }

    private:
      float _margin,_border;  // distances
      float _text;            // percentages
      unsigned int _bits;     // add-on graphic characteristics
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
  virtual ~DrawableFunctor() {}

  void color(const osg::Vec4& c) { _color = c; }
  const osg::Vec4& color() const { return _color; }

  void height(float h) { _height = h; }
  float height() const { return _height; }

  virtual osg::Drawable* operator() ()=0;

private:
  float _height;
  osg::Vec4 _color;
};

class Border
{
public:
  Border(const Box& out,const Box& in,float d):
    _outer(out), _inner(in), _depth(d), _color(osg::Vec4(0.0,0.0,0.0,1.0))
  {}

  Border(const Border& b):
    _outer(b._outer), _inner(b._inner), _depth(b._depth), _color(b._color)
  {}

  Border& operator =(const Border& b)
  {
    _outer = b._outer;
    _inner = b._inner;
    _depth = b._depth;
    _color = b._color;
    return *this;
  }
  virtual ~Border() {}

  osg::Node* operator ()();

  void outer(const Box& b) { _outer = b; }
  const Box& outer() const { return _outer; }
  Box& outer() { return _outer; }

  void inner(const Box& b) { _inner = b; }
  const Box& inner() const { return _inner; }
  Box& inner() { return _inner; }

  void depth(float d) { _depth  = d; }
  float depth() const { return _depth; }

  void color(const osg::Vec4& c) { _color = c; }
  const osg::Vec4& color() const { return _color; }

private:
  Box _outer, _inner;
  float _depth;
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

  virtual ~FlatBox()
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

class Arrow: public DrawableFunctor
{
public:
  Arrow(float h, float d): DrawableFunctor(h), _depth(d) {}
  virtual ~Arrow() {}

  virtual osg::Drawable* operator() ();

  void depth(float d) { _depth = d; }
  float depth() const { return _depth; }

private:
  float _depth;
};

class Disk: public DrawableFunctor
{
public:
  Disk(float h,float d=0.0,unsigned int p=80):
    DrawableFunctor(h),_depth(d),_points(p) {}
  virtual ~Disk() {}

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

osg::Node* Border::operator ()()
{
  // need to make 4 FlatBoxes, but not have them overlap
  float vert = _outer.height()-_inner.height();
  FlatBox topbottom(vert,_outer.width(),_depth);
  topbottom.color( _color );

  osg::ref_ptr<osg::Geode> topgeode = new osg::Geode();
  topgeode->addDrawable( topbottom() );
  osg::ref_ptr<osg::MatrixTransform> top = new osg::MatrixTransform();
  top->addChild( topgeode.get() );
  top->setMatrix( osg::Matrix::translate( osg::Vec3(0.0,0.5*_outer.height()-0.5*vert,0.0) ) );

  osg::ref_ptr<osg::Geode> botgeode = new osg::Geode();
  botgeode->addDrawable( topbottom() );
  osg::ref_ptr<osg::MatrixTransform> bot = new osg::MatrixTransform();
  bot->addChild( botgeode.get() );
  bot->setMatrix( osg::Matrix::translate( osg::Vec3(0.0,-0.5*_outer.height()+0.5*vert,0.0) ) );

  float hori = _outer.width() - _inner.width();
  FlatBox rightleft(_inner.height(),hori,_depth);
  rightleft.color( _color );

  osg::ref_ptr<osg::Geode> rightgeode = new osg::Geode();
  rightgeode->addDrawable( rightleft() );
  osg::ref_ptr<osg::MatrixTransform> right = new osg::MatrixTransform();
  right->addChild( rightgeode.get() );
  right->setMatrix( osg::Matrix::translate( osg::Vec3(0.5*_outer.width()-0.5*hori,0.0,0.0) ) );

  osg::ref_ptr<osg::Geode> leftgeode = new osg::Geode();
  leftgeode->addDrawable( rightleft() );
  osg::ref_ptr<osg::MatrixTransform> left = new osg::MatrixTransform();
  left->addChild( leftgeode.get() );
  left->setMatrix( osg::Matrix::translate( osg::Vec3(-0.5*_outer.width()+0.5*hori,0.0,0.0) ) );

  osg::Group* group = new osg::Group();
  group->addChild( top.get() );
  group->addChild( bot.get() );
  group->addChild( right.get() );
  group->addChild( left.get() );

  return group;
}

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
  float h_4 = 0.5f * h_2;
  vertices->push_back( osg::Vec3(-h_4, h_2 , _depth) ); // top
  vertices->push_back( osg::Vec3(-h_4,-h_2 , _depth) ); // botton
  vertices->push_back( osg::Vec3(h_2, 0.0 , _depth) );  // pointing dir

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
    this->gather_bits( m.flags() );
    node = item_graphic( m.text(), m.parent() );
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
    this->gather_bits( b.flags() );
    node = item_graphic( b.text(), b.parent() );
  }

  return( node.release() );
}

template<class ThemeType>
void ColorThemeSkin<ThemeType>::gather_bits(unsigned int itembits)
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
  if( cause::has(itembits, MenuKit::Item::MARKED) )
    _bits = effect::add( _bits , MARKED );
}

template<class ThemeType>
osg::Node* ColorThemeSkin<ThemeType>::item_graphic(const std::string& txt,const Menu* parent)
{
  const ThemeType& scheme = this->theme();
  Detail::Box thebox = this->box();

  // make the background box(es)
  Detail::FlatBox borderbox( thebox.height(),thebox.width(),-0.003);
  //TODO: make a real border, not use this: Detail::FlatBox midbox(borderbox.height()-_border,borderbox.width()-_border,-0.002);
  if( !parent || parent->layout()==Menu::HORIZONTAL )
  {
    borderbox.color( scheme.middle() );
  }
  else
  {
    borderbox.color( scheme.back() );
  }

  osg::ref_ptr<osg::Geode> backgeode = new osg::Geode();
  backgeode->addDrawable( borderbox() );
  //TODO: make a real border, not: backgeode->addDrawable( midbox() );

  // make the text graphic functor
  Detail::Word word;
  word.height( _text*(thebox.height()-2.0*_border) );
  word.text( txt );
  word.font( font() );
  word.color( scheme.text() );
  word.draw_mode( osgText::Text::TEXT );
  osg::ref_ptr<osg::Drawable> worddrawable = word();

  // make a boundingbox for convenience, used below, to move the word
  osg::BoundingBox bb = worddrawable->getBound();
  float wordheight = bb.yMax() - bb.yMin();
  float wordwidth = bb.xMax() - bb.xMin();

  // make the word geode
  osg::ref_ptr<osg::Geode> wordgeode = new osg::Geode();
  wordgeode->addDrawable( worddrawable.get() );

  // move the word's node
  osg::ref_ptr<osg::MatrixTransform> wordmt = new osg::MatrixTransform();
  wordmt->addChild( wordgeode.get() );
  osg::Vec3 wordcorrection(-0.5*thebox.width()+thebox.height()+_border+_margin,-0.5*wordheight,0.0);
  if( !parent || parent->layout()==Menu::HORIZONTAL )
    wordcorrection[0] = -0.5*wordwidth;
  wordmt->setMatrix( osg::Matrix::translate(wordcorrection) );

  osg::ref_ptr<osg::Group> group = new osg::Group();
  group->addChild( backgeode.get() );
  group->addChild( wordmt.get() );

  // the total correction
  osg::MatrixTransform* mt = new osg::MatrixTransform();
  mt->addChild( group.get() );
  mt->setMatrix( osg::Matrix::translate(0.5*thebox.width(),-0.5*thebox.height(),0.0) );

  osg::Group* top = new osg::Group();  // TODO: take this out
  top->addChild( mt );  // TODO: take this out

  // escape if  null parent
  if( !parent )  // valid parent  TODO: implement
  {
    // TODO: what is this? : osg::Vec3 nosidecorrection(-0.5*wordwidth,-0.5*wordheight,0.0);
    // TODO: what is this? : wordmt->setMatrix( osg::Matrix::translate( nosidecorrection ) );
    //return mt;  // TODO: put this back
    return top; // TODO: take this out
  }

  // escape if horizontal parent
  if( parent->layout() == MenuKit::Menu::HORIZONTAL )
  {
    // TODO: what is this? : osg::Vec3 nosidecorrection(-0.5*wordwidth,-0.5*wordheight,0.0);
    // TODO: what is this? : wordmt->setMatrix( osg::Matrix::translate( nosidecorrection ) );
    //return mt;  // TODO: put this back
    return top; // TODO: take this out
  }

  // TODO: make other graphic boxes

  // left side
  Detail::FlatBox sidebox(borderbox.height(),borderbox.height()+_border,-0.001);
  sidebox.color( scheme.middle() );
  osg::ref_ptr<osg::Drawable> ld = sidebox();
  osg::ref_ptr<osg::Geode> lg = new osg::Geode();
  lg->addDrawable( ld.get() );
  osg::ref_ptr<osg::MatrixTransform> lgmt = new osg::MatrixTransform();
  lgmt->addChild( lg.get() );
  float lxmov = -0.5*thebox.width() + 0.5*sidebox.width();
  lgmt->setMatrix( osg::Matrix::translate( osg::Vec3(lxmov,0.0,0.0) ) );
  group->addChild( lgmt.get() );

  // right side
  osg::ref_ptr<osg::MatrixTransform> rgmt = new osg::MatrixTransform();
  float rxmov = 0.5*thebox.width() - 0.5*sidebox.width() - _border;
  rgmt->setMatrix( osg::Matrix::translate( osg::Vec3(rxmov,0.0,0.0) ) );
  group->addChild( rgmt.get() );

  // TODO: add graphics for each supported bit!!!
  typedef MenuKit::Bits<unsigned int> checker;
  if( checker::has(this->bits(), TOGGLE) )
  {
    if( checker::has(this->bits(), CHECKED) )
    {
      // the border
      Detail::Box outy(thebox.height()-2.0*_border,thebox.height()-2.0*_border);
      Detail::Box inny(outy.height()-2.0*_border,outy.height()-2.0*_border);
      Detail::Border brdr(outy,inny,0.001);
      brdr.color( scheme.special() );
      lgmt->addChild( brdr() );

      Detail::FlatBox markclosed(0.5*inny.height(),0.5*inny.width(),0.002);
      markclosed.color( scheme.special() );
      osg::ref_ptr<osg::Drawable> mcdraw = markclosed();
      osg::ref_ptr<osg::Geode> mbgeo = new osg::Geode();
      mbgeo->addDrawable( mcdraw.get() );
      lgmt->addChild( mbgeo.get() );
    }
  }  // end of TOGGLE add-on

  if( checker::has(this->bits(), RADIO) )
  {
    Detail::Disk markdisc(_text*word.height(),0.0);
    markdisc.color( scheme.text() );
    osg::ref_ptr<osg::Drawable> mddraw = markdisc();

    Detail::Disk markopen(_text*markdisc.height(),0.001);
    markopen.color( scheme.middle() );
    osg::ref_ptr<osg::Drawable> modraw = markopen();

    osg::ref_ptr<osg::Geode> mdgeo = new osg::Geode();
    mdgeo->addDrawable( mddraw.get() );
    mdgeo->addDrawable( modraw.get() );

    if( checker::has(this->bits(), CHECKED) )
    {
      Detail::Disk markclosed(_text*markopen.height(),0.002);
      markclosed.color( scheme.text() );
      osg::ref_ptr<osg::Drawable> mcdraw = markclosed();
      mdgeo->addDrawable( mcdraw.get() );
    }

    lgmt->addChild( mdgeo.get() );
  }  // end of CHECKED add-on

  if( checker::has(this->bits(), MENU) )
  {
    Detail::Arrow mark(0.5*word.height(),0.0);
    mark.color( scheme.text() );
    osg::ref_ptr<osg::Drawable> md = mark();
    osg::ref_ptr<osg::Geode> mgeo = new osg::Geode();
    mgeo->addDrawable( md.get() );

    if( !checker::has(this->bits(), EXPANDED) )
    {
      Detail::Arrow expander(0.5*mark.height(),0.001);
      expander.color( scheme.back() );
      osg::ref_ptr<osg::Drawable> exdraw = expander();
      mgeo->addDrawable( exdraw.get() );
    }

    else  // it is expanded and children are shown
    {
      if( parent->layout()==Menu::VERTICAL )  // only for vertical parents
      {
        Detail::Box inner(thebox.height()-2.0*_border,thebox.width()-2.0*_border);
        Detail::Border fence(thebox,inner,0.001);
        fence.color( scheme.border() );
        osg::ref_ptr<osg::Node> fencenode = fence();
        mt->addChild( fencenode.get() );
      }
    }

    // TODO: if expanded and parent is vertical, draw border

    rgmt->addChild( mgeo.get() );
  }  // end of MENU add-on

  if( checker::has(this->bits() , MARKED) )
  {
    Detail::Box inner(thebox.height()-2.0*_border,thebox.width()-2.0*_border);
    Detail::Border fence(thebox,inner,0.001);
    fence.color( scheme.border() );
    osg::ref_ptr<osg::Node> fencenode = fence();
    mt->addChild( fencenode.get() );
  }  // end of MARKED add-on

  //return mt;  // TODO: put this back
  return top; // TODO: take this out
}

template<class ThemeType>
osg::Node* ColorThemeSkin<ThemeType>::separator_graphic()  // TODO: pass args for vert separator
{
  Detail::FlatBox bgbox(this->separator().height(),
                        this->box().width(),
                        -0.002);
  bgbox.color( this->theme().back() );
  osg::ref_ptr<osg::Drawable> bgdraw = bgbox();

  osg::ref_ptr<osg::Geode> bggeo = new osg::Geode();
  bggeo->addDrawable( bgdraw.get() );

  Detail::FlatBox stripe(_text*(this->box().height()),this->box().width()-_margin,-0.001);
  stripe.color( this->theme().text() );
  osg::ref_ptr<osg::Drawable> fgdraw = stripe();

  // add the parts
  osg::Group* group = new osg::Group();
  group->addChild( bggeo.get() );

  return group;
}

template<class ThemeType>
float ColorThemeSkin<ThemeType>::height(const Menu& m)
{
  if( m.separator() )
    return( this->separator().height() );
  else
    return( box().height() ); // TODO: is this the best thing to do?
}

template<class ThemeType>
float ColorThemeSkin<ThemeType>::height(const Button& b)
{
  if( b.separator() )
    return( this->separator().height() );
  else
    return( box().height() ); // TODO: is this the best thing to do?
}

template<class ThemeType>
float ColorThemeSkin<ThemeType>::width(const Menu& m)
{
  return( item_width( m.text() , m.parent() ) );
}

template<class ThemeType>
float ColorThemeSkin<ThemeType>::width(const Button& b)
{
  return( item_width(b.text(), b.parent()) );
}

template<class ThemeType>
float ColorThemeSkin<ThemeType>::width(const Item* i)
{
  return( item_width(i->text(), i->parent()) );
}

template<class ThemeType>
float ColorThemeSkin<ThemeType>::item_width(const std::string& s,const Menu* parent)
{
  // check for text content
  if( s.empty() )
    return( 0.0 );

  // use graphics engine to calculate width of displayed text
  float wordwidth = this->word_width( s );

  // check for top-level item
  if( !parent )
    return( wordwidth + 2.0*(_margin+_border) );

  // --- LAYOUT correctioning --- //
  // the eqation for the width of a horizontal graphic
  // 2 margins,   2 x _margin
  // 1 text box,  1 x graphic text width
  if( parent->layout() == Menu::HORIZONTAL )
    return( wordwidth + 2.0*(_margin+_border) );

  // equation for the width of a vertical graphic:
  // 2 side boxes, 2 x _box's width (which is the height of the box - 2 x margin)
  // 1 text box,      1 x  graphic text width
  // 2 margins,       2 x _margin
  else
    return( 2.0*(this->box().height()+_border+_margin) + wordwidth );
}

template<class ThemeType>
float ColorThemeSkin<ThemeType>::word_width(const std::string& s)
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
