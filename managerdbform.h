#ifndef MANAGERDBFORM_H
#define MANAGERDBFORM_H

#include <QWidget>
#include <QMessageBox>
#include <QSqlTableModel>
#include <QLabel>

#include "sqlquerydialog.h"

namespace Ui {
class managerDBForm;
}

class managerDBForm : public QWidget {
    Q_OBJECT
    
public:
    explicit managerDBForm(QWidget *parent = nullptr);
    ~managerDBForm();
    
    bool connectDB();
    bool loadTable(const QString& table_name);
    
private slots:
    void on_TbuttonOptions_clicked();
    
    void on_comboBoxTables_currentTextChanged(const QString& table_name);
    
    void on_buttonReadOnly_toggled(bool checked);
    void on_buttonRefresh_clicked();
    void on_buttonAddRow_clicked();
    
    void selectedRowChanged();
    void on_buttonDelRow_clicked();
    
    void tableDataChanged();
    void on_buttonSubmitChanges_clicked();
    void on_buttonRevertChanges_clicked();
    
    void on_buttonSQLQuery_clicked();
    
private:
    void resizeUIElementByContent(QWidget& el, const QString& text);
    void getListFromQueryColumn(QStringList& list, QSqlQuery& query, const QString& column_name);
    void initializeModel(QSqlTableModel* model);
    void setTableInModel(const QString& table_name);
    
    const bool FAIL = 0;
    const bool SUCCESS = 1;
    
    Ui::managerDBForm *ui;
    QSqlDatabase db;
    QSqlTableModel* model = nullptr;
    QStringList tables;                 // Таблицы
    QString currentTable;               // Открытая таблица
    QStringList headers;                // Список заголовков таблицы
    QStringList types;                  // Список типов данных столбцов
    
    std::unique_ptr<SqlQueryDialog> querydlg = std::make_unique<SqlQueryDialog>(); // Окно с созданием sql-запроса
};

#endif // MANAGERDBFORM_H