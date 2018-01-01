/*
 * test.c, function test and an example for templated query from MySQL
 * Copyright (c) 2018, haibolei <duiyanrenda@gmail.com>
 */

#include "query.hpp"
#include <iomanip>

using std::ostream;
using std::setw;
using std::setfill;

/*
mysql> desc employees;
+------------+---------------+------+-----+---------+-------+
| Field      | Type          | Null | Key | Default | Extra |
+------------+---------------+------+-----+---------+-------+
| emp_no     | int(11)       | NO   | PRI | NULL    |       |
| birth_date | date          | NO   |     | NULL    |       |
| first_name | varchar(14)   | NO   |     | NULL    |       |
| last_name  | varchar(16)   | NO   |     | NULL    |       |
| gender     | enum('M','F') | NO   |     | NULL    |       |
| hire_date  | date          | NO   |     | NULL    |       |
+------------+---------------+------+-----+---------+-------+
*/

class EmployeesHandler;

class Employees : public MetaData
{
public:
    friend ostream& operator<< (ostream &os, const Employees &e)
    {
        os << "|" << setw (8) << e.emp_no << "|" 
            << setw (10) << e.birth_date << "|" 
            << setw (16) << e.first_name << "|"
            << setw (16) << e.last_name << "|" 
            << setw (6) << e.gender << "|" 
            << setw (16) << e.hire_date << "|" << endl;
        return os;
    }
    virtual unsigned int num_fields () const
    {
        return NUM_FIELDS;
    }
    friend class EmployeesHandler;
private:
    static const unsigned int NUM_FIELDS = 6;
    int emp_no;
    string birth_date;
    string first_name;
    string last_name;
    string gender;
    string hire_date;
};

class EmployeesHandler : public RowHandler<Employees>
{
public:
    bool handle_row (MYSQL_ROW row, unsigned long* lengths, unsigned int num_fields, Employees &e)
    {
        bool ret_val = true;
        try
        {
            if (e.num_fields () != num_fields)
            {
                throw new MysqlException ("the number of meta data class is not aligned");
            }
            register int i = 0;
            if (!get_from_string (e.emp_no, row[i], lengths[i]))
            {
                throw new MysqlException ("failed to get emp_no");
            }
            i ++;
            if (!get_from_string (e.birth_date, row[i], lengths[i]))
            {
                throw new MysqlException ("failed to get birth_date");
            }
            i ++;
            if (!get_from_string (e.first_name, row[i], lengths[i]))
            {
                throw new MysqlException ("failed to get first_name");
            }
            i ++;
            if (!get_from_string (e.last_name, row[i], lengths[i]))
            {
                throw new MysqlException ("failed to get last_name");
            }
            i ++;
            if (!get_from_string (e.gender, row[i], lengths[i]))
            {
                throw new MysqlException ("failed to get gender");
            }
            i ++;
            if (!get_from_string (e.hire_date, row[i], lengths[i]))
            {
                throw new MysqlException ("failed to get hire_date");
            }
        }
        catch (MysqlException *e)
        {
            cerr << e->what () << endl;
            delete e;
            ret_val = false;
        }
        return ret_val;
    }
};

int main (int argc, char **argv)
{
    MysqlQuery mysql(argv);
    if (mysql.connect ("test", "test", "employees"))
    {
        EmployeesHandler eh;
        vector<Employees> results;
        const string sql_str = "SELECT * FROM employees ORDER BY first_name LIMIT 10";
        if (!mysql.query (&eh, sql_str, results))
        {
            cerr << "failed to run query:" << sql_str << endl;
            return -1;
        }
        using std::cout;
        cout << "got " << results.size () << " rows:" << endl;
        cout << "|" << setw (8) << "emp_no" << "|" 
            << setw (10) << "birth_date" << "|" 
            << setw (16) << "first_name" << "|"
            << setw (16) << "last_name" << "|" 
            << setw (6) << "gender" << "|"
            << setw (16) << "hire_date" << "|" << endl;
        cout << "|" << setw (8 + 1) << setfill ('-') << "|" 
            << setw (10 + 1) << setfill ('-') << "|" 
            << setw (16 + 1) << setfill ('-') << "|"
            << setw (16 + 1) << setfill ('-') << "|" 
            << setw (6 + 1) << setfill ('-') << "|"
            << setw (16 + 1) << setfill ('-') << "|" << endl;
        cout << setfill (' ');
        for (vector<Employees>::const_iterator cit = results.begin (); cit != results.end (); ++ cit)
        {
            cout << *cit;
        }
    }
    return 0;
}
