
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Scene.Visitors
{
  public class FrustumCull : CadKit.Scene.Visitors.Cull
  {
    /// <summary>
    /// Constructor
    /// </summary>
    public FrustumCull()
      : base()
    {
    }

    /// <summary>
    /// Constructor
    /// </summary>
    public FrustumCull(CadKit.Scene.Bins.Bin bin)
      : base(bin)
    {
    }

    /// <summary>
    /// Destructor
    /// </summary>
    ~FrustumCull()
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
