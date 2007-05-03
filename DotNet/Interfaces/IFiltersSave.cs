
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public interface IFiltersSave : IReferenced
  {
    CadKit.Interfaces.Filters Filters { get; }
  }
}
