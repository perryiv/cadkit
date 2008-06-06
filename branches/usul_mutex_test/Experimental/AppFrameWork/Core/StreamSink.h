
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  For redirecting stdio and stderr.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_STANDARD_STREAM_SINK_CLASS_H_
#define _APP_FRAME_WORK_STANDARD_STREAM_SINK_CLASS_H_

#include "AppFrameWork/Core/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <string>

namespace AFW { namespace Core { namespace Detail { struct TwoSinks; struct CharSink; } } }


namespace AFW {
namespace Core {


class APP_FRAME_WORK_EXPORT StreamSink : public Usul::Base::Referenced
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( StreamSink );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( StreamSink );

  // Constructor.
  StreamSink ( const std::string &file );

protected:

  // Use reference counting.
  virtual ~StreamSink();

private:

  // No copying.
  StreamSink ( const StreamSink & );
  StreamSink &operator = ( const StreamSink & );

  AFW::Core::Detail::TwoSinks *_twoSinks;
  AFW::Core::Detail::CharSink *_charSink;
};


} // namespace Core
} // namespace AFW


#endif //_APP_FRAME_WORK_STANDARD_STREAM_SINK_CLASS_H_
