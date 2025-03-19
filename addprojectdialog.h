#ifndef ADDPROJECTDIALOG_H
#define ADDPROJECTDIALOG_H

#include <QWidget>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QResizeEvent>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

namespace Ui {
class AddProjectDialog;
}

class AddProjectDialog : public QWidget {
    Q_OBJECT
    
public:
    explicit AddProjectDialog(QWidget* parent = nullptr);
    ~AddProjectDialog();
    
private slots:
    void on_buttonCancel_clicked();
    void on_buttonOk_clicked();
    
private:    
    virtual void resizeEvent(QResizeEvent* event) override;
    
    Ui::AddProjectDialog* ui;
    QSqlDatabase db;
};

#endif // ADDPROJECTDIALOG_H