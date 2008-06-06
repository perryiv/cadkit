#!/usr/bin/python

###############################################################################
##
##  Copyright (c) 2007, Arizona State University
##  All rights reserved.
##  BSD License: http://www.opensource.org/licenses/bsd-license.html
##  Created by: Adam Kubach
##
###############################################################################

import csv, string, sys, os, re

def formatColumnHeader( s, deltable, delchars ):

    # Remove any white space on leading and trailing ends.
    col = string.strip( s )

    # Replace any '/' with '_'
    col = col.replace ( '/', '_' )

    # Remove any illegal characters.
    col = string.translate(col,deltable,delchars)

    # Change to lower case.
    col = col.lower()

    return col

def determineSqlType ( s ):

    # Does it look like a date?
    if re.match('(\d\d\d\d)-(\d\d)-(\d\d)', s):
        return 'date'

    if re.match('(\d\d)/(\d\d)/(\d\d\d\d)', s):
        return 'date'

    # It would be nice to figure out a better way to determine the type.
    # This isn't going to scale well...
    # Try integers.
    try:
        int(s)
        return 'int4'
    except ValueError:
            # Try floats.
            try:
                float(s)
                return 'float8'
            except ValueError:
                return 'text'

    return 'text'

def createSqlInsert( row, tablename ):
    sql = 'INSERT INTO %s VALUES ('% tablename

    length = len( row )
    last = length - 1

    for i in range( length ):
        value = string.strip(str( row[i] ))
        if value == 'NULL':
            sql = sql + 'NULL'
        elif len(value) > 0:
            sql = sql + '\'' + string.strip(str( row[i] )) + '\''
        else:
            sql = sql + 'NULL'
        if i != last:
            sql += ', '

    sql = sql + ');\n'

    return sql


def main():

    # Get the file name from the arguments.  If it's not there, prompt the user.
    if len(sys.argv) > 1:
        datafile = sys.argv[1]
    else:
        print "Enter a filename for input: ",
        datafile = string.strip(sys.stdin.readline())

    # Get the table name to create.
    tablename = os.path.basename(datafile).split('.')[0]

    #Get the directory the data file lives in.
    dir = os.path.dirname(datafile)

    # if we don't have a directory, make it the current working directory.
    if len(dir) == 0:
        dir = os.getcwd()

    # Create the name of the output file.
    outfile = dir + '/' + tablename + '.sql'

    # Create string translation tables
    allowed = ' _01234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ'
    delchars = ''
    for i in range(255):
        if chr(i) not in allowed: delchars = delchars + chr(i)
    deltable = string.maketrans(' ','_')

    # Open the reader.
    reader = csv.reader( open( datafile, "rb" ),dialect='excel')

    # Get the first line.
    header = reader.next()

    # Get the number of columns.
    num_cols = len(header)

    # Empty list for the column [names], [width].
    cols = []

    # Loop through the header names.
    for col in header:

        # Format the column header for sql.
        formated = formatColumnHeader( col, deltable, delchars )

        # check for duplicate column names.
        d = 0
        dupcol = formated
        while dupcol in cols:
            d = d + 1
            dupcol = fmtcol + '_' + str(d)

        # Add to the list.
        cols.append([dupcol,1])

    row = reader.next();

    col_types = []

    # Loop through the row and determine column types.
    for col in row:
        type = determineSqlType ( col )
        print type
        col_types.append( type )

    # I think this will re-initialize the reader and skip the header
    reader = csv.reader( open( datafile, "rb" ),dialect='excel')
    reader.next()

    inserts = ''

    # Determine max width of each column in each row
    rc = 0
    for row in reader:
        rc = rc + 1

        # Check to make sure the number of columns are consistent.
        if len(row) == num_cols:
            inserts = inserts + createSqlInsert( row, tablename )
            for i in range(len(row)):
                fld = string.strip(row[i])
                if len(fld) > cols[i][1]:
                    cols[i][1] = len(fld)
        else: print 'Warning: Line %s ignored. Different width than header' % (rc)

    print

    # Create the create table statement.
    sql = 'CREATE TABLE %s\n(' % (tablename)
    i = 0
    for col in cols:
        sql = sql + ('\n\t%s %s NULL,' % (col[0],col_types[i]))
        i = i + 1
    sql = sql[:len(sql)-1] + '\n);'

    sqlfile = open(outfile,'w')
    sqlfile.write(sql)
    sqlfile.write('\n')
    sqlfile.write ( inserts )
    sqlfile.close

    print '%s created.' % (outfile)


if __name__ == "__main__":
    main()
