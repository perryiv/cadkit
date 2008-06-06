
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Scene.Nodes.Groups
{
  public class Transform : CadKit.Scene.Nodes.Groups.Group
  {
    /// <summary>
    /// Constructor
    /// </summary>
    public Transform()
      : base()
    {
    }

    /// <summary>
    /// Destructor
    /// </summary>
    ~Transform()
    {
    }

    /// <summary>
    /// Clean this instance.
    /// </summary>
    protected override void _cleanup()
    {
      lock (this.Mutex) { base._cleanup(); }
    }

    /// <summary>
    /// Accept the visitor
    /// </summary>
    public override void accept(CadKit.Scene.Visitors.Visitor visitor)
    {
      visitor.visit(this);
    }
  }
}
