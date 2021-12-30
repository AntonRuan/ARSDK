/*
 *  Created on: 2021/3/2
 *      Author: Anton Ruan
 */

#ifndef __LIDIG_SQLITE3_H__
#define __LIDIG_SQLITE3_H__

#include <iostream>
#include <functional>
#include <sqlite3.h>

class lidig_sqlite3_stmt;
class lidig_sqlite3
{
public:
    lidig_sqlite3();
    ~lidig_sqlite3();

    enum Flags {
        READONLY = SQLITE_OPEN_READONLY,
        READWRITE = SQLITE_OPEN_READWRITE,
        CREATE = SQLITE_OPEN_CREATE
    };
    bool open(const std::string& dbname, int flags = READWRITE | CREATE);
    void close();
    bool execute(const std::string& sql);
    lidig_sqlite3_stmt* query(const std::string& sql);

    bool begin_transaction();
    bool commit_transaction();
    bool rollback_transaction();

    sqlite3* get_db() {return db_;}

private:
    sqlite3* db_;
};

class lidig_sqlite3_stmt
{
public:
    lidig_sqlite3_stmt(lidig_sqlite3* sqlite3);
    ~lidig_sqlite3_stmt();

    bool next();
    lidig_sqlite3_stmt* query(const std::string& sql);

    int get_column_int(int index) {
        return sqlite3_column_int(stmt_, index);
    }

    int get_column_bytes(int index) {
        return sqlite3_column_bytes(stmt_, index);
    }

    int64_t get_column_int64(int index) {
        return sqlite3_column_int64(stmt_, index);
    }

    const void* get_column_blob(int index) {
        return sqlite3_column_blob(stmt_, index);
    }

    const unsigned char* get_column_text(int index) {
        return sqlite3_column_text(stmt_, index);
    }

private:
    sqlite3_stmt *stmt_;
    lidig_sqlite3* sqlite3_;
};

#endif