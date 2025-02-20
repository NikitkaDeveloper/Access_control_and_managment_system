#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QKeyEvent>

#include "userprofile.h"
#include "chiefform.h"
#include "managerdbform.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @brief MainWindow
 * Класс графического интерфейса окна
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
private slots:
    void on_textLogin_textChanged();
    void on_linePass_textChanged(const QString &text);
    void on_buttonAuth_clicked();
    
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H