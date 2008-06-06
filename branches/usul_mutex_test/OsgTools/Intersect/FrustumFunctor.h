  
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_INTERSECT_H__
#define __OSG_TOOLS_INTERSECT_H__

#include "osg/TriangleFunctor"

namespace OsgTools {
namespace Intersect {

template < class T >
class FrustumFunctor : public osg::TriangleFunctor<T>
{
public:
  FrustumFunctor() { }
  virtual ~FrustumFunctor() { }

  virtual void drawArrays(GLenum mode,GLint first,GLsizei count)
  {
    if (this->_vertexArrayPtr==0 && count==0) return;
    
    switch(mode)
    {
        case(GL_TRIANGLES):
        {
          const osg::Vec3* vlast = &this->_vertexArrayPtr[first+count];
          for( const osg::Vec3* vptr= &this->_vertexArrayPtr[first]; vptr < vlast; vptr+=3 )
            this->operator()( *(vptr),*(vptr+1),*(vptr+2),this->_treatVertexDataAsTemporary );
          break;
        }
        case(GL_TRIANGLE_STRIP):
        {
          const osg::Vec3* vptr = &this->_vertexArrayPtr[first];
          for( GLsizei i=2; i < count; ++i, ++vptr )
          {
              if ((i%2)) this->operator()( *(vptr),*(vptr+2),*(vptr+1),this->_treatVertexDataAsTemporary );
              else       this->operator()( *(vptr),*(vptr+1),*(vptr+2),this->_treatVertexDataAsTemporary );
          }
          break;
        }
        case(GL_QUADS):
        {
          const osg::Vec3* vptr = &this->_vertexArrayPtr[first];
          for(GLsizei i=3;i<count;i+=4,vptr+=4)
          {
            this->operator()(*(vptr),*(vptr+1),*(vptr+2),this->_treatVertexDataAsTemporary);
            this->operator()(*(vptr),*(vptr+2),*(vptr+3),this->_treatVertexDataAsTemporary);
          }
          break;
        }
        case(GL_QUAD_STRIP):
        {
          const osg::Vec3* vptr = &this->_vertexArrayPtr[first];
          for(GLsizei i=3;i<count;i+=2,vptr+=2)
          {
            this->operator()(*(vptr),*(vptr+1),*(vptr+2),this->_treatVertexDataAsTemporary);
            this->operator()(*(vptr+1),*(vptr+3),*(vptr+2),this->_treatVertexDataAsTemporary);
          }
          break;
        }
        case(GL_POLYGON): // treat polygons as GL_TRIANGLE_FAN
        case(GL_TRIANGLE_FAN):
        {
          const osg::Vec3* vfirst = &this->_vertexArrayPtr[first];
          const osg::Vec3* vptr = vfirst+1;
          for(GLsizei i=2;i<count;++i,++vptr)
          {
             this->operator()(*(vfirst),*(vptr),*(vptr+1),this->_treatVertexDataAsTemporary);
          }
          break;
        }
        case(GL_POINTS):
          {
            const osg::Vec3* vfirst = &this->_vertexArrayPtr[first];
            for ( GLsizei i = 0; i < count; ++i )
              this->operator() ( *(vfirst + 1 ), this->_treatVertexDataAsTemporary );
            break;
          }
        case(GL_LINES):
          {
            const osg::Vec3* vlast = &this->_vertexArrayPtr[first+count];
            for( const osg::Vec3* vptr= &this->_vertexArrayPtr[first]; vptr < vlast; vptr += 2 )
              this->operator()( *(vptr),*(vptr+1),this->_treatVertexDataAsTemporary );
            break;
          }
        case(GL_LINE_STRIP):
          {
          const osg::Vec3* vptr = &this->_vertexArrayPtr[first];
          for( GLsizei i=1; i < count; ++i, ++vptr )
          {
            this->operator()( *(vptr),*(vptr+1),this->_treatVertexDataAsTemporary );
          }
          break;
          }
        case(GL_LINE_LOOP):
        default:
            // can't be converted into to triangles.
            break;
    }
  }

  virtual void drawElements(GLenum mode,GLsizei count,const GLubyte* indices)
  {
    this->_drawElements( mode, count, indices );
  }

  virtual void drawElements(GLenum mode,GLsizei count,const GLushort* indices)
  {
    this->_drawElements( mode, count, indices );
  }

