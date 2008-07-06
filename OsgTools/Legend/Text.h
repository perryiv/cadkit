
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_LEGEND_TEXT_H__
#define __OSG_TOOLS_LEGEND_TEXT_H__

#include "OsgTools/Export.h"
#include "OsgTools/Legend/Item.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <string>

namespace osg { class Node; }

namespace OsgTools {
namespace Legend {

class OSG_TOOLS_EXPORT Text : public OsgTools::Legend::Item
{
public:
  typedef OsgTools::Legend::Item BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Text );

  /// Constructors.
  Text();
  Text( const std::string& text );

  /// Alignment.
  enum Alignment
  {
    LEFT,
    RIGHT
  };

  /// Get/Set the alignment.
  void                    alignment ( Alignment type );
  Alignment               alignment () const;

  /// Build the scene.
  osg::Node*              buildScene();
  
  /// Set/get the font size.
  void                    fontSize ( double size );
  double                  fontSize() const;
  
  /// Get/Set the text.
  void                    text( const std::string& text );
  const std::string &     text () const;

  /// Set/get the line wrap state.
  void                    wrapLine ( bool b );
  bool                    wrapLine() const;
  
protected:
  virtual ~Text();

private:
  std::string _text;
  Alignment _alignment;
  bool _wrapLine;
  double _fontSize;
};

}
}


#endif //__OSG_TOOLS_LEGEND_TEXT_H__
