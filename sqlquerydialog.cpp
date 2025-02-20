#include "sqlquerydialog.h"
#include "ui_sqlquerydialog.h"

SqlQueryDialog::SqlQueryDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SqlQueryDialog) {
    ui->setupUi(this);
    move(qApp->desktop()->availableGeometry(this).center()-rect().center());
    
    db = QSqlDatabase::database("manage");
}

SqlQueryDialog::~SqlQueryDialog() {
    delete ui;
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