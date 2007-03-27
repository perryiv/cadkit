
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface IGuiDelegateCreate : IReferenced
	{
    /// <summary>
    /// Create new gui-delegate.
    /// </summary>
    object create(object caller);
	}
}
