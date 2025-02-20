#ifndef CONNECTION_H
#define CONNECTION_H

#include <QWidget>
#include <QVariant>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

/**
 * @brief Connection
 * Класс для работы с базой данных
 */
class Connection {
public:
    static QString getIDByAuthData(const QString& login, const QString& pass);
    static int getRoleByID(const QString& id);
    static QString getIDDepartmentByID(const QString& id);
    static bool connectToDBWithUser(const QString& connectionName = "user");
    static QSqlQuery getInfoAboutUser(const QString& id);
    static QSqlQuery getInfoAboutChief(const QString& id_department);
    static bool connectToDBWithManager(const QString& connectionName = "manage");
    
    enum Roles {  // Роли в базе данных
        USER = 1, // Обычный пользователь
        CHIEF,    // Начальник отдела
        MANAGER,  // Менеджер базы данных
        NONE      // Отсутствие роли
    };
};

#endif // CONNECTION_H