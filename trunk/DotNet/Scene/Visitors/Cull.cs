
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Scene.Visitors
{
  public class Cull : CadKit.Scene.Visitors.Visitor
  {
    /// <summary>
    /// Data members
    /// </summary>
    CadKit.Scene.Bins.Bin _bin = null;

    /// <summary>
    /// Constructor
    /// </summary>
    protected Cull()
      : base()
    {
    }

    /// <summary>
    /// Constructor
    /// </summary>
    protected Cull ( CadKit.Scene.Bins.Bin bin )
      : base()
    {
      this.Bin = bin;
    }

    /// <summary>
    /// Destructor
    /// </summary>
    ~Cull()
    {
      System.Diagnostics.Debug.Assert(null != _bin, "Error 1838298260: bin not null");
    }

    /// <summary>
    /// Clean this instance.
    /// </summary>
    protected override void _cleanup()
    {
      lock (this.Mutex)
      {
        this.Bin = null;
        base._cleanup();
      }
    }

    /// <summary>
    /// Set/get the bin.
    /// </summary>
    public CadKit.Scene.Bins.Bin Bin
    {
      get { lock (this.Mutex) { return _bin; } }
      set
      {
        lock (this.Mutex)
        {
          CadKit.Referenced.Base.dereference(_bin);
          _bin = value;
          CadKit.Referenced.Base.reference(_bin);
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
