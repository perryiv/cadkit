
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Scene.Base
{
  public class Object : CadKit.Referenced.Base
  {
    /// <summary>
    /// Constructor
    /// </summary>
    protected Object()
      : base()
    {
    }

    /// <summary>
    /// Destructor
    /// </summary>
    ~Object()
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
  }
}
