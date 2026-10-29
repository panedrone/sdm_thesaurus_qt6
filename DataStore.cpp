#include "DataStore.h"

#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>

CDataStore::CDataStore(const QString &connectionName) : connectionName(connectionName), transaction(false) {
}

bool CDataStore::open(const QString &file) {
    errors.clear();
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName(file);
    if (!db.open()) {
        errors = db.lastError().text();
        return false;
    }
    return true;
}

void CDataStore::close() {
    errors.clear();
    transaction = false;
    QSqlDatabase::database(connectionName, false).close();
}

bool CDataStore::begin() {
    errors.clear();
    QSqlDatabase db = QSqlDatabase::database(connectionName, false);
    if (!db.transaction()) {
        errors = db.lastError().text();
        return false;
    }
    transaction = true;
    return true;
}

bool CDataStore::commit() {
    errors.clear();
    QSqlDatabase db = QSqlDatabase::database(connectionName, false);
    if (!db.commit()) {
        errors = db.lastError().text();
        return false;
    }
    transaction = false;
    return true;
}

bool CDataStore::rollback() {
    errors.clear();
    QSqlDatabase db = QSqlDatabase::database(connectionName, false);
    transaction = false;
    if (!db.rollback()) {
        errors = db.lastError().text();
        return false;
    }
    return true;
}

// exec prepares the query, binds the parameters by position and runs it.
bool CDataStore::exec(DL_STR sql, QSqlQuery &query, int params_count, CValue *params[]) {
    errors.clear();
    if (!query.prepare(QString::fromUtf8(sql))) {
        errors = query.lastError().text();
        return false;
    }
    for (int i = 0; i < params_count; i++) {
        query.addBindValue(params[i]->toVariant());
    }
    if (!query.exec()) {
        errors = query.lastError().text();
        return false;
    }
    return true;
}

bool CDataStore::readRow(QSqlQuery &query, int ret_values_count, CValue *ret_values[], DL_STR col_names[]) {
    for (int i = 0; i < ret_values_count; i++) {
        int index = query.record().indexOf(QString::fromUtf8(col_names[i]));
        if (index < 0) {
            errors = QString("no column %1 in the result").arg(col_names[i]);
            return false;
        }
        ret_values[i]->fromVariant(query.value(index));
    }
    return true;
}

int CDataStore::insert(DL_STR sql, int params_count, CValue *params[], int AI_values_count, CValue *AI_values[]) {
    QSqlQuery query(QSqlDatabase::database(connectionName, false));
    if (!exec(sql, query, params_count, params)) {
        return -1;
    }
    int rows = query.numRowsAffected();
    if (AI_values_count < 1) {
        return rows;
    }
    QVariant id = query.lastInsertId();
    AI_values[0]->fromVariant(id);
    return rows;
}

int CDataStore::execDML(DL_STR sql, int params_count, CValue *params[]) {
    QSqlQuery query(QSqlDatabase::database(connectionName, false));
    if (!exec(sql, query, params_count, params)) {
        return -1;
    }
    return query.numRowsAffected();
}

int CDataStore::queryRow(DL_STR sql, int params_count, CValue *params[], int ret_values_count, CValue *ret_values[], DL_STR col_names[]) {
    QSqlQuery query(QSqlDatabase::database(connectionName, false));
    query.setForwardOnly(true);
    if (!exec(sql, query, params_count, params)) {
        return -1;
    }
    int rows = 0;
    while (query.next()) {
        if (!readRow(query, ret_values_count, ret_values, col_names)) {
            return -1;
        }
        rows++;
    }
    if (rows > 1) {
        errors = "The query returned more than 1 row";
    }
    return rows;
}

int CDataStore::queryRowList(DL_STR sql, int params_count, CValue *params[], int ret_values_count, CValue *ret_values[], DL_STR col_names[], CRowProcessor *rowProcessor) {
    QSqlQuery query(QSqlDatabase::database(connectionName, false));
    query.setForwardOnly(true);
    if (!exec(sql, query, params_count, params)) {
        return -1;
    }
    int rows = 0;
    while (query.next()) {
        if (!readRow(query, ret_values_count, ret_values, col_names)) {
            return -1;
        }
        rows++;
        rowProcessor->processRow();
    }
    return rows;
}
