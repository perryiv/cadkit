
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Scene.Visitors
{
  public class Visitor : CadKit.Scene.Base.Object
  {
    /// <summary>
    /// Constructor
    /// </summary>
    protected Visitor()
      : base()
    {
    }

    /// <summary>
    /// Destructor
    /// </summary>
    ~Visitor()
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
    public virtual void visit(CadKit.Scene.Nodes.Node n)
    {
    }

    /// <summary>
    /// Visit this node.
    /// </summary>
    public virtual void visit(CadKit.Scene.Nodes.Groups.Group g)
    {
    }

    /// <summary>
    /// Visit this node.
    /// </summary>
    public virtual void visit(CadKit.Scene.Nodes.Groups.Transform t)
    {
    }

    /// <summary>
    /// Visit this node.
    /// </summary>
    public virtual void visit(CadKit.Scene.Nodes.Geometry.Geometry g)
    {
    }

    /// <summary>
    /// Visit this node.
    /// </summary>
    public virtual void visit(CadKit.Scene.Nodes.Geometry.Implicit g)
    {
    }

    /// <summary>
    /// Visit this node.
    /// </summary>
    public virtual void visit(CadKit.Scene.Nodes.Geometry.VertexBased g)
    {
    }
  }
}
