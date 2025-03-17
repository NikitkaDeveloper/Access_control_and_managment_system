#ifndef CHIEFFORM_H
#define CHIEFFORM_H

#include <QDesktopWidget>
#include <QVBoxLayout>
#include <QPushButton>

#include "connection.h"
#include "userprofile.h"

namespace Ui {
class chiefForm;
}

class chiefForm : public QWidget {
    Q_OBJECT
    
public:
    explicit chiefForm(QSqlQuery q, QWidget *parent = nullptr);
    ~chiefForm();
    
    void fillBoxEmployees();
    void fillInfo();
    
private slots:
    void on_buttonInfoCard_clicked();
    void clearUserProfileMember();
    
private:
    virtual void resizeEvent(QResizeEvent* event) override;
    bool getSelectedButtonIndex();
    
    Ui::chiefForm *ui;
    QSqlQuery list;
    int pos = 0;
    bool isStartNow = true;
    bool boxEmloyeesIsEmpty = true;
    QPushButton* employee = nullptr;
    QVector<QPushButton*> pButtons;
    QVBoxLayout* employeesVBox = nullptr;
    
    userprofile* profile = nullptr;
};

#endif // CHIEFFORM_H