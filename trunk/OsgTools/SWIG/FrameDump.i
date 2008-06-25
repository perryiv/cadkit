%{
#include "OsgTools/Render/FrameDump.h"
%}

#ifdef SWIGPYTHON
%include "python/std_string.i"
#else
%include "std_string.i"
#endif

namespace OsgTools {
  namespace Render {

  class FrameDump
  {
  public:
    FrameDump();
    ~FrameDump();
    
    //const std::string &       base() const;
    void                      base ( const std::string &b );
    
    //unsigned int              digits() const;
  	void                      digits ( unsigned int d );

  	//const std::string &       dir() const;
  	void                      dir ( const std::string &d );

  	void                      dump ( bool state );
  	//bool                      dump() const;
  	
  	//const std::string &       ext() const;
  	void                      ext ( const std::string &e );

	//float                     scale() const;
  	void                      scale ( float p );
    
    //unsigned int              start() const;
  	void                      start ( unsigned int s );
  };
  
  }
}
