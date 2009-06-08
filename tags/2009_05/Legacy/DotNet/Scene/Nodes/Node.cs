
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Scene.Nodes
{
  public class Node : CadKit.Scene.Base.Object
  {
    /// <summary>
    /// Data members.
    /// </summary>
    private object _data = null;
    private string _name = null;

    /// <summary>
    /// Delegates
    /// </summary>
    public delegate void VoidReturnWithNoArguments();

    /// <summary>
    /// Constructor
    /// </summary>
    protected Node()
      : base()
    {
    }

    /// <summary>
    /// Destructor
    /// </summary>
    ~Node()
    {
    }

    /// <summary>
    /// Clean this instance.
    /// </summary>
    protected override void _cleanup()
    {
      lock (this.Mutex)
      {
        base._cleanup();
      }
    }

    /// <summary>
    /// Accept the visitor
    /// </summary>
    public virtual void accept(CadKit.Scene.Visitors.Visitor visitor)
    {
      visitor.visit(this);
    }

    /// <summary>
    /// Set/get the user data.
    /// </summary>
    public object UserData
    {
      get { lock (this.Mutex) { return _data; } }
      set { lock (this.Mutex) { _data = value; } }
    }

    /// <summary>
    /// Get/set the name.
    /// </summary>
    public string Name
    {
      // Return a copy.
      get { lock (this.Mutex) { return ((null == _name) ? null : System.String.Copy(_name)); } }
      set { lock (this.Mutex) { _name = value; } }
    }
  }
}
