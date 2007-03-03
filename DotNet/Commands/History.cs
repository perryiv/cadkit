
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Commands
{
  public class History : 
    CadKit.Referenced.Base,
    CadKit.Interfaces.ICommandHistory
  {
    /// <summary>
    /// Local types.
    /// </summary>
    class Stack : System.Collections.Generic.Stack<CadKit.Interfaces.ICommand> { }


    /// <summary>
    /// Data members.
    /// </summary>
    private CadKit.Threads.Tools.Lock _lock = null;
    private Stack _done = new Stack();
    private Stack _undone = new Stack();


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
      CadKit.Interfaces.ICommand cmd = command as CadKit.Interfaces.ICommand;
      if (null != cmd)
      {
        using (this.Lock.write())
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
      if (true == this.CanUndo)
      {
        // Move the command over to the "undone" stack before we execute.
        CadKit.Interfaces.ICommand command = null;
        using (this.Lock.read())
        {
          command = _done.Pop();
        }
        if (null != command)
        {
          using (this.Lock.write())
          {
            _undone.Push(command);
          }

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
      if (true == this.CanRedo)
      {
        // Move the command over to the "done" stack before we execute.
        CadKit.Interfaces.ICommand command = null;
        using (this.Lock.read())
        {
          command = _undone.Pop();
        }
        if (null != command)
        {
          using (this.Lock.write())
          {
            _done.Push(command);
          }

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
      get { return this.CanUndo; }
    }


    /// <summary>
    /// Can the command be redone?
    /// </summary>
    bool CadKit.Interfaces.ICommandHistory.CanRedo
    {
      get { return this.CanRedo; }
    }


    /// <summary>
    /// Can the command be undone?
    /// </summary>
    public bool CanUndo
    {
      get { using (this.Lock.read()) { return (((_done.Count > 0) && (null != _done.Peek())) && _done.Peek().canUndo()); } }
    }


    /// <summary>
    /// Can the command be redone?
    /// </summary>
    public bool CanRedo
    {
      get { using (this.Lock.read()) { return (((_undone.Count > 0) && (null != _undone.Peek()))); } }
    }


    /// <summary>
    /// Remove all the commands.
    /// </summary>
    void CadKit.Interfaces.ICommandHistory.clear()
    {
      this.clear();
    }


    /// <summary>
    /// Remove all the commands.
    /// </summary>
    public void clear()
    {
      using (this.Lock.write())
      {
        _done.Clear();
        _undone.Clear();
      }
    }


    /// <summary>
    /// Return the lock.
    /// </summary>
    public CadKit.Threads.Tools.Lock Lock
    {
      get
      {
        // If this gets called from the finalizer then the lock may have 
        // already been destroyed and set to null.
        if (null == _lock)
        {
          _lock = new CadKit.Threads.Tools.Lock();
        }
        return _lock;
      }
    }
  }
}
