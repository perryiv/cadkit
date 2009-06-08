
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

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
      * This class might easily be rewritten as just an implementation
      * of extending STL's map container template.  Then it would not
      * be so specific to 4-d vector types as the constructor here has
      * limited the class to such a template argument.  Then again,
      * it will still be necessary to provide smart initial values,
      * and that was the original purpose of writing this class. :/
      */
    template<typename ColorType>
    class ColorTheme
    {
    public:
      typedef ColorType color_type;
      typedef std::map<std::string,ColorType> color_map;

      ColorTheme(): _colormap()
      {
        ///\todo TODO: optimize initial value assigment with the map's insert method, low priority
        _colormap["text"] = color_type(0.0f,0.0f,0.0f,1.0f);        // black
        _colormap["middle"] = color_type(0.5f,0.5f,0.5f,1.0f);      // gray
        _colormap["border"] = color_type(0.0f,0.0f,1.0f,1.0f);      // blue
        _colormap["horizontal_background"] = color_type(0.9f,0.9f,0.9f,1.0f);  // gray
        _colormap["vertical_background"] = color_type(0.9f,0.9f,0.9f,1.0f);  // light gray
        _colormap["special"] = color_type(0.2f,0.2f,0.6f,1.0f);     // dark gray
      }

      ColorTheme(const ColorTheme& bt): _colormap(bt._colormap) {}
      ColorTheme(const color_map& cm): _colormap(cm) {}
      virtual ~ColorTheme() {}
      ColorTheme& operator= (const ColorTheme& bt) { _colormap = bt._colormap; return( *this ); }

      const ColorType& operator [](const std::string& key) const
      {
        color_map::const_iterator iter = _colormap.find( key );
        if( iter == _colormap.end() ) /// TODO: use Perry's exception code here
          assert( 0 );

        return iter->second;
      }

      void set_map(const color_map& m) { _colormap = m; }
      const color_map& get_map() const { return _colormap; }
      color_map& get_map()             { return _colormap; }

    private:
      color_map _colormap;
    };

  };

};

#endif
