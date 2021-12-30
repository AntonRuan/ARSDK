#include "base/lidig_logger.h"
#include "db/lidig_sqlite3.h"

lidig_sqlite3::lidig_sqlite3() {
    LogTrace();
}

lidig_sqlite3::~lidig_sqlite3() {
    LogTrace();
}

bool lidig_sqlite3::open(const std::string& dbname, int flags) {
    LogInfo() << dbname;
    if (sqlite3_open_v2(dbname.c_str(), &db_, flags, nullptr) != SQLITE_OK) {
        LogError() << dbname << " failed!";
        return false;
    }

    return true;
}

void lidig_sqlite3::close() {
    if (db_ == nullptr)
        return;

    if (sqlite3_close_v2(db_) != SQLITE_OK) {
        LogError() << "failed!";
        return;
    }

    db_ = nullptr;
}

bool lidig_sqlite3::execute(const std::string& sql) {
    int ret = sqlite3_exec(db_, sql.c_str(), 0, 0, 0);
    if (ret != SQLITE_OK)
        return false;

    return true;
}

lidig_sqlite3_stmt* lidig_sqlite3::query(const std::string& sql) {
    lidig_sqlite3_stmt* stmt = new lidig_sqlite3_stmt(this);
    return stmt->query(sql);
}

bool lidig_sqlite3::begin_transaction() {
    int ret = sqlite3_exec(db_, "begin;", 0, 0, 0);
    if (ret != SQLITE_OK)
        return false;

    return true;
}

bool lidig_sqlite3::commit_transaction() {
    int ret = sqlite3_exec(db_, "commit;", 0, 0, 0);
    if (ret != SQLITE_OK)
        return false;

    return true;
}

bool lidig_sqlite3::rollback_transaction() {
    int ret = sqlite3_exec(db_, "rollback;", 0, 0, 0);
    if (ret != SQLITE_OK)
        return false;

    return true;
}


lidig_sqlite3_stmt::lidig_sqlite3_stmt(lidig_sqlite3* sqlite3): sqlite3_(sqlite3) {
    LogTrace();
}

lidig_sqlite3_stmt::~lidig_sqlite3_stmt() {
    LogTrace();
}

bool lidig_sqlite3_stmt::next() {
    int ret = sqlite3_step(stmt_);

    return ret == SQLITE_ROW;;
}

lidig_sqlite3_stmt* lidig_sqlite3_stmt::query(const std::string& sql) {
    int ret = sqlite3_prepare_v2(sqlite3_->get_db(), sql.c_str(), -1, &stmt_, 0);
    if (ret != SQLITE_OK)
        return this;

    return this;
}