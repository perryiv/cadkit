
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Win32
{
  public static class LastError
  {
    /// <summary>
    /// Constants.
    /// </summary>
    public const int FORMAT_MESSAGE_FROM_SYSTEM = 0x00001000; // See winbase.h
    public const string SYSTEM_KERNEL_DLL = "kernel32.dll";

    /// <summary>
    /// Format the last Win32 error.
    /// </summary>
    [System.Runtime.InteropServices.DllImport(Win32.LastError.SYSTEM_KERNEL_DLL)]
    private static extern int FormatMessage(int flags, System.IntPtr source, int messageId, int languageId, System.Text.StringBuilder buffer, int size, System.IntPtr arguments);

    /// <summary>
    /// Get the last Win32 error.
    /// </summary>
    [
    System.ComponentModel.Category("Win32"),
    System.ComponentModel.Description("Windows System Error"),
    System.ComponentModel.Browsable(false)
    ]
    public static string Message
    {
      get
      {
        System.Text.StringBuilder message = new System.Text.StringBuilder(1024);
        int error = System.Runtime.InteropServices.Marshal.GetLastWin32Error();
        int result = Win32.LastError.FormatMessage(Win32.LastError.FORMAT_MESSAGE_FROM_SYSTEM, System.IntPtr.Zero, error, 0, message, message.Capacity, System.IntPtr.Zero);
        if (0 == result)
        {
          return "";
        }
        else
        {
          return message.ToString().TrimEnd(new char[] { '\n', '\r' });
        }
      }
    }
  }
}
