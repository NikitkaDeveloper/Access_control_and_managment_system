#include "connection.h"

/**
 * @brief getIDByAuthData
 * Получение id пользователя из базы данных
 * по введённому логину и паролю
 * @param login - логин
 * @param pass  - пароль
 * @return id   - id пользователя
 */
QString Connection::getIDByAuthData(const QString& login, const QString& pass) {
    QString id = "";
    
    {
        if (connectToDBWithManager("getID")) {
            QSqlDatabase check = QSqlDatabase::database("getID");
            QSqlQuery query(check);
            query.prepare(QString("SELECT id, login, pgp_sym_decrypt(pass::bytea, 'HsdFdDsgkdKHo') FROM users "
                                  "WHERE login = :login AND pgp_sym_decrypt(pass::bytea, 'HsdFdDsgkdKHo') = :pass;"));
            query.bindValue(":login", login);
            query.bindValue(":pass", pass);
            
            if (query.exec()) {
                query.first();
                id = query.value("id").toString();
            }
            query.clear();
            check.close();
        }
    }
    QSqlDatabase::removeDatabase("getID");
    
    return id;
}

bool Connection::connectToDBWithUser(const QString& connectionName) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", connectionName);
    db.setHostName("192.168.0.104");
    db.setDatabaseName("ACMS");
    db.setUserName("db_user");
    db.setPassword("user");
    if (!db.open()) {
        QMessageBox::critical(nullptr, QObject::tr("Ошибка аутентификации"), QObject::tr("Не удалось подключиться к базе данных!"));
        return false;
    }
    return true;
}

QSqlQuery Connection::getInfoAboutUser(const QString& id) {
    QSqlDatabase db = QSqlDatabase::database("user");
    
    QSqlQuery query(db);
    // Получение всей рабочей информации о сотруднике из базы данных
    query.prepare(QString("SELECT e.*, d.department, j.job FROM employees e "
                       "INNER JOIN departments d "
                       "ON e.id_department = d.id "
                       "INNER JOIN jobs j "
                       "ON e.id_job = j.id "
                       "WHERE e.id = :id"));
    query.bindValue(":id", id);
    query.exec();
    
    return query;
}

QSqlQuery Connection::getInfoAboutChief(const QString& id_department) {
    QSqlDatabase db = QSqlDatabase::database("user");
    
    QSqlQuery query(db);
    query.prepare(QString("SELECT e.*, d.department, j.job FROM employees e "
                       "INNER JOIN departments d "
                       "ON e.id_department = d.id "
                       "INNER JOIN jobs j "
                       "ON e.id_job = j.id "
                       "WHERE e.id_department = :id_department"));
    query.bindValue(":id_department", id_department);
    query.exec();
    
    return query;
}

bool Connection::connectToDBWithManager(const QString& connectionName) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", connectionName);
    db.setHostName("192.168.0.104");
    db.setDatabaseName("ACMS");
    db.setUserName("manager");
    db.setPassword("m1n2g3");
    if (!db.open()) {
        QMessageBox::critical(nullptr, QObject::tr("Ошибка аутентификации"), QObject::tr("Не удалось подключиться к базе данных!"));
        return false;
    }
    return true;
}

/**
 * @brief getRoleByID
 * Получение id роли пользователя в базе данных
 * по его личному id
 * @param  id - личный id пользователя
 * @return role - id роли
 */
int Connection::getRoleByID(const QString& id) {
    int role = NONE;
    
    {
        if (connectToDBWithUser("getRole")) {
            QSqlDatabase check = QSqlDatabase::database("getRole");
            QSqlQuery query(check);
            query.prepare(QString("SELECT id, id_role FROM employees "
                                "WHERE id = :id;"));
            query.bindValue(":id", id);
            
            if (query.exec()) {
                query.first();
                role = query.value("id_role").toInt();
            }
            query.clear();
            check.close();
        }
    }
    QSqlDatabase::removeDatabase("getRole");
    
    return role;
}

QString Connection::getIDDepartmentByID(const QString& id) {
    QString id_department = "";
    
    {
        if (connectToDBWithUser("getDepartment")) {
            QSqlDatabase check = QSqlDatabase::database("getDepartment");
            QSqlQuery query(check);
            query.prepare(QString("SELECT id, id_department FROM employees "
                                "WHERE id = :id;"));
            query.bindValue(":id", id);
            
            if (query.exec()) {
                query.first();
                id_department = query.value("id_department").toString();
            }
            query.clear();
            check.close();
        }
    }
    QSqlDatabase::removeDatabase("getDepartment");
    
    return id_department;
}