  virtual void drawElements(GLenum mode,GLsizei count,const GLuint* indices)
  {
    this->_drawElements( mode, count, indices );
  }

protected:
  template < class IndexType > void _drawElements ( GLenum mode,GLsizei count,const IndexType* indices )
  {
    if (indices==0 || count==0) return;

    typedef const IndexType* IndexPointer;

    switch(mode)
    {
        case(GL_TRIANGLES):
        {
            IndexPointer ilast = &indices[count];
            for(IndexPointer  iptr=indices;iptr<ilast;iptr+=3)
                this->operator()(this->_vertexArrayPtr[*iptr],this->_vertexArrayPtr[*(iptr+1)],this->_vertexArrayPtr[*(iptr+2)],this->_treatVertexDataAsTemporary);
            break;
        }
        case(GL_TRIANGLE_STRIP):
        {
            IndexPointer iptr = indices;
            for(GLsizei i=2;i<count;++i,++iptr)
            {
                if ((i%2)) this->operator()(this->_vertexArrayPtr[*(iptr)],this->_vertexArrayPtr[*(iptr+2)],this->_vertexArrayPtr[*(iptr+1)],this->_treatVertexDataAsTemporary);
                else       this->operator()(this->_vertexArrayPtr[*(iptr)],this->_vertexArrayPtr[*(iptr+1)],this->_vertexArrayPtr[*(iptr+2)],this->_treatVertexDataAsTemporary);
            }
            break;
        }
        case(GL_QUADS):
        {
            IndexPointer iptr = indices;
            for(GLsizei i=3;i<count;i+=4,iptr+=4)
            {
                this->operator()(this->_vertexArrayPtr[*(iptr)],this->_vertexArrayPtr[*(iptr+1)],this->_vertexArrayPtr[*(iptr+2)],this->_treatVertexDataAsTemporary);
                this->operator()(this->_vertexArrayPtr[*(iptr)],this->_vertexArrayPtr[*(iptr+2)],this->_vertexArrayPtr[*(iptr+3)],this->_treatVertexDataAsTemporary);
            }
            break;
        }
        case(GL_QUAD_STRIP):
        {
            IndexPointer iptr = indices;
            for(GLsizei i=3;i<count;i+=2,iptr+=2)
            {
                this->operator()(this->_vertexArrayPtr[*(iptr)],this->_vertexArrayPtr[*(iptr+1)],this->_vertexArrayPtr[*(iptr+2)],this->_treatVertexDataAsTemporary);
                this->operator()(this->_vertexArrayPtr[*(iptr+1)],this->_vertexArrayPtr[*(iptr+3)],this->_vertexArrayPtr[*(iptr+2)],this->_treatVertexDataAsTemporary);
            }
            break;
        }
        case(GL_POLYGON): // treat polygons as GL_TRIANGLE_FAN
        case(GL_TRIANGLE_FAN):
        {
            IndexPointer iptr = indices;
            const osg::Vec3& vfirst = this->_vertexArrayPtr[*iptr];
            ++iptr;
            for(GLsizei i=2;i<count;++i,++iptr)
            {
                this->operator()(vfirst,this->_vertexArrayPtr[*(iptr)],this->_vertexArrayPtr[*(iptr+1)],this->_treatVertexDataAsTemporary);
            }
            break;
        }
        case(GL_POINTS):
          {
            IndexPointer ilast = &indices[count];
            for(IndexPointer  iptr=indices;iptr<ilast; ++iptr)
                this->operator()(this->_vertexArrayPtr[*iptr],this->_treatVertexDataAsTemporary);
            break;
          }
        case(GL_LINES):
          {
            IndexPointer ilast = &indices[count];
            for(IndexPointer  iptr=indices; iptr < ilast; iptr += 2)
                this->operator()(this->_vertexArrayPtr[*iptr],this->_vertexArrayPtr[*(iptr+1)],this->_treatVertexDataAsTemporary);
            break;
          }
        case(GL_LINE_STRIP):
          {
            IndexPointer iptr = indices;
            for(GLsizei i=1;i<count;++i,++iptr)
            {
              this->operator()(this->_vertexArrayPtr[*(iptr)],this->_vertexArrayPtr[*(iptr+1)],this->_treatVertexDataAsTemporary);
            }
            break;
          }
        case(GL_LINE_LOOP):
        default:
            // can't be converted into to triangles.
            break;
    }

  }
  
};

}

}

#endif // __OSG_TOOLS_INTERSECT_H__

