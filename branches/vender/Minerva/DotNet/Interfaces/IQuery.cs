
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
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
