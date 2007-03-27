
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

namespace Minerva { namespace Core { namespace DB { class Connection; } } }

namespace DT {
  namespace Minerva {
    namespace Glue {

      public ref class Connection : public DT::Minerva::Interfaces::IDatabaseConnection
      {
      public:
        Connection();
        ~Connection();
        !Connection();


        /// Get/Set the hostname.
        property System::String^ Hostname
        {
          virtual System::String^ get();
          virtual void set ( System::String^ );
        }


        /// Get/ Set the database
        property System::String^ Database
        {
          virtual System::String^ get();
          virtual void set ( System::String^ );
        }


        /// Get/Set the username.
        property System::String^ Username
        {
          virtual System::String^ get();
          virtual void set ( System::String^ );
        }


        /// Get/Set the password.
        property System::String^ Password
        {
          virtual System::String^ get();
          virtual void set ( System::String^ );
        }

        virtual void connect();
        virtual void disconnect();

        ::Minerva::Core::DB::Connection*  nativeConnection();

      private:
        ::Minerva::Core::DB::Connection *_connection;
      };
    }
  }
}
