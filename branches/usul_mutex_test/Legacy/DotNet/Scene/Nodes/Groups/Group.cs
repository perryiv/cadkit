
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Scene.Nodes.Groups
{
  public class Group : CadKit.Scene.Nodes.Node
  {
    /// <summary>
    /// Local types.
    /// </summary>
    public class Nodes : System.Collections.Generic.List<CadKit.Scene.Nodes.Node> { }

    /// <summary>
    /// Data members.
    /// </summary>
    private Nodes _nodes = new Nodes();

    /// <summary>
    /// Constructor
    /// </summary>
    public Group()
      : base()
    {
    }

    /// <summary>
    /// Destructor
    /// </summary>
    ~Group()
    {
      System.Diagnostics.Debug.Assert(null != _nodes, "Error 9450775080: null children" );
      System.Diagnostics.Debug.Assert(0 != _nodes.Count, System.String.Format("Error 9135138260: deleting group with {0} children" + _nodes.Count));
    }

    /// <summary>
    /// Clean this instance.
    /// </summary>
    protected override void _cleanup()
    {
      lock (this.Mutex)
      {
        this.removeAll();
        base._cleanup();
      }
    }

    /// <summary>
    /// Accept the visitor
    /// </summary>
    public override void accept(CadKit.Scene.Visitors.Visitor visitor)
    {
      visitor.visit(this);
    }

    /// <summary>
    /// Append the node.
    /// </summary>
    public void append(CadKit.Scene.Nodes.Node node)
    {
      lock (this.Mutex)
      {
        if (null != node)
        {
          _nodes.Add(node);
          node.reference();
        }
      }
    }

    /// <summary>
    /// Remove the node. Returns true if the node was found and removed.
    /// </summary>
    public bool remove(CadKit.Scene.Nodes.Node node)
    {
      lock (this.Mutex)
      {
        return this.remove(_nodes.IndexOf(node));
      }
    }

    /// <summary>
    /// Remove the node specified by the index. Returns true if the node was found and removed.
    /// </summary>
    public bool remove(int child)
    {
      lock (this.Mutex)
      {
        if ( child > 0 && child < _nodes.Count )
        {
          CadKit.Scene.Nodes.Node node = _nodes[child];
          _nodes.RemoveAt(child);
          CadKit.Referenced.Base.dereference(node);
          return true;
        }
        return false;
      }
    }

    /// <summary>
    /// Remove All the child nodes.
    /// </summary>
    public void removeAll()
    {
      lock (this.Mutex)
      {
        foreach (CadKit.Scene.Nodes.Node node in _nodes)
        {
          CadKit.Referenced.Base.dereference(node);
        }
        _nodes.Clear();
      }
    }

    /// <summary>
    /// Get the nodes.
    /// </summary>
    public Nodes NodeList
    {
      get { lock (this.Mutex) { return _nodes; } }
    }

    /// <summary>
    /// Get a copy of the nodes as n array.
    /// </summary>
    public CadKit.Scene.Nodes.Node[] NodeArray
    {
      get { lock (this.Mutex) { return _nodes.ToArray(); } }
    }
  }
}
