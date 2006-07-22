
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Commands
{
  public class History : CadKit.Interfaces.ICommandHistory
  {
    /// <summary>
    /// Local types.
    /// </summary>
    class Stack : System.Collections.Generic.Stack<CadKit.Interfaces.ICommand> { }

    /// <summary>
    /// Data members.
    /// </summary>
    private object _mutex = new object();
    Stack _done = new Stack();
    Stack _undone = new Stack();

    /// <summary>
    /// Constructor.
    /// </summary>
    public History()
    {
    }

    /// <summary>
    /// Add the recently executed command.
    /// </summary>
    void CadKit.Interfaces.ICommandHistory.add(object command)
    {
      lock (_mutex)
      {
        CadKit.Interfaces.ICommand cmd = command as CadKit.Interfaces.ICommand;
        if (null != cmd)
        {
          _done.Push(cmd);
          _undone.Clear();
        }
      }
    }

    /// <summary>
    /// Undo the last command.
    /// </summary>
    void CadKit.Interfaces.ICommandHistory.undo()
    {
      lock (_mutex)
      {
        if (true == this.CanUndo)
        {
          // Move the command over to the "undone" stack before we execute.
          CadKit.Interfaces.ICommand command = _done.Pop();
          _undone.Push(command);

          // Undo the command. This may throw.
          command.undo();
        }
      }
    }

    /// <summary>
    /// Redo the command that was last undone.
    /// </summary>
    void CadKit.Interfaces.ICommandHistory.redo()
    {
      lock (_mutex)
      {
        if (true == this.CanRedo)
        {
          // Move the command over to the "done" stack before we execute.
          CadKit.Interfaces.ICommand command = _undone.Pop();
          _done.Push(command);

          // Execute the command. This may throw.
          command.execute();
        }
      }
    }

    /// <summary>
    /// Can the command be undone?
    /// </summary>
    bool CadKit.Interfaces.ICommandHistory.CanUndo
    {
      get { lock (_mutex) { return this.CanUndo; } }
    }

    /// <summary>
    /// Can the command be redone?
    /// </summary>
    bool CadKit.Interfaces.ICommandHistory.CanRedo
    {
      get { lock (_mutex) { return this.CanRedo; } }
    }

    /// <summary>
    /// Can the command be undone?
    /// </summary>
    public bool CanUndo
    {
      get { lock (_mutex) { return (((_done.Count > 0) && (null != _done.Peek())) && _done.Peek().canUndo()); } }
    }

    /// <summary>
    /// Can the command be redone?
    /// </summary>
    public bool CanRedo
    {
      get { lock (_mutex) { return (((_undone.Count > 0) && (null != _undone.Peek()))); } }
    }
  }
}
