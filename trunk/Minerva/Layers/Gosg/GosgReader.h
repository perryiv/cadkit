
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __GOSG_READER_H__
#define __GOSG_READER_H__

#include "Minerva/Core/Data/DataObject.h"

#include "Usul/Interfaces/IRead.h"

#include <vector>


class GosgReader : public Minerva::Core::Data::DataObject,
                   public Usul::Interfaces::IRead
                 
{
public:

  /// Typedefs.
  typedef Minerva::Core::Data::DataObject BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( GosgReader );
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  GosgReader();
  
  // Read the file.
  virtual void read ( const std::string &filename, Usul::Interfaces::IUnknown *caller = 0x0, Usul::Interfaces::IUnknown *progress = 0x0 );
  
protected:

  virtual ~GosgReader();
};


#endif // __GOSG_READER_H__
