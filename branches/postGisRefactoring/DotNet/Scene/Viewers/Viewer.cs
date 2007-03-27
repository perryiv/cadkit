
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Scene.Viewer
{
  public class Viewer : CadKit.Scene.Visitors.Visitor
  {
    /// <summary>
    /// Local types.
    /// </summary>
    public class Renderers : System.Collections.Generic.Stack<CadKit.Scene.Visitors.Renderer>{}

    /// <summary>
    /// Data members
    /// </summary>
    Renderers _renderers = new Renderers();

    /// <summary>
    /// Constructor
    /// </summary>
    protected Viewer()
      : base()
    {
    }

    /// <summary>
    /// Constructor
    /// </summary>
    protected Viewer(CadKit.Scene.Visitors.Renderer renderer)
      : base()
    {
      this.push(renderer);
    }

    /// <summary>
    /// Destructor
    /// </summary>
    ~Viewer()
    {
      System.Diagnostics.Debug.Assert(null != _renderers, "Error 5139068980: stack of renderers is null");
      System.Diagnostics.Debug.Assert(0 != _renderers.Count, "Error 2701738567: stack of renderers is not empty");
    }

    /// <summary>
    /// Clean this instance.
    /// </summary>
    protected override void _cleanup()
    {
      lock (this.Mutex)
      {
        this.clear();
        base._cleanup();
      }
    }

    /// <summary>
    /// Get the current renderer.
    /// </summary>
    public CadKit.Scene.Visitors.Renderer Renderer
    {
      get
      {
        lock (this.Mutex)
        {
          return ( _renderers.Count > 0 ) ? _renderers.Peek() : null;
        }
      }
    }

    /// <summary>
    /// Push the renderer.
    /// </summary>
    public void push(CadKit.Scene.Visitors.Renderer r)
    {
      lock (this.Mutex)
      {
        _renderers.Push(r);
        CadKit.Referenced.Base.reference(r);
      }
    }

    /// <summary>
    /// Pop the renderer.
    /// </summary>
    public void pop()
    {
      lock (this.Mutex)
      {
        if (_renderers.Count > 0)
        {
          CadKit.Scene.Visitors.Renderer r = _renderers.Pop();
          CadKit.Referenced.Base.dereference(r);
        }
      }
    }

    /// <summary>
    /// Clear this instance.
    /// </summary>
    public void clear()
    {
      lock (this.Mutex)
      {
        foreach (CadKit.Scene.Visitors.Renderer r in _renderers)
        {
          CadKit.Referenced.Base.dereference(r);
        }
        _renderers.Clear();
      }
    }
  }
}
