#ifndef _menukit_osg_colorthemeskin_h_
#define _menukit_osg_colorthemeskin_h_

#include "MenuKit/OSG/ThemeSkin.h"   // the base class
#include "MenuKit/OSG/ColorTheme.h"  // to get a tyepdef
#include "MenuKit/OSG/Border.h"      // graphics helpers
#include "MenuKit/OSG/FlatBox.h"
#include "MenuKit/OSG/Word.h"
#include "MenuKit/OSG/Arrow.h"
#include "MenuKit/OSG/Disk.h"

#include "MenuKit/Bits.h"
#include "MenuKit/Box.h"
#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
// TODO: #include "Item.h"

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

    /** VisualThemeSkin
      * The VisualThemeSkin class is a simple implementation
      * of the ThemeSkin base class.  Its goal is to provide
      * Menu Items with a similar appearance to the Menu Items
      * in Microsoft's Visual Studio IDE.
      */
    template<typename ThemeMap>
    class VisualThemeSkin : public ThemeSkin<ThemeMap>
    {
    public:
      typedef ThemeSkin<ThemeMap> BaseClass;

      enum BITS
      {
        NONE      = 0x00000000,
        MENU      = 0x00000001,
        CHECKED   = 0x00000002,
        TOGGLE    = 0x00000004,
        RADIO     = 0x00000008,
        DISABLED  = 0x00000010,
        EXPANDED  = 0x00000020,
        MARKED    = 0x00000040,
      };

      VisualThemeSkin(): BaseClass(),
        _margin(0.2), _text(0.8), _border(0.02), _bits(NONE)
      {}

      VisualThemeSkin(const VisualThemeSkin& s): BaseClass(s),
        _margin(s._margin), _text(s._text), _border(s._border), _bits(s._bits)
      {}

      VisualThemeSkin& operator= (const VisualThemeSkin& ct)
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

      void gather_bits(unsigned int menubits);

      void bits(BITS b) { _bits = b; }
      BITS bits() const { return _bits; }

    private:
      float _margin,  // distance between box's edge to an add-on
            _border;  /// TODO: rename this // must be an extra width parameter, don't remember
      float _text;    // percentage of box height
      BITS _bits;     // add-on graphic characteristics
    };

    typedef VisualThemeSkin<osgColorTheme> osgVisualSkin;
  };

};

// ------------------------ //
//   SKIN IMPLEMENTATION
// ------------------------ //

using namespace MenuKit;
using namespace OSG;

