#include "chiefform.h"
#include "ui_chiefform.h"

chiefForm::chiefForm(QSqlQuery q, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::chiefForm) {
    ui->setupUi(this);
    move(qApp->desktop()->availableGeometry(this).center()-rect().center());
    
    this->setStyleSheet(
        "QFrame {color: orange;}"
        "QGroupBox {background-color: transparent;}"
        "QGroupBox:title {color: rgb(255, 255, 255);}"
        "QLabel {color: rgb(255, 255, 255);}"
        "QPushButton:disabled {color: rgb(100, 100, 100);}"
        "QPushButton:enabled {color: rgb(255, 255, 255);}"
        "QLineEdit {background-color: rgba(0, 0, 0, 0); color: rgb(255, 255, 255); qproperty-frame: false;}"
        "QMessageBox QLabel {color: rgb(0, 0, 0);}"
        "QMessageBox QPushButton:enabled {color: rgb(0, 0, 0);}"
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

void chiefForm::resizeEvent(QResizeEvent* event) {
    QPixmap pixmap(":/Resources/UI/chief_background.png");
    pixmap = pixmap.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Window, pixmap);
    this->setPalette(palette);
    
    pixmap = QPixmap(":/Resources/UI/auth_button.png");
    pixmap = pixmap.scaled(ui->buttonInfoCard->size(), Qt::IgnoreAspectRatio);
    ui->buttonInfoCard->setFlat(true);
    ui->buttonInfoCard->setAutoFillBackground(true);
    palette = ui->buttonInfoCard->palette();
    palette.setBrush(QPalette::Button, pixmap);
    ui->buttonInfoCard->setPalette(palette);
    
    if (!pButtons.isEmpty()) {
        pixmap = QPixmap(":/Resources/UI/item.png");
        for (int i = 0; i < pButtons.size(); ++i) {
            pixmap = pixmap.scaled(pButtons.at(i)->size(), Qt::IgnoreAspectRatio);
            pButtons.at(i)->setFlat(true);
            pButtons.at(i)->setAutoFillBackground(true);
            palette = pButtons.at(i)->palette();
            palette.setBrush(QPalette::Button, pixmap);
            pButtons.at(i)->setPalette(palette);
        }
    }
    
    QWidget::resizeEvent(event);
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
        employee = new QPushButton(list.value("id").toString() + ". " + list.value("fullname").toString());
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
    
    QPixmap pic("../ACMS/Avatars/" + list.value("avatar").toString());
    QSize picsize(ui->labelAvatar->size());
    pic = pic.scaled(picsize, Qt::KeepAspectRatio);
    ui->labelAvatar->setPixmap(pic);
    
    ui->lineStatus->setText(list.value("status").toString());
    ui->linePhone->setText(list.value("phone").toString());
    ui->lineEmail->setText(list.value("email").toString());
    
    ui->lineProject->setText(Connection::getCurrentProjectByID(list.value("id").toString()));
}

void chiefForm::on_buttonInfoCard_clicked() {
    QString id = list.value("id").toString();
    profile = new userprofile(Connection::getInfoAboutUser(id));
    connect(profile, &QWidget::destroyed, this, &chiefForm::clearUserProfileMember);
    profile->show();
}

void chiefForm::clearUserProfileMember() {
    if (profile != nullptr) {
        delete profile;
        profile = nullptr;
    }
}