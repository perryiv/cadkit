
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base file class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FILE_BASE_FILE_CLASS_H_
#define _USUL_FILE_BASE_FILE_CLASS_H_

#include "Usul/Base/BaseObject.h"
#include "Usul/Types/Types.h"

#include <string>


namespace Usul {
namespace File {


class USUL_EXPORT BaseFile : public Usul::Base::BaseObject
{
public:

  // Useful typedefs.
  typedef Usul::Base::BaseObject BaseClass;
  typedef Usul::Types::Uint64 SizeType;

  // Type information.
  USUL_DECLARE_TYPE_ID ( BaseFile );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( BaseFile );

  // Constructor.
  BaseFile ( const std::string &name );

  // Get the file name.
  std::string                 name() const;

  // Get the size of the file.
  SizeType                    size() const;
  static SizeType             size ( const std::string &file );

protected:

  // Use reference counting.
  virtual ~BaseFile();

private:

  // No copying or assignment.
  BaseFile ( const BaseFile & );
  BaseFile &operator = ( const BaseFile & );

  std::string _name;
};


} // namespace File
} // namespace Usul


#endif // _USUL_FILE_BASE_FILE_CLASS_H_
