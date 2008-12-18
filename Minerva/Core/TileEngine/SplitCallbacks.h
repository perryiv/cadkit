
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Callbacks that determine tile splitting behavior.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_SPLIT_CALLBACKS_H__
#define __MINERVA_CORE_SPLIT_CALLBACKS_H__

#include "Minerva/Core/Export.h"

#include "Serialize/XML/Macros.h"

#include "Usul/Base/Object.h"

namespace Minerva { namespace Core { namespace TileEngine { class Tile; } } }


namespace Minerva {
namespace Core {
namespace TileEngine {
namespace Callbacks {


///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all split callbacks.
//
///////////////////////////////////////////////////////////////////////////////

class MINERVA_EXPORT SplitCallback : public Usul::Base::Object
{
public:

  typedef Usul::Base::Object BaseClass;
  typedef Minerva::Core::TileEngine::Tile Tile;

  USUL_DECLARE_REF_POINTERS ( SplitCallback );

  // Return true if the tile should split.
  virtual bool shouldSplit ( bool suggestion, Tile * ) = 0;

protected:

  // Constructor
  SplitCallback();

  // Use reference counting.
  virtual ~SplitCallback();

private:

  // No copying or assignment.
  SplitCallback ( const SplitCallback & );
  SplitCallback &operator = ( const SplitCallback & );

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS ( SplitCallback );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Simple pass-through split callback.
//
///////////////////////////////////////////////////////////////////////////////

class MINERVA_EXPORT PassThrough : public SplitCallback
{
public:

  typedef SplitCallback BaseClass;

  USUL_DECLARE_REF_POINTERS ( PassThrough );

  // Constructor
  PassThrough();

  // Return true if the tile should split.
  virtual bool shouldSplit ( bool suggestion, Tile * );

protected:

  // Use reference counting.
  virtual ~PassThrough();

  SERIALIZE_XML_DEFINE_MEMBERS ( PassThrough );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Callback that splits the tile until the given level is reached.
//
///////////////////////////////////////////////////////////////////////////////

class MINERVA_EXPORT SplitToLevel : public SplitCallback
{
public:

  typedef SplitCallback BaseClass;

  USUL_DECLARE_REF_POINTERS ( SplitToLevel );

  // Constructor
  SplitToLevel ( unsigned int maxLevel = 2 );

  // Return true if the tile should split.
  virtual bool shouldSplit ( bool suggestion, Tile * );

protected:

  // Use reference counting.
  virtual ~SplitToLevel();

private:

  unsigned int _maxLevel;

  SERIALIZE_XML_DEFINE_MEMBERS ( SplitToLevel );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Callback that will split if the level is less than the max.
//
///////////////////////////////////////////////////////////////////////////////

class MINERVA_EXPORT SplitIfLess : public SplitCallback
{
public:

  typedef SplitCallback BaseClass;

  USUL_DECLARE_REF_POINTERS ( SplitIfLess );

  // Constructor
  SplitIfLess ( unsigned int maxLevel = 20 );

  // Return true if the tile should split.
  virtual bool shouldSplit ( bool suggestion, Tile * );

protected:

  // Use reference counting.
  virtual ~SplitIfLess();

private:

  unsigned int _maxLevel;

  SERIALIZE_XML_DEFINE_MEMBERS ( SplitIfLess );
};


} // namespace Callbacks
} // namespace TileEngine
} // namespace Core
} // namespace Minerva


#endif // __MINERVA_CORE_SPLIT_CALLBACKS_H__
