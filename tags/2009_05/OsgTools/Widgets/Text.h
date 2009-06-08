
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
#include "OsgTools/Widgets/Item.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <string>

namespace osg { class Node; }
namespace osgText { class Text; }

namespace OsgTools {
namespace Widgets {

class OSG_TOOLS_EXPORT Text : public OsgTools::Widgets::Item
{
public:
  typedef OsgTools::Widgets::Item BaseClass;
  typedef float ValueType;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Text );

  /// Constructors.
  Text();
  Text( const std::string& text );

  /// Horizontal Alignment.
  enum Alignment
  {
    LEFT,
    RIGHT
  };
  
  /// Vertical Alignment.
  enum VAlignment
  {
    TOP,
    CENTER,
    BOTTOM
  };

  /// Set/get the horizontal alignment.
  void                    alignmentHorizontal ( Alignment type );
  Alignment               alignmentHorizontal() const;
  
  /// Set/get the vertical alignment.
  void                    alignmentVertical ( VAlignment type );
  VAlignment              alignmentVertical() const;
  
  /// Set/get the auto size.
  void                    autoSize ( bool b );
  bool                    autoSize() const;

  /// Build the scene.
  osg::Node*              buildScene ( unsigned int depth = 0 );
  
  /// Estimate the needed size for the item.
  virtual Size            estimateSizeForWidth ( unsigned int w ) const;
  
  /// Set/get the font size.
  void                    fontSize ( double size );
  double                  fontSize() const;
  
  /// Get/Set the text.
  void                    text( const std::string& text );
  const std::string &     text () const;
  
  /// Set/get the border color.
  void                    textColor ( const Color& );
  Color                   textColor() const;

  /// Set/get the line wrap state.
  void                    wrapLine ( bool b );
  bool                    wrapLine() const;
  
protected:
  
  virtual ~Text();
  
  osgText::Text*          _makeText ( unsigned int depth ) const;

  /// Get x and y position on text based on alignment settings.
  ValueType               _xTextPosition ( unsigned int width ) const;
  ValueType               _yTextPosition ( unsigned int height ) const;
  
private:
  std::string _text;
  Alignment _alignment;
  VAlignment _alignmentVertical;
  bool _wrapLine;
  bool _autoSize;
  double _fontSize;
  Color _textColor;
};

}
}


#endif //__OSG_TOOLS_LEGEND_TEXT_H__
