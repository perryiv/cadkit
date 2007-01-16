
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

using namespace System;

#pragma warning ( disable : 4561 )
#include "Minerva/Core/GUI/Controller.h"

namespace DT
{
  namespace Minerva
  {
    namespace Plugins
    {
      namespace Document
      {
        namespace Glue 
        {

	        public ref class DistributedGlue
	        {
		        public:

              typedef System::Collections::Generic::List< System::String ^ >  Strings;

              DistributedGlue();
              ~DistributedGlue();

              /// Get/Set hostname.
              property System::String^ Hostname
              {
                System::String^ get()
                {
                  return _host;
                }
                void set( System::String^ string )
                {
                  _host = string;
                }
              }

              /// Get/Set database.
              property System::String^ Database
              {
                System::String^ get()
                {
                  return _database;
                }
                void set( System::String^ string )
                {
                  _database = string;
                }
              }

              /// Get/Set username.
              property System::String^ Username
              {
                System::String^ get()
                {
                  return _user;
                }
                void set( System::String^ string )
                {
                  _user = string;
                }
              }              

              /// Get/Set password.
              property System::String^ Password
              {
                System::String^ get()
                {
                  return _password;
                }
                void set( System::String^ string )
                {
                  _password = string;
                }
              }

              void connect( );
              void disconnect();

              int  connectToSession( System::String ^ name );
              void deleteSession();
              Strings^ getAvailableSessions();

              void removeLayer( CadKit::Interfaces::ILayer ^ layer );
              void showLayer ( CadKit::Interfaces::ILayer ^ layer );
              void modifyLayer ( CadKit::Interfaces::ILayer ^layer );

              void startAnimation(float speed, bool accumulate, bool dateTimeStep, bool timeWindow, int numDays);
              void stopAnimation();

            private:

              ::Minerva::Core::GUI::Controller * _controller;

              System::String ^ _host;
              System::String ^ _database;
              System::String ^ _user;
              System::String ^ _password;
	        };
        }
      }
    }
  }
}
