# mysql_query

## Overview
Typically when your application want to fetch data from MySQL database, the following steps needs to be done:
* init (mysql_init)
* connect to database (mysql_real_connect)
* run query (mysql_real_connect)
* store the result (mysql_store_result)
* handle data one row after another (mysql_fetch_row, mysql_num_fields, mysql_fetch_lengths)
* clean up (mysql_close)
For applications, it needs to extract meta data from those results and move on to next step. Can we simplify this step?
## Template method
If we look into the actual source code, we can get the skeleton of this procedure:
* construct the SQL statement to be executed
* execute the SQL statement
* handle the results in MYSQL_ROW
* handle the exceptions

If we use C++ template class/function, can we life easier? This is why this project is created.
### MetaData 
All entity classes need to derive from this base class to make sure the interface is available.
```C++
  /*
   * @class MetaData has an interface to return the target field number
   */
  
  class MetaData
  {
  public:
      virtual unsigned int num_fields () const = 0;
  };
```
### RowHandler
All handlers for entity classes needs to derive from this base class to make sure the interface is defined.
```C++
/*
   * @class RowHandler has an interface to handle one row, whose type is MYSQL_ROW;
   * it is an array of strings.
   * @row, struct defined by MySQL
   * @lengths, lengths for each column
   * @num_fields,  number of fields in meta data
   * @c, instance of entity class
   */
  
  template <class T>
  class RowHandler
  {
  public:
      virtual bool handle_row (MYSQL_ROW row, unsigned long* lengths, unsigned int num_fields, T &c) = 0;
  };
```

### Template function for SQL query
```C++
    /*
     * template function to run a query sql and save results into a container
     * @H, handler class for results
     * @T, container class for results
     * @rh, pointer of handler class
     * @query_sql, SQL query you want to execute
     * @array, container for results
     * @ret_valurn, true if no error found
     */

    template <class T>
    bool query(RowHandler<T> *rh, const string& query_sql, vector<T>& array)
```
## Dependency
* mysql client lib (i.e, libmysqlclient-dev for ubuntu)
## Build
* g++ -g -o query_test query.cpp test.cpp `pkg-config --cflags --libs mysqlclient`
## Test
* create test database
There is open source test database recommended by MySQL: https://github.com/datacharmer/test_db.
* execute the query_test
./query_test 
got 10 rows:
11935	1963-03-23	           Aamer	     Jayawardene	M	1996-10-26
13011	1955-02-25	           Aamer	       Glowinski	F	1989-10-08
22279	1959-01-30	           Aamer	         Kornyak	M	1985-02-25
20678	1963-12-25	           Aamer	         Parveen	F	1987-03-25
23269	1952-02-15	           Aamer	         Szmurlo	M	1988-05-25
12160	1954-12-11	           Aamer	     Garrabrants	M	1989-09-19
24404	1960-04-21	           Aamer	         Tsukuda	M	1998-12-25
11800	1958-12-09	           Aamer	         Fraisse	M	1990-08-08
28043	1957-07-13	           Aamer	           Kroll	F	1986-05-17
15332	1961-12-29	           Aamer	           Slutz	F	1989-05-19

## To-do

   

