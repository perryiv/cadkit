
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Scene.Bins
{
  public class Bin : CadKit.Scene.Base.Object
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
    public Bin()
      : base()
    {
    }

    /// <summary>
    /// Destructor
    /// </summary>
    ~Bin()
    {
      System.Diagnostics.Debug.Assert(null != _nodes, "Error 1010441224: null children");
      System.Diagnostics.Debug.Assert(0 != _nodes.Count, System.String.Format("Error 5480098260: deleting bin with {0} children" + _nodes.Count));
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
    /// Clear the bin.
    /// </summary>
    public void clear()
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
