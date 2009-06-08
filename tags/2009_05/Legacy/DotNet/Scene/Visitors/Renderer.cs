
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Scene.Visitors
{
  public class Renderer : CadKit.Scene.Visitors.Visitor
  {
    /// <summary>
    /// Data members.
    /// </summary>
    CadKit.Scene.Visitors.Update _update = null;
    CadKit.Scene.Visitors.Cull _cull = null;
    CadKit.Scene.Visitors.Draw _draw = null;

    /// <summary>
    /// Constructor
    /// </summary>
    protected Renderer()
      : base()
    {
      CadKit.Scene.Bins.Bin bin = new CadKit.Scene.Bins.Bin();
      this.Update = new CadKit.Scene.Visitors.Update();
      this.Cull = new CadKit.Scene.Visitors.FrustumCull(bin);
      this.Draw = new CadKit.Scene.Visitors.NoDraw(bin);
    }

    /// <summary>
    /// Destructor
    /// </summary>
    ~Renderer()
    {
      System.Diagnostics.Debug.Assert(null == _update, "Error 2879979294: updete visitor not null");
      System.Diagnostics.Debug.Assert(null == _cull, "Error 2400454944: cull visitor not null");
      System.Diagnostics.Debug.Assert(null == _draw, "Error 1341321364: draw visitor not null");
    }

    /// <summary>
    /// Clean this instance.
    /// </summary>
    protected override void _cleanup()
    {
      lock (this.Mutex)
      {
        this.Update = null;
        this.Cull = null;
        this.Draw = null;
        base._cleanup();
      }
    }

    /// <summary>
    /// Set/get the update visitor.
    /// </summary>
    public CadKit.Scene.Visitors.Update Update
    {
      get { lock (this.Mutex) { return _update; } }
      set
      {
        lock (this.Mutex)
        {
          CadKit.Referenced.Base.dereference(_update);
          _update = value;
          CadKit.Referenced.Base.reference(_update);
        }
      }
    }

    /// <summary>
    /// Set/get the cull visitor.
    /// </summary>
    public CadKit.Scene.Visitors.Cull Cull
    {
      get { lock (this.Mutex) { return _cull; } }
      set
      {
        lock (this.Mutex)
        {
          CadKit.Referenced.Base.dereference(_cull);
          _cull = value;
          CadKit.Referenced.Base.reference(_cull);
        }
      }
    }

    /// <summary>
    /// Set/get the draw cull visitor.
    /// </summary>
    public CadKit.Scene.Visitors.Draw Draw
    {
      get { lock (this.Mutex) { return _draw; } }
      set
      {
        lock (this.Mutex)
        {
          CadKit.Referenced.Base.dereference(_draw);
          _draw = value;
          CadKit.Referenced.Base.reference(_draw);
        }
      }
    }

    /// <summary>
    /// Call to render the scene.
    /// </summary>
    public void render ( CadKit.Scene.Nodes.Node node )
    {
      lock (this.Mutex)
      {
        if (null != node)
        {
          node.accept(this);
        }
      }
    }

    /// <summary>
    /// Visit this node.
    /// </summary>
    public override void visit(CadKit.Scene.Nodes.Node n)
    {
    }

    /// <summary>
    /// Visit this node.
    /// </summary>
    public override void visit(CadKit.Scene.Nodes.Groups.Group g)
    {
    }
  }
}
