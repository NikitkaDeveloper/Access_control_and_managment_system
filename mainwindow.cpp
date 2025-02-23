#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    /* qApp->setStyleSheet(
        "QWidget {background-color: rgb(202, 240, 248);}"
        "QLabel {color: rgb(0, 119, 182);}"
        "QLineEdit {color: rgb(0, 180, 216);}"
        ); */
    move(qApp->desktop()->availableGeometry(this).center()-rect().center());
    
    ui->textLogin->setFocus();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_textLogin_textChanged() {
    QString text = ui->textLogin->toPlainText();
    if (!text.isEmpty() && !ui->linePass->text().isEmpty())
        ui->buttonAuth->setEnabled(true);
    else ui->buttonAuth->setEnabled(false);
    
    // Замена пробела в логине на \n
    if (text.endsWith(' ')) {
        text.remove(text.length() - 1, 1);
        ui->textLogin->setPlainText(text);
        ui->textLogin->moveCursor(QTextCursor::End);
        QKeyEvent* event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
        QApplication::sendEvent(ui->textLogin, event);
        delete event;
    }
}

void MainWindow::on_linePass_textChanged(const QString &text)
{
    if (!text.isEmpty() && !ui->linePass->text().isEmpty())
        ui->buttonAuth->setEnabled(true);
    else ui->buttonAuth->setEnabled(false);
}

void MainWindow::on_buttonAuth_clicked() {
    QString login = ui->textLogin->toPlainText();
    std::string str = login.toStdString();
    
    // Обратная замена всех \n в логине на пробелы
    for (unsigned short i = str.find('\n'); i != (unsigned short)std::string::npos; i = str.find('\n'))
        str.replace(i, 1, " ");
    login = QString::fromStdString(str);
    QString pass = ui->linePass->text();
    
    QString id = Connection::getIDByAuthData(login, pass);
    
    int role = Connection::NONE;
    if (id != "")
        role = Connection::getRoleByID(id);
    else {
        QMessageBox::critical(this, tr("Ошибка аутентификации"), tr("Указан неверный логин или пароль!"));
        return;
    }
    
    // Подключение к базе данных в зависимости от роли пользователя
    switch (role) {
    case Connection::USER: {                    // Если подключается обычный пользователь
        if (!Connection::connectToDBWithUser()) // Подключение к БД с правами обычного пользователя
            return;
        
        userprofile* window = new userprofile(Connection::getInfoAboutUser(id)); // Создание окна сотрудника и получение информации о нём
        window->show();
        this->close();
        break;
    }
    case Connection::CHIEF: {                   // Если подключается начальник отдела
        if (!Connection::connectToDBWithUser()) // Подключение к БД с правами обычного пользователя
            return;
        
        QString id_department = Connection::getIDDepartmentByID(id);                     // Получение id отдела
        chiefForm* window = new chiefForm(Connection::getInfoAboutChief(id_department)); // Создание окна начальника и получение списка сотрудников
        window->show();
        this->close();
        break;
    }
    case Connection::MANAGER: {                      // Если подключается менеджер БД
        if (!Connection::connectToDBWithManager())   // Подключение к БД с правами менеджера БД
            return;
        
        managerDBForm* window = new managerDBForm(); // Создание окна менеджера БД
        window->show();
        this->close();
        break;
    }
    default:
        QMessageBox::critical(this, tr("Ошибка аутентификации"), tr("Не удалось подключиться к серверу!"));
        ui->textLogin->clear();
        ui->linePass->clear();
        break;
    }
}