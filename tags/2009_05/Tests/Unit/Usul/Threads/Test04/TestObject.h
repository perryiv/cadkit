
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Test object.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_TEST_OBJECT_CLASS_H_
#define _USUL_TEST_OBJECT_CLASS_H_

#include "Usul/Base/BaseObject.h"


class TestObject : public Usul::Base::BaseObject
{
public:

  typedef Usul::Base::BaseObject BaseClass;

  USUL_DECLARE_REF_POINTERS ( TestObject );
  USUL_DECLARE_TYPE_ID ( TestObject );

  TestObject();

  unsigned int      getData() const;
  void              setData ( unsigned int );

  void              exercise();

protected:

  virtual ~TestObject();

private:

  unsigned int _data;
};


#endif // _USUL_TEST_OBJECT_CLASS_H_
