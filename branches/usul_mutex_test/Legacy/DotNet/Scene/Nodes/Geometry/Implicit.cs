
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Scene.Nodes.Geometry
{
  public class Implicit : CadKit.Scene.Nodes.Geometry.Geometry
  {
    /// <summary>
    /// Constructor
    /// </summary>
    protected Implicit()
      : base()
    {
    }

    /// <summary>
    /// Destructor
    /// </summary>
    ~Implicit()
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
    public override void accept(CadKit.Scene.Visitors.Visitor visitor)
    {
      visitor.visit(this);
    }
  }
}
