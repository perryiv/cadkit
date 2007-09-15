
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Tool pair.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_TOOL_PAIR_CALLBACK_H_
#define _CV_TOOL_PAIR_CALLBACK_H_

#include "CadViewer/Functors/Tool.h"


namespace CV {
namespace Functors {


  class ToolPair : public Usul::Functors::Interaction::Common::BaseFunctor
{
public:

  // Useful typedef(s).
  USUL_DECLARE_REF_POINTERS ( ToolPair );
  typedef Usul::Functors::Interaction::Common::BaseFunctor BaseClass;
  typedef BaseClass::Unknown Unknown;
  typedef Tool::Transforms Transforms;

  ToolPair ( 
    Unknown *unknown, 
    Tool *c1, 
    Tool *c2,
    const std::string& name );
  ToolPair ( const ToolPair &tp );

  // Have the Tools do their thing.
  virtual void          operator()();

  // Set the transforms.
  void                  transforms ( const Transforms &t );

protected:

  virtual ~ToolPair();

  Tool::ValidRefPtr _tool1;
  Tool::ValidRefPtr _tool2;
};


}; // namespace Functors
}; // namespace CV


#endif // _CV_TOOL_PAIR_CALLBACK_H_
