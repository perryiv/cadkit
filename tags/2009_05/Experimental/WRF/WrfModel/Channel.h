
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __WRF_CHANNEL_H__
#define __WRF_CHANNEL_H__

#include "Serialize/XML/Macros.h"

#include "Usul/Base/Object.h"
#include "Usul/Pointers/Pointers.h"

class Channel : public Usul::Base::Object
{
public:
  typedef Usul::Base::Object BaseClass;

  USUL_DECLARE_REF_POINTERS ( Channel );

  Channel ();

  /// Get/Set the name of the channel.
  void                 name ( const std::string& name );
  const std::string&   name () const;

  /// Get/Set the index of the channel.
  void                 index ( unsigned int index );
  unsigned int         index () const;

  /// Get/Set the min value of the channel.
  void                 min ( double min );
  double               min () const;
  
  /// Get/Set the max value of the channel.
  void                 max ( double max );
  double               max () const;

protected:
  /// Use reference counting.
  virtual ~Channel ();

private:
  std::string _name;
  unsigned int _index;
  double _min;
  double _max;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS ( Channel );
};

#endif // __WRF_CHANNEL_H__
