
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Policy classes for when the requested child node is not found.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _XML_DOM_MISSING_CHILD_POLICY_CLASS_H_
#define _XML_DOM_MISSING_CHILD_POLICY_CLASS_H_


namespace XML {
namespace Config {


///////////////////////////////////////////////////////////////////////////////
//
//  Create missing children.
//
///////////////////////////////////////////////////////////////////////////////

struct CreateMissing
{
  bool operator()() const
  {
    return true;
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Don't create missing children.
//
///////////////////////////////////////////////////////////////////////////////

struct NoCreateMissing
{
  bool operator()() const
  {
    return false;
  }
};


}; // namespace Config
}; // namespace XML


#endif // _XML_DOM_MISSING_CHILD_POLICY_CLASS_H_