template<typename ThemeMap>
osg::Node* VisualThemeSkin<ThemeMap>::operator ()(const Menu& m)
{
  _bits = NONE;    // reset bits to nothing
  typedef MenuKit::Bits<BITS> setter;
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

template<typename ThemeMap>
osg::Node* VisualThemeSkin<ThemeMap>::operator ()(const Button& b)
{
  _bits = NONE;    // reset bits to nothing

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

template<typename ThemeMap>
void VisualThemeSkin<ThemeMap>::gather_bits(unsigned int itembits)
{
  // TODO: set up any bits necessary
  typedef MenuKit::Bits<unsigned int> cause;
  typedef MenuKit::Bits<BITS> effect;

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

template<typename ThemeMap>
osg::Node* VisualThemeSkin<ThemeMap>::item_graphic(const std::string& txt,const Menu* parent)
{
  const ThemeMap& scheme = this->theme();
  Detail::Box thebox = this->box();

  // make the background box(es)
  Detail::FlatBox borderbox( thebox.height(),thebox.width(),-0.003);
  if( !parent || parent->layout()==Menu::HORIZONTAL )
  {
    borderbox.color( scheme["middle"] );
  }
  else
  {
    borderbox.color( scheme["background"] );
  }

  osg::ref_ptr<osg::Geode> backgeode = new osg::Geode();
  backgeode->setName("MK_OSG_CTS_backgroundbox_geode");
  backgeode->addDrawable( borderbox() );

  // make the text graphic functor
  Detail::Word word;
  word.height( _text*(thebox.height()-2.0*_border) );
  word.text( txt );
  word.font( font() );
  word.color( scheme["text"] );
  word.draw_mode( osgText::Text::TEXT );
  osg::ref_ptr<osg::Drawable> worddrawable = word();

  // make a boundingbox for convenience, used below, to move the word
  const osg::BoundingBox& bb = worddrawable->getBound();
  float wordheight = bb.yMax() - bb.yMin();
  float wordwidth = bb.xMax() - bb.xMin();

  // make the word geode
  osg::ref_ptr<osg::Geode> wordgeode = new osg::Geode();
  wordgeode->setName("MK_OSG_CTS_word_geode");
  wordgeode->addDrawable( worddrawable.get() );

  // move the word's node
  osg::ref_ptr<osg::MatrixTransform> wordmt = new osg::MatrixTransform();
  wordmt->setName("MK_OSG_CTS_word_xform");
  wordmt->addChild( wordgeode.get() );
  osg::Vec3 wordcorrection(-0.5*thebox.width()+thebox.height()+_border+_margin,-0.5*wordheight,0.0);
  if( !parent || parent->layout()==Menu::HORIZONTAL )
    wordcorrection[0] = -0.5*wordwidth;
  wordmt->setMatrix( osg::Matrix::translate(wordcorrection) );

  osg::ref_ptr<osg::Group> group = new osg::Group();
  group->setName("MK_OSG_CTS_leaf_group");
  group->addChild( backgeode.get() );
  group->addChild( wordmt.get() );

  // the total correction
  osg::MatrixTransform* mt = new osg::MatrixTransform();
  mt->setName("MK_OSG_CTS_leafxform");
  mt->addChild( group.get() );
  mt->setMatrix( osg::Matrix::translate(0.5*thebox.width(),-0.5*thebox.height(),0.0) );

  // escape if  null parent
  if( !parent )  // valid parent  TODO: implement
  {
    // TODO: what is this? : osg::Vec3 nosidecorrection(-0.5*wordwidth,-0.5*wordheight,0.0);
    // TODO: what is this? : wordmt->setMatrix( osg::Matrix::translate( nosidecorrection ) );
    return mt;
  }

  // escape if horizontal parent
  if( parent->layout() == MenuKit::Menu::HORIZONTAL )
  {
    // TODO: what is this? : osg::Vec3 nosidecorrection(-0.5*wordwidth,-0.5*wordheight,0.0);
    // TODO: what is this? : wordmt->setMatrix( osg::Matrix::translate( nosidecorrection ) );
    return mt;
  }

  /// TODO: make other graphic boxes.  completed?

  // left side
  Detail::FlatBox sidebox(borderbox.height(),borderbox.height()+_border,-0.001);
  sidebox.color( scheme["middle"] );
  osg::ref_ptr<osg::Drawable> ld = sidebox();
  osg::ref_ptr<osg::Geode> lg = new osg::Geode();
  lg->setName("MK_OSG_CTS_leftleaf");
  lg->addDrawable( ld.get() );
  osg::ref_ptr<osg::MatrixTransform> lgmt = new osg::MatrixTransform();
  lgmt->setName("MK_OSG_CTS_leftleafxform");
  lgmt->addChild( lg.get() );
  float lxmov = -0.5*thebox.width() + 0.5*sidebox.width();
  lgmt->setMatrix( osg::Matrix::translate( osg::Vec3(lxmov,0.0,0.0) ) );
  group->addChild( lgmt.get() );

  // right side
  osg::ref_ptr<osg::MatrixTransform> rgmt = new osg::MatrixTransform();
  rgmt->setName("MK_OSG_CTS_rightleafxform");
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
      brdr.color( scheme["special"] );
      lgmt->addChild( brdr() );

      Detail::FlatBox markclosed(0.5*inny.height(),0.5*inny.width(),0.002);
      markclosed.color( scheme["special"] );
      osg::ref_ptr<osg::Drawable> mcdraw = markclosed();
      osg::ref_ptr<osg::Geode> mbgeo = new osg::Geode();
      mbgeo->setName("MK_OSG_CTS_markbox");
      mbgeo->addDrawable( mcdraw.get() );
      lgmt->addChild( mbgeo.get() );
    }
  }  // end of TOGGLE add-on

  if( checker::has(this->bits(), RADIO) )
  {
    Detail::Disk markdisc(_text*word.height(),0.0);
    markdisc.color( scheme["text"] );
    osg::ref_ptr<osg::Drawable> mddraw = markdisc();

    Detail::Disk markopen(_text*markdisc.height(),0.001);
    markopen.color( scheme["middle"] );
    osg::ref_ptr<osg::Drawable> modraw = markopen();

    osg::ref_ptr<osg::Geode> mdgeo = new osg::Geode();
    mdgeo->setName("MK_OSG_CTS_radiobitsgeode");
    mdgeo->addDrawable( mddraw.get() );
    mdgeo->addDrawable( modraw.get() );

    if( checker::has(this->bits(), CHECKED) )
    {
      Detail::Disk markclosed(_text*markopen.height(),0.002);
      markclosed.color( scheme["text"] );
      osg::ref_ptr<osg::Drawable> mcdraw = markclosed();
      mdgeo->addDrawable( mcdraw.get() );
    }

    lgmt->addChild( mdgeo.get() );
  }  // end of CHECKED add-on

  if( checker::has(this->bits(), MENU) )
  {
    Detail::Arrow mark(0.5*word.height(),0.0);
    mark.color( scheme["text"] );
    osg::ref_ptr<osg::Drawable> md = mark();
    osg::ref_ptr<osg::Geode> mgeo = new osg::Geode();
    mgeo->setName("MK_OSG_CTS_menubitsgeode");
    mgeo->addDrawable( md.get() );

    if( !checker::has(this->bits(), EXPANDED) )
    {
      Detail::Arrow expander(0.5*mark.height(),0.001);
      expander.color( scheme["background"] );
      osg::ref_ptr<osg::Drawable> exdraw = expander();
      mgeo->addDrawable( exdraw.get() );
    }

    else  // it is expanded and children are shown
    {
      if( parent->layout()==Menu::VERTICAL )  // only for vertical parents
      {
        Detail::Box inner(thebox.height()-2.0*_border,thebox.width()-2.0*_border);
        Detail::Border fence(thebox,inner,0.001);
        fence.color( scheme["border"] );
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
    fence.color( scheme["border"] );
    osg::ref_ptr<osg::Node> fencenode = fence();
    mt->addChild( fencenode.get() );
  }  // end of MARKED add-on

  return mt;
}

/// TODO: support vertical separators for horizontal menus, low priority
template<typename ThemeMap>
osg::Node* VisualThemeSkin<ThemeMap>::separator_graphic()
{
  Detail::FlatBox bgbox(this->separator().height(),
                        this->box().width(),         /// TODO: possible problem area?
                        -0.002);
  bgbox.color( this->theme()["background"] );
  osg::ref_ptr<osg::Drawable> bgdraw = bgbox();

  // make the background geode and add the geometry
  osg::ref_ptr<osg::Geode> bggeo = new osg::Geode();
  bggeo->setName("MK_OSG_CTS_separator_background_geode");
  bggeo->addDrawable( bgdraw.get() );

  // make the left box
  Detail::FlatBox leftbox(this->separator().height(),
                          this->box().height()+_border,
                         -0.001);
  leftbox.color( this->theme()["middle"] );
  osg::ref_ptr<osg::Drawable> leftboxdraw = leftbox();

  // add the left box's geometry to a node
  osg::ref_ptr<osg::Geode> leftgeo = new osg::Geode();
  leftgeo->setName("MK_OSG_CTS_separator_leftgeo");
  leftgeo->addDrawable( leftboxdraw.get() );

  // move the left box node
  osg::ref_ptr<osg::MatrixTransform> leftxform = new osg::MatrixTransform();
  leftxform->setName("MK_OSG_CTS_separator_leftxform");
  leftxform->addChild( leftgeo.get() );
  leftxform->setMatrix( osg::Matrix::translate( osg::Vec3(-0.5*bgbox.width()+0.5*leftbox.width(),
                                                          0.0,
                                                          0.0) ) );

  // make the colored stripe geometry
  Detail::FlatBox stripe(_text*(this->separator().height()),
                         this->box().width()-this->box().height()-2.0*_border-_margin,
                         -0.001);
  stripe.color( this->theme()["text"] );
  osg::ref_ptr<osg::Drawable> stripedraw = stripe();

  // add the stripe geometry to a node
  osg::ref_ptr<osg::Geode> stripegeo = new osg::Geode();
  stripegeo->setName("MK_OSG_CTS_separator_stripe_geode");
  stripegeo->addDrawable( stripedraw.get() );

  // move the stripe node
  osg::ref_ptr<osg::MatrixTransform> xs = new osg::MatrixTransform();
  xs->setName("MK_OSG_CTS_separator_stripe_xform");
  xs->addChild( stripegeo.get() );

  // use the equation for a stripe, offset for the left box
  xs->setMatrix( osg::Matrix::translate( osg::Vec3(0.5*leftbox.width()+_border+0.5*_margin,
                                                   0.0,0.0) ) );

  // add the parts
  osg::MatrixTransform* group = new osg::MatrixTransform();
  group->setName("MK_OSG_CTS_separator_xform");
  group->addChild( bggeo.get() );
  group->addChild( leftxform.get() );
  group->addChild( xs.get() );

  // position the graphic
  group->setMatrix( osg::Matrix::translate( osg::Vec3(0.5*bgbox.width(),
                                                      -0.5*bgbox.height(),
                                                      0.0) ) );
  return group;
}

template<typename ThemeMap>
float VisualThemeSkin<ThemeMap>::height(const Menu& m)
{
  if( m.separator() )
    return( this->separator().height() );
  else
    return( box().height() ); // TODO: is this the best thing to do?
}

template<typename ThemeMap>
float VisualThemeSkin<ThemeMap>::height(const Button& b)
{
  if( b.separator() )
    return( this->separator().height() );
  else
    return( box().height() ); // TODO: is this the best thing to do?
}

template<typename ThemeMap>
float VisualThemeSkin<ThemeMap>::width(const Menu& m)
{
  return( item_width( m.text() , m.parent() ) );
}

template<typename ThemeMap>
float VisualThemeSkin<ThemeMap>::width(const Button& b)
{
  return( item_width(b.text(), b.parent()) );
}

template<typename ThemeMap>
float VisualThemeSkin<ThemeMap>::width(const Item* i)
{
  return( item_width(i->text(), i->parent()) );
}

template<typename ThemeMap>
float VisualThemeSkin<ThemeMap>::item_width(const std::string& s,const Menu* parent)
{
  // check for text content
  if( s.empty() )
    return( 0.0 );

  // use graphics engine to calculate width of displayed text
  float wordwidth = this->word_width( s );

  // check for top-level item
  if( !parent )
    return( wordwidth + 2.0*(_margin+_border) );

  // --- LAYOUT correcting --- //

  // the eqation for the width of a horizontal graphic
  // 2 margins,   2 x _margin
  // 1 text box,  1 x graphic text width
  if( parent->layout() == Menu::HORIZONTAL )
    return( wordwidth + 2.0*(_margin+_border) );

  /// TODO: When calculating this equation, include more spacing between pieces with _margin for better appearance, and use this new distance for positioning in graphic creation.
  // equation for the width of a vertical graphic:
  // 2 side boxes: 2 x _box's width (which is the height of the box - 2 x margin)
  // 1 text box:   1 x  graphic text width
  // 2 margins:    2 x _margin
  else
    return( 2.0*(this->box().height()+_border+_margin) + wordwidth );
}

template<typename ThemeMap>
float VisualThemeSkin<ThemeMap>::word_width(const std::string& s)
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
