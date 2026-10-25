#ifndef SDM_DATA_STORE_H
#define SDM_DATA_STORE_H

/*
    The data store for C++, Qt 5/6 and SQLite (QtSql, the driver QSQLITE), used by the generated DAO.
    "sdmc init -lang cpp -db sqlite" writes it next to the DAO, "sdmc ds" updates it to the sdmc version.

    The generated DAO use only the adapter: CValue and the value classes, CRowProcessor, CListReader,
    DL_STR and DL_T, the methods of CDataStore under "the adapter of the generated DAO". Keep their
    names and parameters; everything else is yours: the connection, transactions, logging, more value
    classes for the type map of settings.xml.

        CDataStore ds;
        if (!ds.open("app.sqlite3")) {
            qDebug() << ds.getLastErrors();
        }
        TasksDao dao(ds);

    Made from DataStore_Qt5.h of SQL DAL Maker: https://sqldalmaker.sourceforge.net/
 */

#include <QtSql/QSqlDatabase>
#include <QByteArray>
#include <QString>
#include <QVariant>

#include <list>

/*
    CValue is a nullable value of a column or a parameter. QtSql passes values as QVariant, and each
    value class converts its Value to and from it: toVariant() for a parameter, fromVariant() for a
    fetched column (IsNull is set by the data store). As is, CValue holds anything in Variant: it is
    the type of the fields the type map has no class for (<type-map default="CValue">). Add your own
    class the same way: a Value and the two conversions.
 */
class CValue {
public:

    bool IsNull;

    QVariant Variant; // CValue itself only

    CValue() : IsNull(true) {
    }

    virtual ~CValue() {
    }

    virtual QVariant toVariant() const {
        return Variant;
    }

    virtual void fromVariant(const QVariant &v) {
        Variant = v;
    }

protected:

    explicit CValue(bool isNull) : IsNull(isNull) {
    }
};

class CInteger : public CValue {
public:

    CInteger() : CValue(false), Value(0) {
    }

    qint64 Value;

    QVariant toVariant() const override {
        return Value;
    }

    void fromVariant(const QVariant &v) override {
        Value = v.toLongLong();
    }
};

class CDouble : public CValue {
public:

    CDouble() : CValue(false), Value(0) {
    }

    double Value;

    QVariant toVariant() const override {
        return Value;
    }

    void fromVariant(const QVariant &v) override {
        Value = v.toDouble();
    }
};

class CText : public CValue {
public:

    CText() : CValue(false) {
    }

    QString Value;

    QVariant toVariant() const override {
        return Value;
    }

    void fromVariant(const QVariant &v) override {
        Value = v.toString();
    }
};

class CBlob : public CValue {
public:

    CBlob() : CValue(false) {
    }

    QByteArray Value;

    QVariant toVariant() const override {
        return Value;
    }

    void fromVariant(const QVariant &v) override {
        Value = v.toByteArray();
    }
};

class CRowProcessor {
public:

    virtual ~CRowProcessor() {
    }

    virtual void processRow() = 0;
};

/*
    A DAO method for a list calls read() for each fetched row:

        struct : CListReader<Task> {
            void read(Task &t) { qDebug() << t.t_subject.Value; }
        } reader;
        dao.read_task_list(reader);
 */
template <class T>
class CListReader {
public:

    virtual ~CListReader() {
    }

    virtual void read(T &t) = 0;
};

/*
    CListReader2 collects the rows:

        CListReader2<Task> reader;
        dao.read_task_list(reader);
        qDebug() << reader.list.size();
 */
template <class T>
class CListReader2 : public CListReader<T> {
public:

    std::list<T> list;

    void read(T &t) {
        list.push_back(t);
    }
};

#define DL_STR const char *
#define DL_T(quote) quote

class QSqlQuery;

class CDataStore {
public:

    // connectionName: the QSqlDatabase connection of this data store
    explicit CDataStore(const QString &connectionName = QLatin1String(QSqlDatabase::defaultConnection));

    // the errors of the last call, empty if it succeeded
    const QString &getLastErrors() const {
        return errors;
    }

    // the SQLite file; yours, as everything but the adapter
    bool open(const QString &file);
    void close();

    bool begin();
    bool commit();
    bool rollback();
    bool inTransaction() const {
        return transaction;
    }

    // the adapter of the generated DAO -------------------------------------------------------------

    // AI_values: the generated values, at most one; without them the DAO call execDML instead.
    // Returns the number of inserted rows or -1 on error.
    int insert(DL_STR sql, int params_count, CValue *params[], int AI_values_count, CValue *AI_values[]);

    // Returns the number of affected rows or -1 on error.
    int execDML(DL_STR sql, int params_count, CValue *params[]);

    // No params: params_count == 0 and params == 0. Returns the number of fetched rows or -1 on error;
    // more than 1 row is an error too (getLastErrors), ret_values have the last one.
    int queryRow(DL_STR sql, int params_count, CValue *params[], int ret_values_count, CValue *ret_values[], DL_STR col_names[]);

    // No params: params_count == 0 and params == 0. Returns the number of fetched rows or -1 on error.
    int queryRowList(DL_STR sql, int params_count, CValue *params[], int ret_values_count, CValue *ret_values[], DL_STR col_names[], CRowProcessor *rowProcessor);

private:

    QString connectionName;
    QString errors;
    bool transaction;

    bool exec(DL_STR sql, QSqlQuery &query, int params_count, CValue *params[]);
    bool readRow(QSqlQuery &query, int ret_values_count, CValue *ret_values[], DL_STR col_names[]);
};

#endif // SDM_DATA_STORE_H
