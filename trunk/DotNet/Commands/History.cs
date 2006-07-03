
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Commands
{
  public class History
  {
    /// <summary>
    /// Local types.
    /// </summary>
    class Stack : System.Collections.Generic.Stack<CadKit.Commands.Command> { }

    /// <summary>
    /// Data members.
    /// </summary>
    private static History _instance = null;
    private object _mutex = new object();
    Stack _done = new Stack();
    Stack _undone = new Stack();

    /// <summary>
    /// Constructor.
    /// </summary>
    private History()
    {
    }

    /// <summary>
    /// Get the instance.
    /// </summary>
    public static History Instance
    {
      get
      {
        lock ("CadKit.Commands.History.instance")
        {
          if (null == _instance)
            _instance = new History();
          return _instance;
        }
      }
    }

    /// <summary>
    /// Add the recently executed command.
    /// </summary>
    public void add(CadKit.Commands.Command command)
    {
      lock (_mutex)
      {
        _done.Push(command);
        _undone.Clear();
      }
    }

    /// <summary>
    /// Undo the last command.
    /// </summary>
    public void undo()
    {
      lock (_mutex)
      {
        if (true == this.CanUndo)
        {
          // Move the command over to the "undone" stack before we execute.
          CadKit.Commands.Command command = _done.Pop();
          _undone.Push(command);

          // Execute the command. This may throw.
          command.execute();
        }
      }
    }

    /// <summary>
    /// Redo the command that was last undone.
    /// </summary>
    public void redo()
    {
      lock (_mutex)
      {
        if (true == this.CanUndo)
        {
          // Move the command over to the "done" stack before we execute.
          CadKit.Commands.Command command = _undone.Pop();
          _done.Push(command);

          // Execute the command. This may throw.
          command.execute();
        }
      }
    }

    /// <summary>
    /// Can the command be undone?
    /// </summary>
    public bool CanUndo
    {
      get
      {
        lock (_mutex)
        {
          return (((_done.Count > 0) && (null != _done.Peek())) && _done.Peek().canUndo());
        }
      }
    }

    /// <summary>
    /// Can the command be redone?
    /// </summary>
    public bool CanRedo
    {
      get
      {
        lock (_mutex)
        {
          return (((_undone.Count > 0) && (null != _undone.Peek())) && _undone.Peek().canRedo());
        }
      }
    }
  }
}
