
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Interfaces
{
  public interface IDatabaseConnection
  {
    string Hostname { get; set; }
    string Database { get; set; }
    string Username { get; set; }
    string Password { get; set; }

    void connect();
    void disconnect();
  }
}
