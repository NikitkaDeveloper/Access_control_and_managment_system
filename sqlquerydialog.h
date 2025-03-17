#ifndef SQLQUERYDIALOG_H
#define SQLQUERYDIALOG_H

#include <QWidget>
#include <QDesktopWidget>
#include <QResizeEvent>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

namespace Ui {
class SqlQueryDialog;
}

class SqlQueryDialog : public QWidget {
    Q_OBJECT
    
public:
    explicit SqlQueryDialog(QWidget *parent = nullptr);
    ~SqlQueryDialog();
    
private slots:
    void on_buttonRun_clicked();
    void on_buttonClear_clicked();
    
private:
    virtual void resizeEvent(QResizeEvent* event) override;
    
    Ui::SqlQueryDialog *ui;
    QSqlDatabase db;
};

#endif // SQLQUERYDIALOG_H