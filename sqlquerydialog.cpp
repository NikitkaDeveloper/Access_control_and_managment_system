#include "sqlquerydialog.h"
#include "ui_sqlquerydialog.h"

SqlQueryDialog::SqlQueryDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SqlQueryDialog) {
    ui->setupUi(this);
    move(qApp->desktop()->availableGeometry(this).center()-rect().center());
    
    this->setStyleSheet(
        "QLabel {color: rgb(255, 255, 255);}"
        "QPushButton:disabled {color: rgb(100, 100, 100);}"
        "QPushButton:enabled {color: rgb(255, 255, 255);}"
        "QMessageBox QLabel {color: rgb(0, 0, 0);}"
        "QMessageBox QPushButton:enabled {color: rgb(0, 0, 0);}"
        );
    
    db = QSqlDatabase::database("manage");
}

SqlQueryDialog::~SqlQueryDialog() {
    delete ui;
}

void SqlQueryDialog::resizeEvent(QResizeEvent* event) {
    QPixmap pixmap(":/Resources/UI/user_background.png");
    pixmap = pixmap.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Window, pixmap);
    this->setPalette(palette);
    
    pixmap = QPixmap(":/Resources/UI/auth_button.png");
    pixmap = pixmap.scaled(ui->buttonClear->size(), Qt::IgnoreAspectRatio);
    ui->buttonClear->setFlat(true);
    ui->buttonClear->setAutoFillBackground(true);
    palette = ui->buttonClear->palette();
    palette.setBrush(QPalette::Button, pixmap);
    ui->buttonClear->setPalette(palette);
    
    pixmap = QPixmap(":/Resources/UI/auth_button.png");
    pixmap = pixmap.scaled(ui->buttonRun->size(), Qt::IgnoreAspectRatio);
    ui->buttonRun->setFlat(true);
    ui->buttonRun->setAutoFillBackground(true);
    palette = ui->buttonRun->palette();
    palette.setBrush(QPalette::Button, pixmap);
    ui->buttonRun->setPalette(palette);
    
    QWidget::resizeEvent(event);
}

void SqlQueryDialog::on_buttonRun_clicked() {
    QString str = ui->textQuery->toPlainText();
    if (str.isEmpty()) {
        ui->textResult->setText("Пустой запрос");
        return;
    }
    
    QSqlQuery query(db);
    query.exec(str);
    
    if (query.isActive())
        ui->textResult->setText("Успех");
    else ui->textResult->setText("Ошибка");
}

void SqlQueryDialog::on_buttonClear_clicked() {
    ui->textQuery->clear();
}