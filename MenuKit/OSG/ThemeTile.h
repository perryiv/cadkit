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
    template<class ThemeType>
    class ThemeTile : public TileFunctor
    {
    public:
      typedef TileFunctor BaseClass;
      typedef std::map<BaseClass::DisplayMode,ThemeType> ThemeMap;

      ThemeTile(): BaseClass(), _thememap()
      {
        ThemeType simple;
        _thememap[BaseClass::DISABLED] = simple;
        _thememap[BaseClass::HIGHLIGHT] = simple;
        _thememap[BaseClass::NORMAL] = simple;
      }

      ThemeTile(const ThemeTile& tt): BaseClass(tt), _thememap(tt._thememap)
      {}

      ThemeTile& operator= (const ThemeTile& tt)
      {
        BaseClass::operator=(tt);
        _thememap = tt._thememap;
      }

      virtual ~ThemeTile() {}

      virtual const ThemeType& proper_theme() const
      {
        BaseClass::DisplayMode m = this->mode();
        ThemeMap::const_iterator iter = _thememap.find( m );

        if( iter == _thememap.end() ) /// TODO: use Perry's exception code here
          assert( 0 );

        return iter->second;
      }

      void theme_map(const ThemeMap& t) { _thememap = t; }
      const ThemeMap& theme_map() const { return _thememap; }

    private:
      ThemeMap _thememap;
    };

    typedef ThemeTile<osg::Vec4> osgThemeTile;
  };

};

#endif
