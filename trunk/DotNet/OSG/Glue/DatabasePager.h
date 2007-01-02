
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

namespace osgDB { class DatabasePager; }

namespace CadKit 
{
  namespace OSG 
  { 
    namespace Glue 
    {
	    public ref class DatabasePager
	    {
      public:
		    DatabasePager();
        DatabasePager( System::IntPtr ptr );
        ~DatabasePager();
        !DatabasePager();

        System::IntPtr  ptr();
        void            ptr ( System::IntPtr );

      protected:
        void            _unreference();
      private:
        osgDB::DatabasePager *_databasePager;
	    };
    }
  }
}
