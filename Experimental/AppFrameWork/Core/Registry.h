
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

#include <string>

namespace AFW { namespace Core { class Frame; class Window; } }
namespace XmlTree { class Document; }


namespace AFW {
namespace Core {


class APP_FRAME_WORK_EXPORT Registry : public Object
{
public:

  // Typedefs.
  typedef Object BaseClass;

  // Flush changes back to disk.
  void                                flush();

  // Initialize the registry.
  virtual void                        init ( const std::string &vendor, const std::string &name );

  // Write the value.
  virtual void                        writeValue ( const std::string &section, const std::string &key, bool );
  virtual void                        writeValue ( const std::string &section, const std::string &key, int );
  virtual void                        writeValue ( const std::string &section, const std::string &key, unsigned int );
  virtual void                        writeValue ( const std::string &section, const std::string &key, const std::string & );
  virtual void                        writeValue ( const std::string &section, const AFW::Core::Types::Position & );
  virtual void                        writeValue ( const std::string &section, const AFW::Core::Types::Size & );

  // Write window geometry.
  virtual void                        writeGeometry ( const AFW::Core::Frame * );
  virtual void                        writeGeometry ( const AFW::Core::Window * );

protected:

  // Constructor
  Registry();

  // Use reference counting.
  virtual ~Registry();

private:

  // No copying.
  Registry ( const Registry & );
  Registry &operator = ( const Registry & );

  XmlTree::Document *_xml;
  std::string _file;
  bool _dirty;

  AFW_DECLARE_OBJECT ( Registry );
};


} // namespace Core
} // namespace AFW


#endif //_APP_FRAME_WORK_REGISTRY_CLASS_H_
