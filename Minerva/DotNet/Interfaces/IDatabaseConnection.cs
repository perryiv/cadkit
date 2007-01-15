
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
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
