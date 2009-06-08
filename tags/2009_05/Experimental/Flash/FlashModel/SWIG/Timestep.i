
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

%{
#include "Timestep.h"
%}

%include "Usul/SWIG/SmartPointer.i"
%include "Dataset.i"

%include "std_string.i"

class Timestep
{
public:
  Timestep( const std::string& );
  
  /// Load the data with the given name.
  void loadData ( const std::string& name );
  
  /// Get the min and max values.
  double minimum() const;
  double maximum() const;
  
protected:
  virtual ~Timestep();
};

%template(TimestepPtr) Usul::Pointers::SmartPointer<Timestep,Usul::Pointers::Configs::RefCountingNullOk>;
