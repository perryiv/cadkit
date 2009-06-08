
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

namespace osgGA { class MatrixManipulator; }

namespace CadKit 
{
  namespace OSG 
  { 
    namespace Glue 
    {
	    public ref class MatrixManipulator
	    {
      public:
		    MatrixManipulator();
        MatrixManipulator( System::IntPtr ptr );
        ~MatrixManipulator();
        !MatrixManipulator();

        System::IntPtr  ptr();
        void            ptr ( System::IntPtr );

      protected:
        void            _unreference();
      private:
        osgGA::MatrixManipulator *_matrixManipulator;
	    };
    }
  }
}
