
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins
{
  class Factory : CadKit.Interfaces.IClassFactory
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public Factory()
    {
    }

    /// <summary>
    /// Create the instance.
    /// </summary>
    object CadKit.Interfaces.IClassFactory.createInstance(System.Type type)
    {
      //if ( type.ToString() == System.Type.GetType ( CadKit.Interfaces.IPlugin ).ToString() )
      //{
        return new CadKit.LargeTriangleDocument.Document();
      //}
    }
  }
}
