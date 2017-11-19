#ifndef _QUERY_HPP_
#define _QUERY_HPP_

#include <iostream>
using std::cerr;
using std::endl;

#include <string>
using std::string;

#include <exception>
using std::exception;

#include <sstream>
using std::istringstream;

#include <vector>
using std::vector;

#include <mysql.h>

/*
 * exception for query
 */
class MysqlException : public exception
{
public:
    explicit MysqlException (const char *msg) : m_str (msg)
    {
    }
    explicit MysqlException (const string& msg) : m_str (msg)
    {
    }
    virtual ~MysqlException () throw ()
    {
    }
    virtual const char* what() const throw ()
    {
        return m_str.c_str ();
    }
private:
    MysqlException ();
    MysqlException (const MysqlException &e);
    string m_str;
};

/*
 * tools to get msyql results; 
 * in mysql.h, typedef char** MYSQL_ROW
 * Using the string stream to convert those string into the specific type
 * @t, meta data instance
 * @s, string for one column
 * @length, length of string, useful for unsiged char; by default, it is terminted by a '\0'
 */

template <class T>
bool get_from_string (T &t, const char *s, int length)
{
    //cerr << __FUNCTION__ << ":"  << s << endl;
    istringstream iss (s);
    iss >> t;
    return !iss.fail ();
}

/*
 * a full specialization version for unsigned char*
 */
template <>
bool get_from_string<unsigned char*> (unsigned char* &t, const char *s, int length);

/*
 * @class MetaData has an interface to return the target field number
 */

class MetaData
{
public:
    virtual unsigned int num_fields () const = 0;
};

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

class MysqlQuery
{
public:
    MysqlQuery (char **argv);
    ~MysqlQuery ();
    /*
     * Wrapper class to cover both object reference and pointer
     * It will help to simplify the code in the template function query
     */
    template <class T>
    class Holder
    {
    public:
        T get_inst ()
        {
            return t;
        }
    private:
        T t;
    };

    /*
     * a partial specialization version for pointer types
     */
    template <class T>
    class Holder<T*>
    {
    public:
        T* get_inst ()
        {
            return new T ();
        }
    };

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
    {
        bool ret_val = true;
        try
        {
            if (!m_mysql)
            {
                throw new MysqlException ("mysql client connection is not initialized.");
            }
            if (mysql_real_query (m_mysql, query_sql.c_str (), query_sql.length ()))
            {
                throw new MysqlException (query_sql + " failed; error: " + mysql_error (m_mysql));
            }
            m_res = mysql_store_result (m_mysql);
            if (!m_res)
            {
                if (mysql_field_count (m_mysql) == 0)
                {
                    cerr << "empty result, row affected: " << mysql_affected_rows (m_mysql) << endl;
                }
                else
                {
                    throw new MysqlException ("failed to get the result.");
                }
            }
            else
            {
                int num_rows = mysql_num_rows (m_res);
                if (num_rows == 0)
                {
                    cerr << "got result set, row affected: " << num_rows << endl;
                }
                else
                {
                    unsigned int num_fields = mysql_num_fields (m_res);
                    for (int i = 0; i < num_rows; i ++)
                    {
                        MYSQL_ROW row = mysql_fetch_row (m_res);
                        unsigned long *lengths = mysql_fetch_lengths (m_res);
                        if (row && lengths)
                        {
                            Holder<T> h;
                            T t = h.get_inst ();
                            if (!rh->handle_row (row, lengths, num_fields, t))
                            {
                                throw new MysqlException ("failed to handle one row.");
                            }
                            else
                            {
                                array.push_back (t);
                            }
                        }
                    }
                }
            }
        }
        catch (MysqlException *e)
        {
            cerr << e->what () << endl;
            delete e;
            if (m_res)
            {
                mysql_free_result (m_res);
            }
            ret_val = false;
        }
        return ret_val;
    }
    bool connect (const string &user, const string &passwd, const string &db);
private:
    MYSQL *m_mysql;
    MYSQL_RES *m_res;
};


#endif

