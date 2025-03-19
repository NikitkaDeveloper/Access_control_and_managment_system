#ifndef USERPROFILE_H
#define USERPROFILE_H

#include <QWidget>
#include <QAbstractTableModel>
#include <QDesktopWidget>
#include <QFormLayout>
#include <QLabel>
#include <QComboBox>
#include <QFont>
#include <qdatetime.h>
#include <QFile>
#include <QTextStream>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "connection.h"
#include "addprojectdialog.h"

namespace Ui {
class userprofile;
}

/**
 * @brief The TableAuthLogModel class
 * Класс модели для элемента tableAuthLog
 * для вывода информации о посещении
 */
class TableAuthLogModel : public QAbstractTableModel {
    Q_OBJECT
    
public:
    TableAuthLogModel(QObject* parent = nullptr);
    
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    
    enum Column {                             // Список колонок таблицы
        TIME = 0,                             // Колонка "Время"
        STATUS,                               // Колонка "Статус"
        LAST
    };
    QVector<QString> times;
    QVector<QString> statuses;
    
    typedef QHash<Column, QVariant> AuthData; // Содержит время и статус перемещения человека
    typedef QList<AuthData> Log;
    Log log;
};

class userprofile : public QWidget {
    Q_OBJECT

public:
    explicit userprofile(QSqlQuery q, QWidget *parent = nullptr);
    ~userprofile();
    
    TableAuthLogModel* dbmodel = nullptr;     // Модель элемента tableAuthLog
    
private slots:
    void on_calendar_clicked(const QDate &date);
    void on_buttonToday_clicked();
    
    void on_buttonRefresh_clicked();
    void on_buttonEdit_toggled(bool checked);
    void on_buttonCancel_clicked();
    void on_buttonSave_clicked();
    void on_buttonAddProject_clicked();
    
private:
    void fillInfo();
    void setupTableView();
    bool readHisory(const QDate& date);
    void loadAuthLog(const QVector<QString>& times, const QVector<QString>& statuses);
    void fillProjects(bool editMode);
    void clearProjectsInfo();
    void setSaveCancelButtonsVisible(bool status);
    void clearDialogMemder();
    
    virtual void resizeEvent(QResizeEvent* event) override;
    
    const bool FAIL = 0;
    const bool SUCCESS = 1;
    
    Ui::userprofile *ui;
    
    QFormLayout* projectsFBox = nullptr;
    QHash<QString, QString> projects;
    QList<QLabel*> pProjectNames;        // Лист с указателями на элементы с названиями проектов в boxProjects
    QList<QWidget*> pProjectStatuses;    // Лист с указателями на элементы со статусами проектов в boxProjects
    
    bool statusIsChanged = false;        // Если были изменения в статусе(ах) проекта(ов), то значение станет true
    QFile file;
    QSqlDatabase db;
    QSqlQuery query;
    QDate today;
    
    AddProjectDialog* addprojectdialog = nullptr;
};

#endif // USERPROFILE_H