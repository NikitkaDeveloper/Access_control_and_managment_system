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

/**
 * @brief connectToDBWithUser
 * Подключение к базе данных
 * с привилегиями обычного пользователя
 * @param connectionName - имя подключения
 * @return true в случае успеха, false при неудаче
 */
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

/**
 * @brief getInfoAboutUser
 * Получение информации о сотруднике (без проектов)
 * @param id - id сотрудника
 * @return выполненный запрос query со всей информацией
 */
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


/**
 * @brief getInfoAboutChief
 * Получение списка сотрудников в отделе
 * @param id_department - id отдела
 * @return выполненный запрос query со всей информацией
 */
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

/**
 * @brief connectToDBWithManager
 * Подключение к базе данных
 * с привилегиями менеджера БД
 * @param connectionName - имя подключения
 * @return true в случае успеха, false при неудаче
 */
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
 * @param id - личный id пользователя
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

/**
 * @brief getIDDepartmentByID
 * Получение id отдела сотрудника из базы данных
 * по его личному id
 * @param id - личный id пользователя
 * @return id_department - id отдела
 */
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

/**
 * @brief getProjectStatuses
 * Получение возможных статусов проекта
 * @return statuses - список статусов
 */
QStringList Connection::getProjectStatuses() {
    QStringList statuses;
    
    {
        if (connectToDBWithUser("getProjectStatuses")) {
            QSqlDatabase check = QSqlDatabase::database("getProjectStatuses");
            QSqlQuery query(check);
            query.exec(QString("SELECT status FROM project_statuses;"));
            
            if (query.isActive()) {
                while (query.next())
                    statuses.push_back(query.value("status").toString());
            }
            query.clear();
            check.close();
        }
    }
    QSqlDatabase::removeDatabase("getProjectStatuses");
    
    return statuses;
}

/**
 * @brief getProjectsByID
 * Получение всех проектов, где
 * задействован сотрудник
 * @param id - личный id сотрудника
 * @return projects - хэш-таблица с названием проекта
 * в качестве ключа и его статусом как значением
 */
QHash<QString, QString> Connection::getProjectsByID(const QString& id) {
    QHash<QString, QString> projects;
    
    {
        if (connectToDBWithUser("getProjects")) {
            QSqlDatabase check = QSqlDatabase::database("getProjects");
            QSqlQuery query(check);
            query.prepare(QString("SELECT p.*, ps.* FROM projects p "
                                  "INNER JOIN project_statuses ps "
                                  "ON p.id_status = ps.id "
                                  "WHERE :id = any(p.id_employees);"));
            query.bindValue(":id", id);
            
            if (query.exec()) {
                while (query.next())
                    projects.insert(query.value("project_name").toString(),
                                    query.value("status").toString());
            }
            query.clear();
            check.close();
        }
    }
    QSqlDatabase::removeDatabase("getProjects");
    
    return projects;
}

/**
 * @brief setProjectStatus
 * Установка статуса проекта
 * @param project_name - имя проекта
 * @param status - новый статус проекта
 * @return true в случае успеха, false при неудаче
 */
bool Connection::setProjectStatus(const QString& project_name, const QString& status) {
    {
        if (connectToDBWithManager("setProjectStatus")) {
            QSqlDatabase check = QSqlDatabase::database("setProjectStatus");
            QSqlQuery query(check);
            query.prepare(QString("SELECT * FROM project_statuses "
                                  "WHERE status = :status;"));
            query.bindValue(":status", status);
            
            if (query.exec()) {
                query.first();
                int id_status = query.value("id").toInt();
                
                query.prepare(QString("UPDATE projects "
                                      "SET id_status = :id_status "
                                      "WHERE project_name = :project_name;"));
                query.bindValue(":project_name", project_name);
                query.bindValue(":id_status", id_status);
                
                if (!query.exec())
                    return false;
            }
            else return false;
            query.clear();
            check.close();
        }
    }
    QSqlDatabase::removeDatabase("setProjectStatus");
    
    return true;
}

/**
 * @brief getCurrentProjectByID
 * Получение текущего проекта сотрудника
 * по личному id
 * @param id - личный id сотрудника
 * @return project - текущий проект
 */
QString Connection::getCurrentProjectByID(const QString &id) {
    QString project = "";
    
    {
        if (connectToDBWithUser("getCurrentProject")) {
            QSqlDatabase check = QSqlDatabase::database("getCurrentProject");
            QSqlQuery query(check);
            query.prepare(QString("SELECT p.*, ps.* FROM projects p "
                                  "INNER JOIN project_statuses ps "
                                  "ON p.id_status = ps.id "
                                  "WHERE :id = any(p.id_employees) AND "
                                  "(ps.status = 'Проектирование' OR "
                                  "ps.status = 'Кодирование' OR "
                                  "ps.status = 'Тестирование');"));
            query.bindValue(":id", id);
            
            if (query.exec()) {
                query.first();
                if (!query.isValid())
                    project = "Отсутствует";
                else
                    project = query.value("project_name").toString();
            }
            query.clear();
            check.close();
        }
    }
    QSqlDatabase::removeDatabase("getCurrentProject");
    
    return project;
}