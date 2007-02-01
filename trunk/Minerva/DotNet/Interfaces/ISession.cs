
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

using Strings = System.Collections.Generic.List<string>;

namespace DT.Minerva.Interfaces
{
  public interface ISession
  {
    /// Connect to a session.
    int connectToSession(string name);

    /// Delete the session.
    void deleteSession();

    /// Get all availabe sessions.
    Strings getAvailableSessions();
  }
}
