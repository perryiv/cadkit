
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Registry class for persistent application data.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_REGISTRY_CLASS_H_
#define _APP_FRAME_WORK_REGISTRY_CLASS_H_

#include "AppFrameWork/Core/Object.h"
#include "AppFrameWork/Core/Types.h"

namespace AFW { namespace Core { class Frame; class Window; } }
namespace XmlTree { class Root; }


namespace AFW {
namespace Core {


class APP_FRAME_WORK_EXPORT Registry : public Object
{
public:

  // Typedefs.
  typedef Object BaseClass;

  // Write the value.
  void                                writeValue ( const std::string &section, const std::string &key, bool );
  void                                writeValue ( const std::string &section, const std::string &key, int );
  void                                writeValue ( const std::string &section, const std::string &key, unsigned int );
  void                                writeValue ( const std::string &section, const std::string &key, const std::string & );
  void                                writeValue ( const std::string &section, const AFW::Core::Types::Position & );
  void                                writeValue ( const std::string &section, const AFW::Core::Types::Size & );

  // Write window geometry.
  void                                writeGeometry ( const AFW::Core::Frame * );
  void                                writeGeometry ( const AFW::Core::Window * );

protected:

  // Constructor
  Registry();

  // Use reference counting.
  virtual ~Registry();

  void                                _init();

private:

  // No copying.
  Registry ( const Registry & );
  Registry &operator = ( const Registry & );

  XmlTree::Root *_xml;

  AFW_DECLARE_OBJECT ( Registry );
};


} // namespace Core
} // namespace AFW


#endif //_APP_FRAME_WORK_REGISTRY_CLASS_H_
