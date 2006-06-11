
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit
{
  namespace OpenGL
  {
    public partial class Canvas : System.Windows.Forms.Panel
    {
      // Local type used below.
      public class EmptyColumns : System.Collections.Generic.LinkedList<System.Data.DataColumn> { }

      /// <summary>
      /// Call this to get the pixel formats.
      /// </summary>
      public System.Data.DataTable availablePixelFormats()
      {
        try
        {
          if ( null != _renderContext )
          {
            return CadKit.Glue.PixelFormat.query( _renderContext.deviceContext() );
          }
        }
        catch ( System.Exception e )
        {
          System.Console.WriteLine( "Error 1024033356: {0}\n{1}", e.Message, e.StackTrace );
        }
        return ( new System.Data.DataTable() );
      }
    }
  }
}
