//
// Author: John K. Grant
// All rights reserved.
//

#ifndef _menukit_osg_colortheme_h_
#define _menukit_osg_colortheme_h_

#include "osg/Vec4"
#include <map>

namespace MenuKit
{

  namespace OSG
  {

    /** ColorTheme
      * The ColorTheme class is a simple convenience class encapsulating
      * a "theme" (like a map of colors) for other (more complicated)
      * classes to use as an object member.  It uses a std::map<>
      * with a 4D (color) vector type as the template argument.
      * It could be replaced with only the map, but this class helps
      * to provide good initial color values for the map.
      *
      * This class could easily be rewritten as just an implementation
      * of extending STL's map container template.  Then it would not
      * be so specific to 4-d vector types as the constructor here has
      * limited the class to such a template argument.  Then again,
      * it will still be necessary to provide smart initial values,
      * and that was the original purpose of writing this class. :/
      */
    template<class ColorType>
    class ColorTheme
    {
      /// TODO: rename class to 'Theme' because our definition of "theme" is just a color map
    public:
      typedef std::map<std::string,ColorType> ColorMap;
      ColorTheme(): _colormap()
      {
        /// TODO: optimize initial value assigment with an algorithm
        _colormap["text"] = ColorType(0.0f,0.0f,0.0f,1.0f);
        _colormap["middle"] = ColorType(0.5f,0.5f,0.5f,1.0f);
        _colormap["border"] = ColorType(0.0f,0.0f,0.8f,1.0f);
        _colormap["background"] = ColorType(0.7f,0.7f,0.8f,1.0f);
        _colormap["special"] = ColorType(0.2f,0.2f,0.6f,1.0f);
      }

      ColorTheme(const ColorTheme& bt): _colormap(bt._colormap) {}
      ColorTheme(const ColorMap& cm): _colormap(cm) {}
      virtual ~ColorTheme() {}
      ColorTheme& operator= (const ColorTheme& bt) { _colormap = bt._colormap; return( *this ); }

      const ColorType& operator [](const std::string& key) const
      {
        ColorMap::const_iterator iter = _colormap.find( key );
        if( iter == _colormap.end() ) /// TODO: use Perry's exception code here
          assert( 0 );

        return iter->second;
      }

      void color_map(const ColorMap& m) { _colormap = m; }
      const ColorMap& color_map() const { return _colormap; }
      ColorMap& color_map() { return _colormap; }

    private:
      ColorMap _colormap;
    };

    typedef ColorTheme<osg::Vec4> osgColorTheme;
  };

};

#endif
