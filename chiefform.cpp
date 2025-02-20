#include "chiefform.h"
#include "ui_chiefform.h"

chiefForm::chiefForm(QSqlQuery q, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::chiefForm) {
    ui->setupUi(this);
    move(qApp->desktop()->availableGeometry(this).center()-rect().center());
    window()->setStyleSheet(
        "QLineEdit {background-color: rgba(0, 0, 0, 0); qproperty-frame: false;}"
        );
    list = q;
    
    employeesVBox = new QVBoxLayout;
    
    fillBoxEmployees();
    if (!boxEmloyeesIsEmpty) {
        pButtons.first()->setChecked(true);
        isStartNow = false;
    }
}

chiefForm::~chiefForm() {
    QSqlDatabase::removeDatabase("user");
    
    for (int i = 0; i < pButtons.size(); ++i)
        delete pButtons.at(i);
    delete employeesVBox;
    
    delete ui;
}

bool chiefForm::getSelectedButtonIndex() {
    for (int i = 0; i < pButtons.size(); ++i) {
        if (pButtons.at(i)->isChecked() && i != pos) {
            pButtons.at(pos)->setChecked(false);
            pos = i;
            return true;
        }
    }
    return false;
}

void chiefForm::fillBoxEmployees() {
    while (list.next()) {
        employee = new QPushButton(list.value("fullname").toString());
        pButtons.push_back(employee);
        employeesVBox->addWidget(employee);
        
        employee->setCheckable(true);
        connect(employee, &QPushButton::toggled, this, &chiefForm::fillInfo);
        
        boxEmloyeesIsEmpty = false;
    }
    employeesVBox->addStretch(1);
    ui->boxEmployees->setLayout(employeesVBox);
}

void chiefForm::fillInfo() {
    if (!getSelectedButtonIndex() && !isStartNow) {
        pButtons.at(pos)->setChecked(true);
        return;
    }
    
    list.seek(pos);
    
    QPixmap pic("../ACMS/" + list.value("avatar").toString());
    QSize picsize(ui->labelAvatar->size());
    pic = pic.scaled(picsize, Qt::KeepAspectRatio);
    ui->labelAvatar->setPixmap(pic);
    
    ui->lineStatus->setText(list.value("status").toString());
    ui->linePhone->setText(list.value("phone").toString());
    ui->lineEmail->setText(list.value("email").toString());
}

void chiefForm::on_buttonInfoCard_clicked() {
    QString id = list.value("id").toString();
    profile = new userprofile(Connection::getInfoAboutUser(id));
    connect(profile, &QWidget::destroyed, this, &chiefForm::clearUserProfileMember);
    profile->show();
}

void chiefForm::clearUserProfileMember() {
    if (profile != nullptr)
        delete profile;
}