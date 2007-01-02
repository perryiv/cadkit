
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

using namespace System;

#include "OsgTools/Render/Renderer.h"

class ossimPlanetDatabasePager;

namespace osgGA { class MatrixManipulator; }

namespace CadKit
{
  namespace Plugins
  {
    namespace Documents
    {
      namespace OSSIMPlanetDocument
      {
        namespace Glue
        {

          public ref class Panel : public CadKit::OpenGL::Canvas
          {
          public:
            typedef CadKit::OpenGL::Canvas BaseClass;

            Panel();
            ~Panel();
            !Panel();

            void init();
            void clear();

            System::Drawing::Color^ backgroundColor();
            void backgroundColor ( System::Drawing::Color color );

            CadKit::OSG::Glue::Node^ scene();
            void scene( CadKit::OSG::Glue::Node^ node );

          protected:
            void _init();
            void _clear();

            virtual void _paintOpenGL() override;
            virtual void _resizeOpenGL() override;

            virtual void OnMouseMove(System::Windows::Forms::MouseEventArgs ^e) override;
            virtual void OnMouseWheel(System::Windows::Forms::MouseEventArgs ^e) override;
            virtual void OnMouseUp(System::Windows::Forms::MouseEventArgs ^e) override;
            virtual void OnMouseDown(System::Windows::Forms::MouseEventArgs ^e) override;

          private:
            OsgTools::Render::Renderer *_renderer;
            ossimPlanetDatabasePager *  _databasePager;
            osgGA::MatrixManipulator *  _matrixManipulator;
          };
        }
      }
    }
  }
}
