
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface ICommandHistory : IReferenced
  {
    /// <summary>
    /// Add the recently executed command.
    /// </summary>
    void add(object command);

    /// <summary>
    /// Undo the last command.
    /// </summary>
    void undo();

    /// <summary>
    /// Redo the command that was last undone.
    /// </summary>
    void redo();

    /// <summary>
    /// Can the command be undone?
    /// </summary>
    bool CanUndo { get; }

    /// <summary>
    /// Can the command be redone?
    /// </summary>
    bool CanRedo { get; }

    /// <summary>
    /// Remove all the commands.
    /// </summary>
    void clear();
  }
}
