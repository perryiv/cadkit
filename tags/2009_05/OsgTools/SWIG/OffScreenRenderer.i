//%module OsgTools
%{
#include "OsgTools/Render/OffScreenRenderer.h"
%}

%include "FrameDump.i"

#ifdef SWIGPYTHON
%include "python/boost_shared_ptr.i"
#endif

%template(FrameDumpPtr) boost::shared_ptr<OsgTools::Render::FrameDump>;

namespace OsgTools {
  namespace Render {
  
  class OffScreenRenderer
  {
    public:
      OffScreenRenderer();
      
      void init ( unsigned int, unsigned int );

      //void frameDump ( OsgTools::Render::OffScreenRenderer::FrameDumpPtr );
      void frameDump ( OsgTools::Render::OffScreenRenderer::FrameDumpPtr );

      void lookAt ( double ex, double ey, double ez, double cx, double cy, double cz, double ux, double uy, double uz );
    
      void model ( osg::Node* );

      void render();

    protected:
      virtual ~OffScreenRenderer();
  };
  
  }
}

%{

void setFrameDump( OsgTools::Render::OffScreenRenderer* viewer, OsgTools::Render::FrameDump* frameDump )
{
  if ( 0x0 != viewer && 0x0 != frameDump )
  {
    viewer->frameDump ( OsgTools::Render::OffScreenRenderer::FrameDumpPtr ( frameDump ) );
  }
}

%}

void setFrameDump( OsgTools::Render::OffScreenRenderer* viewer, OsgTools::Render::FrameDump* frameDump );
