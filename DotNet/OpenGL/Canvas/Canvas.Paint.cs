
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
      /// <summary>
      /// Call this to paint using OpenGL.
      /// </summary>
      public void paintOpenGL()
      {
        try
        {
          if ( null != _renderContext )
          {
            this._paintOpenGL();
          }
        }
        catch ( System.Exception e )
        {
          System.Console.WriteLine( "Error 9052564260: {0}\n{1}", e.Message, e.StackTrace );
        }
      }

      /// <summary>
      /// Call this to paint using OpenGL.
      /// </summary>
      private void _paintOpenGL()
      {
      }
    }
  }
}
