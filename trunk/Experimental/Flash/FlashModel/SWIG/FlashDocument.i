
%module FlashDocument
%{
#include "FlashDocument.h"
%}

//%include "std/std_string.i"

class FlashDocument
{
public:
  FlashDocument();
  
  void read ( const std::string&, Usul::Interfaces::IUnknown*, Usul::Interfaces::IUnknown* );
  
  //virtual osg::Node *         buildScene ( const FlashDocument::Options &options, Usul::Interfaces::IUnknown* caller );
  
  /// Usul::Interfaces::ITimeVaryingData
  void                setCurrentTimeStep ( unsigned int current );
  unsigned int        getCurrentTimeStep () const;
  
  unsigned int        getNumberOfTimeSteps () const;
  
protected:
  virtual ~FlashDocument();
};

namespace Usul {
  namespace Pointers {
    
    
    template
    <
    class T,
    class Config_
    >
    struct SmartPointer
    {
    public:
      SmartPointer();
      ~SmartPointer();
    };
  }
}

%template(FlashDocumentPtr) Usul::Pointers::SmartPointer<FlashDocument,Usul::Pointers::Configs::RefCountingNullOk>;

%{

static FlashDocument* createDocument( char* filename )
{
  FlashDocument::RefPtr doc ( new FlashDocument );
  doc->read ( filename, 0x0, 0x0 );
	return doc.release();
}

static osg::Node* buildScene ( FlashDocument* doc )
{
  if ( 0x0 != doc )
  	return doc->buildScene( FlashDocument::BaseClass::Options() );
  
  return 0x0;
}

%}

FlashDocument* createDocument( char* filename );
osg::Node* buildScene ( FlashDocument* );
