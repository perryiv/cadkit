
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

%{
#include "Dataset.h"
%}

%include "Usul/SWIG/SmartPointer.i"

class Dataset
{
public:
  Dataset( hid_t FileHandle, const std::string& name );
  
protected:
  virtual ~Dataset();
  
};

%template(DatasetPtr) Usul::Pointers::SmartPointer<Dataset,Usul::Pointers::Configs::RefCountingNullOk>;
