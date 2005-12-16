
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Frame class to arrange child windows.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_FRAME_CLASS_H_
#define _APP_FRAME_WORK_FRAME_CLASS_H_

#include "AppFrameWork/Core/Group.h"


namespace AFW {
namespace Core {


class APP_FRAME_WORK_EXPORT Frame : public Group
{
public:

  // Typedefs.
  typedef Group BaseClass;

  // Possible layouts.
  enum ChildLayout
  {
    HORIZONTAL,
    VERTICAL
  };

  // Accept the visitor.
  virtual void                        accept ( AFW::Core::BaseVisitor * );

  // Set/get the child layout.
  void                                layout ( ChildLayout );
  ChildLayout                         layout();

protected:

  // Constructor
  Frame();

  // Use reference counting.
  virtual ~Frame();

private:

  // No copying.
  Frame ( const Frame & );
  Frame &operator = ( const Frame & );

  ChildLayout _layout;

  AFW_DECLARE_OBJECT ( Frame );
};


} // namespace Core
} // namespace AFW


#endif //_APP_FRAME_WORK_FRAME_CLASS_H_
