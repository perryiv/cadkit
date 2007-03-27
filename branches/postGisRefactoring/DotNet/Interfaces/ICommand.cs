
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface ICommand : IReferenced
  {
    /// <summary>
    /// Can the command be undone?
    /// </summary>
    bool canUndo();

    /// <summary>
    /// Execute the command.
    /// </summary>
    void execute();

    /// <summary>
    /// Undo the command.
    /// </summary>
    void undo();
  }
}
