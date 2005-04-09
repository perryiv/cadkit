//
// Author: John K. Grant
// All rights reserved.
//

#ifndef _menukit_osg_colorthemetile_h_
#define _menukit_osg_colorthemetile_h_

#include "TileFunctor.h"   // baseclass
#include "osg/Vec4"        // for typedef
#include <map>             // member type

namespace MenuKit
{

  namespace OSG
  {
    /** ThemeTile
      * The ThemeTile extends then TileFunctor by
      * adding a ThemeType member.  The class itself
      * is still meant to be extended by implementing      
      * the 'width' and 'height' memeber functions.
      */
    class ThemeTile : public TileFunctor
    {
    public:
      typedef TileFunctor base_class;
      typedef ThemeMap theme_type;
      typedef ThemeTile<theme_type> thisclass;
      MENUKIT_DECLARE_POINTER ( thisclass );

      typedef std::map<typename base_class::display_mode,theme_type> DisplayModeThemeMap;

      ThemeTile(): base_class(), _thememap()
      {
      }

      void theme_map(const DisplayModeThemeMap& t) { _thememap = t; }
      const DisplayModeThemeMap& theme_map() const { return _thememap; }
      DisplayModeThemeMap& theme_map() { return _thememap; }

    protected:
      virtual ~ThemeTile() {}

      virtual const theme_type& _proper_theme() const
      {
        DisplayModeThemeMap::const_iterator iter( _thememap.find( this->mode() ) );
        if( iter == _thememap.end() ) ///\todo TODO: use Perry's exception code here
          assert( 0 );
        return iter->second;
      }

    private:

      ///\todo TODO: evaluate if this class is copyable
      // not implemented by design
      ThemeTile(const ThemeTile& tt);//: base_class(tt), _thememap(tt._thememap)
      //{}

      ThemeTile& operator= (const ThemeTile& tt);
      //{
      //  base_class::operator=(tt);
      //  _thememap = tt._thememap;
      //}

      DisplayMode _mode;
    };

  };

};

#endif
