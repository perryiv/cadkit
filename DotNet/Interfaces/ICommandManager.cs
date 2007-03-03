
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface ICommandManager : IReferenced
  {
    /// <summary>
    /// Execute the command.
    /// </summary>
    void execute(object command);

    /// <summary>
    /// Undo the command.
    /// </summary>
    void undo(object command);
  }
}
