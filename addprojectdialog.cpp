#include "addprojectdialog.h"
#include "ui_addprojectdialog.h"

AddProjectDialog::AddProjectDialog(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::AddProjectDialog) {
    ui->setupUi(this);
    move(qApp->desktop()->availableGeometry(this).center()-rect().center());
    
    this->setStyleSheet(
        "QLabel {color: rgb(255, 255, 255);}"
        "QMessageBox QLabel {color: rgb(0, 0, 0);}"
        "QPushButton:disabled {color: rgb(100, 100, 100);}"
        "QPushButton:enabled {color: rgb(255, 255, 255);}"
        "QMessageBox QPushButton:enabled {color: rgb(0, 0, 0);}"
        );
    
    db = QSqlDatabase::database("user");
}

AddProjectDialog::~AddProjectDialog() {
    delete ui;
}

void AddProjectDialog::resizeEvent(QResizeEvent* event) {
    QPixmap pixmap(":/Resources/UI/user_background.png");
    pixmap = pixmap.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Window, pixmap);
    this->setPalette(palette);
    
    pixmap = QPixmap(":/Resources/UI/auth_button.png");
    pixmap = pixmap.scaled(ui->buttonCancel->size(), Qt::IgnoreAspectRatio);
    ui->buttonCancel->setFlat(true);
    ui->buttonCancel->setAutoFillBackground(true);
    palette = ui->buttonCancel->palette();
    palette.setBrush(QPalette::Button, pixmap);
    ui->buttonCancel->setPalette(palette);
    
    pixmap = QPixmap(":/Resources/UI/auth_button.png");
    pixmap = pixmap.scaled(ui->buttonOk->size(), Qt::IgnoreAspectRatio);
    ui->buttonOk->setFlat(true);
    ui->buttonOk->setAutoFillBackground(true);
    palette = ui->buttonOk->palette();
    palette.setBrush(QPalette::Button, pixmap);
    ui->buttonOk->setPalette(palette);
    
    QWidget::resizeEvent(event);
}

void AddProjectDialog::on_buttonCancel_clicked() {
    if (QMessageBox::question(this, tr("Подтвердите действие"), tr("Вы точно хотите закрыть окно?")) == QMessageBox::Yes)
        this->close();
}

void AddProjectDialog::on_buttonOk_clicked() {
    if (!ui->lineInfo->text().isEmpty() && !ui->lineIDs->text().isEmpty()) {
        if (QMessageBox::question(this, tr("Подтвердите действие"), tr("Вы точно хотите создать заказ?")) == QMessageBox::Yes) {
            QString info = ui->lineInfo->text();
            QString ids = ui->lineIDs->text();
            
            QSqlQuery query(db);
            query.prepare(QString("INSERT INTO projects (project_name, id_employees, id_status) "
                                  "VALUES(:project_name, ARRAY[" + ids + "], 1);"));
            query.bindValue(":project_name", info);
            
            if (query.exec()) {
                QMessageBox::information(this, tr("Операция завершена"), tr("Запрос успешно выполнен"));
                this->close();
            }
            else
                QMessageBox::critical(this, tr("Ошибка"), tr("Запрос не выполнен"));
        }
    }
}