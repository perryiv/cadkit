
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_FACTORY_READERS_H__
#define __MINERVA_CORE_FACTORY_READERS_H__

#include "Minerva/Core/Export.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include <string>
#include <map>
#include <vector>

namespace Minerva {
namespace Core {
namespace Factory {

  
  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Base creator class.
  //
  ///////////////////////////////////////////////////////////////////////////////
  
  class BaseCreator : public Usul::Base::Object
  {
  public:
    
    typedef Usul::Base::Object BaseClass;
    
    USUL_DECLARE_REF_POINTERS ( BaseCreator );
    
    virtual Usul::Interfaces::IUnknown  *  operator()() = 0;
    
  protected:
    
    BaseCreator () : BaseClass () { }
    
    virtual ~BaseCreator() { }
  };
  
  
class MINERVA_EXPORT Readers
{
public:
  //typedef Usul::Factory::BaseCreator BaseCreator;
  typedef std::pair < std::string, std::string > Filter;
  typedef std::vector<Filter> Filters;
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  
  /// Get the instance.
  static Readers& instance();

  /// Add a creator.
  void                              add ( const std::string &filter, const std::string& extension, BaseCreator *creator );
  
  /// Remove a creator.
  void                              remove ( const std::string &filter, const std::string& extension );

  /// Create.
  Usul::Interfaces::IUnknown *      create ( const std::string &extension );

  /// Get all filters.
  Filters                           filters() const;
private:
  Readers();
  ~Readers();

  typedef std::map < std::string, BaseCreator::ValidRefPtr > Creators;

  static Readers *_instance;
  mutable Mutex *_mutex;
  Filters _filters;
  Creators _creators;
};

  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Concrete creator class.
  //
  ///////////////////////////////////////////////////////////////////////////////
  
  template < class T > class TypeCreator : public BaseCreator 
  {
  public:
    USUL_DECLARE_REF_POINTERS ( TypeCreator );
    typedef BaseCreator BaseClass;
    typedef T ObjectType;
      
    TypeCreator () : BaseClass ()
    {
    }
      
    virtual Usul::Interfaces::IUnknown *operator()()
    {
      Usul::Interfaces::IUnknown::QueryPtr unknown ( new T );
      return unknown.release();
    }
      
  protected:
      
    virtual ~TypeCreator()
    {
    }
  };
  

  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Class for automatically registering readers.
  //
  ///////////////////////////////////////////////////////////////////////////////
  
  template < class ReaderType > struct RegisterReader
  {
    RegisterReader ( const std::string &filter, const std::string& extension ) : _filter ( filter ), _extension ( extension )
    {
      Readers::instance().add ( filter, extension, new ReaderType );
    }
    ~RegisterReader()
    {
      Readers::instance().remove ( _filter, _extension );
    }
    
  private:
    std::string _filter;
    std::string _extension;
  };
  
}
}
}

#define MINERVA_FACTORY_REGISTER_READER(the_filter,the_extension,the_type)\
namespace { Minerva::Core::Factory::RegisterReader < Minerva::Core::Factory::TypeCreator < the_type > > _creator_for_##the_type (the_filter, the_extension ); }

#endif // __MINERVA_CORE_FACTORY_READERS_H__
