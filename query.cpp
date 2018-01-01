/*
 * query.cpp, an abstraction for SQL query from MySQL
 * Copyright (c) 2018, haibolei <duiyanrenda@gmail.com>
 */

#include "query.hpp"


/*
 * a full specialization version for unsigned char*
 */
template <>
bool get_from_string<unsigned char*> (unsigned char* &t, const char *s, int length)
{
    for (int i = 0; i < length; i ++)
    {
        t[i] = (unsigned char) s[i];
    }
    return true;
}

bool MysqlQuery::connect (const string &user, const string &passwd, const string &db)
{
    bool ret = true;
    try
    {
        if (!m_mysql)
        {
            throw new MysqlException ("empty pointer");
        }
        if (!mysql_real_connect (m_mysql, NULL, user.c_str (), passwd.c_str (), db.c_str (),
                    0, NULL, 0))
        {
            throw new MysqlException (mysql_error (m_mysql));
        }
    }
    catch (MysqlException *e)
    {
        cerr << e->what () << endl;
        delete e;
        ret = false;
    }
    return ret;
}

MysqlQuery::MysqlQuery (char** argv)
{
    m_mysql = new MYSQL;
    mysql_init (m_mysql);
    mysql_options (m_mysql, MYSQL_READ_DEFAULT_GROUP, argv[0]);
}
MysqlQuery::~MysqlQuery ()
{
    mysql_close (m_mysql);
    delete m_mysql;
}

