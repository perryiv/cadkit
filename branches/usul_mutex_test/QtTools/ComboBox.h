
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __QT_TOOLS_COMBO_BOX_H__
#define __QT_TOOLS_COMBO_BOX_H__

#include "Usul/Strings/Qt.h"

#include "QtGui/QComboBox"

#include <algorithm>


namespace QtTools {
namespace ComboBox {


template < class StringList >
inline void populate ( QComboBox& comboBox, const StringList& strings, const typename StringList::value_type& selected = "" )
{
  QStringList sl;
  Usul::Strings::convertFrom ( strings, sl );
  comboBox.addItems ( sl );
  
  QStringList::iterator iter ( std::find ( sl.begin(), sl.end(), selected.c_str() ) );
  int index ( iter != sl.end() ? std::distance ( sl.begin(), iter ) : -1 );
  comboBox.setCurrentIndex ( index );
}


}
}


#endif // __QT_TOOLS_COMBO_BOX_H__
