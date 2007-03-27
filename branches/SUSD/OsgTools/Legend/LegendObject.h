
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_LEGEND_OBJECT_H__
#define __OSG_TOOLS_LEGEND_OBJECT_H__

#include "OsgTools/Export.h"
#include "OsgTools/Legend/Icon.h"
#include "OsgTools/Legend/Text.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

namespace OsgTools {
namespace Legend {

class OSG_TOOLS_EXPORT LegendObject : public Usul::Base::Referenced
{
public:
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( LegendObject );

  LegendObject();

  /// Get/Set the icon.
  void                   icon( Icon* );
  Icon*                  icon();
  const Icon*            icon() const;

  /// Get/Set the text.
  void                   text( Text* );
  Text*                  text();
  const Text*            text() const;

  osg::Node*             buildScene();

  /// Set the size.
  void                 size( unsigned int width, unsigned int height );

protected:
  virtual ~LegendObject();

private:
  Icon::RefPtr _icon;
  Text::RefPtr _text;

  unsigned int _width;
  unsigned int _height;
};

}
}


#endif //__OSG_TOOLS_LEGEND_OBJECT_H__
