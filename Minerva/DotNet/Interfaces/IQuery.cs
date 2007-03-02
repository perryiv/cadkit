
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Interfaces
{
  public interface IQuery
  {
    System.Data.IDataReader execteQueryIDataReader(string query);

    long executeCountQuery(string table, string where);
  }
}
