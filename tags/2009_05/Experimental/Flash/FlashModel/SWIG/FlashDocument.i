
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

%module FlashDocument
%{
#include "FlashDocument.h"
%}

%include "Timestep.i"

#ifdef SWIGPYTHON
%include "python/std_string.i"
%include "python/std_map.i"
#else
%include "std_string.i"
#endif

%template(Options) std::map<std::string,std::string>;

class FlashDocument
{
public:
  FlashDocument();
  
  // Read/Write.
  void                read  ( const std::string& filename, Usul::Interfaces::IUnknown*, Usul::Interfaces::IUnknown* );
  void                write ( const std::string& filename, Usul::Interfaces::IUnknown *, Usul::Interfaces::IUnknown * ) const;
  
  /// Build the scene.
  osg::Node *         buildScene ( const std::map<std::string,std::string> &options, Usul::Interfaces::IUnknown* caller );
  
  /// Set/get the draw bounding box flag.
  void                drawBBox ( bool b );
  bool                isDrawBBox() const;
  
  /// Set/get the draw points flag.
  void                drawPoints ( bool b );
  bool                isDrawPoints() const;
  
  /// Set/get the draw volume flag.
  void                drawVolume ( bool b );
  bool                isDrawVolume() const;
  
  /// Usul::Interfaces::ITimeVaryingData
  void                setCurrentTimeStep ( unsigned int current );
  unsigned int        getCurrentTimeStep () const;
  
  unsigned int        getNumberOfTimeSteps () const;
  
  /// Load the i'th timestep.
  Usul::Pointers::SmartPointer<Timestep,Usul::Pointers::Configs::RefCountingNullOk>         loadTimestep ( unsigned int i, bool cache );
  
  void                updateNotify ( Usul::Interfaces::IUnknown* );
  
protected:
  virtual ~FlashDocument();
};

%include "Usul/SWIG/SmartPointer.i"

%template(FlashDocumentPtr) Usul::Pointers::SmartPointer<FlashDocument,Usul::Pointers::Configs::RefCountingNullOk>;
//%template(FlashDocumentPtr) FlashDocument::RefPtr;

%{

static Usul::Pointers::SmartPointer<FlashDocument,Usul::Pointers::Configs::RefCountingNullOk> createDocument()
{
  FlashDocument::RefPtr doc ( new FlashDocument );
	return doc;
}

%}

Usul::Pointers::SmartPointer<FlashDocument,Usul::Pointers::Configs::RefCountingNullOk> createDocument();
