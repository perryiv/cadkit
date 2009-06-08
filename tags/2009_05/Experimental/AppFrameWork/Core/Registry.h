
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

namespace AFW { namespace Windows { class Frame; class Window; } }
namespace XmlTree { class Document; }


namespace AFW {
namespace Core {


class APP_FRAME_WORK_EXPORT Registry : public Object
{
public:

  // Typedefs.
  typedef Object BaseClass;
  typedef AFW::Core::Types::Position Position;
  typedef AFW::Core::Types::Size Size;

  // Flush changes back to disk.
  void                                flush();

  // Initialize the registry.
  virtual void                        init ( const std::string &vendor, const std::string &name );

  // Read the value.
  virtual bool                        readValue ( const std::string &section, const std::string &key, bool defaultValue );
  virtual int                         readValue ( const std::string &section, const std::string &key, int defaultValue );
  virtual unsigned int                readValue ( const std::string &section, const std::string &key, unsigned int defaultValue );
  virtual std::string                 readValue ( const std::string &section, const std::string &key, const std::string &defaultValue );
  virtual Position                    readValue ( const std::string &section, const Position &defaultValue );
  virtual Size                        readValue ( const std::string &section, const Size &defaultValue );

  // Write the value.
  virtual void                        writeValue ( const std::string &section, const std::string &key, bool );
  virtual void                        writeValue ( const std::string &section, const std::string &key, int );
  virtual void                        writeValue ( const std::string &section, const std::string &key, unsigned int );
  virtual void                        writeValue ( const std::string &section, const std::string &key, const std::string & );
  virtual void                        writeValue ( const std::string &section, const Position & );
  virtual void                        writeValue ( const std::string &section, const Size & );

  // Write window geometry.
  virtual void                        writeGeometry ( const AFW::Windows::Frame * );
  virtual void                        writeGeometry ( const AFW::Windows::Window * );

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
