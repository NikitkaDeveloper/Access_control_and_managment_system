#include "userprofile.h"
#include "ui_userprofile.h"

userprofile::userprofile(QSqlQuery q, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::userprofile) {
    ui->setupUi(this);
    move(qApp->desktop()->availableGeometry(this).center()-rect().center());
    
    db = QSqlDatabase::database("user");
    query = q;
    
    this->setStyleSheet(
        "QTabBar::tab {background: transparent; color: rgb(255, 255, 255);}"
        // "QFrame {color: cyan;}"
        // "QLayout {bckground-color: rgba(0, 0, 0, 0);}"
        // "QLabel {color: rgb(255, 255, 255);}"
        "QPushButton:disabled {color: rgb(100, 100, 100);}"
        "QPushButton:enabled {color: rgb(255, 255, 255);}"
        "QLineEdit {background-color: rgba(0, 0, 0, 0); qproperty-frame: false;}"
        );
    
    fillInfo(); // Заполнение информации о сотруднике (кроме проектов)
    setSaveCancelButtonsVisible(false);
    today = ui->calendar->selectedDate();
    
    projectsFBox = new QFormLayout;
    projects = Connection::getProjectsByID(query.value("id").toString()); // Получение всех проектов сотрудника
    if (!projects.isEmpty()) {
        ui->labelNoProjects->setEnabled(false);
        ui->labelNoProjects->setVisible(false);
        ui->buttonEdit->setEnabled(true);
        fillProjects(false); // Заполнение вкладки "Проекты"
    }
    
    // Заполнение вкладки "История"
    file.setFileName("../ACMS/History/" + ui->lineName->text() + ".txt");
    file.open(QIODevice::ReadOnly);
    readHisory(today);
}

void userprofile::resizeEvent(QResizeEvent* event) {
    QPixmap pixmap(":/Resources/UI/user_background.png");
    pixmap = pixmap.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Window, pixmap);
    this->setPalette(palette);
    
    pixmap = QPixmap(":/Resources/UI/button.png");
    pixmap = pixmap.scaled(ui->buttonToday->size(), Qt::IgnoreAspectRatio);
    ui->buttonToday->setFlat(true);
    ui->buttonToday->setAutoFillBackground(true);
    palette = ui->buttonToday->palette();
    palette.setBrush(QPalette::Button, pixmap);
    ui->buttonToday->setPalette(palette);
    
    QWidget::resizeEvent(event);
}

TableAuthLogModel::TableAuthLogModel(QObject* parent) : QAbstractTableModel(parent) {}

int TableAuthLogModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent)
    return times.count();
}

int TableAuthLogModel::columnCount(const QModelIndex& parent) const {
    Q_UNUSED(parent)
    return LAST;
}

QVariant TableAuthLogModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role != Qt::DisplayRole)
        return QVariant();
    if(orientation == Qt::Vertical)
        return section;
    
    switch(section) {
    case TIME:
        return tr( "Время" );
    case STATUS:
        return tr( "Статус" );
    }
    return QVariant();
}

QVariant TableAuthLogModel::data(const QModelIndex& index, int role) const {
    if(
        !index.isValid() || log.count() <= index.row() ||
        (role != Qt::DisplayRole && role != Qt::EditRole)
        )
        return QVariant();
    
    return log[index.row()][Column(index.column())];
}

bool TableAuthLogModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if( !index.isValid() || role != Qt::EditRole || log.count() <= index.row())
        return false;
    
    log[index.row()][Column(index.column())] = value;
    emit dataChanged(index, index);
    
    return true;
}

Qt::ItemFlags TableAuthLogModel::flags(const QModelIndex& index) const {
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    return flags;
}

/**
 * @brief setupTableView
 * Первоначальная настройка tableAuthLog
 */
void userprofile::setupTableView() {
    ui->tableAuthLog->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    QHeaderView* header = ui->tableAuthLog->verticalHeader();
    header->setVisible(false);
    ui->tableAuthLog->setVerticalHeader(header);
    
    ui->tableAuthLog->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableAuthLog->setColumnWidth(TableAuthLogModel::STATUS, ui->tableAuthLog->width() - ui->tableAuthLog->columnWidth(TableAuthLogModel::TIME));
}

/**
 * @brief loadAuthLog
 * Загрузка данных в таблицу
 * полученных после вызова функции readHistory()
 * @param times
 * @param statuses
 */
void userprofile::loadAuthLog(const QVector<QString>& times, const QVector<QString>& statuses) {
    TableAuthLogModel::AuthData authdata;
    
    for (unsigned int i = 0; i < times.count(); ++i) {
        authdata[TableAuthLogModel::TIME] = times[i];
        authdata[TableAuthLogModel::STATUS] = statuses[i];
        dbmodel->log.append(authdata);
    }
    
    ui->tableAuthLog->setModel(dbmodel);
    setupTableView();
}

userprofile::~userprofile() {
    if (file.isOpen()) file.close();
    QSqlDatabase::removeDatabase("user");
    
    /* for (int i = 0; i < pProjectNames.size(); ++i) {
        delete pProjectNames.at(i);
        delete pProjectStatuses.at(i);
    } */
    clearProjectsInfo();
    delete projectsFBox;
    
    delete ui;
}

/**
 * @brief setSaveCancelButtonsVisible
 * Установка видимости кнопок сохранения и отмены изменений
 * @param status - состояние видимости элементов
 */
void userprofile::setSaveCancelButtonsVisible(bool status) {
    ui->buttonSave->setEnabled(status);
    ui->buttonCancel->setEnabled(status);
    ui->buttonSave->setVisible(status);
    ui->buttonCancel->setVisible(status);
}

/**
 * @brief readHisory
 * Считывание информации о посещении в конкретный день
 * в векторы times и statuses
 * @param date
 * @return true, если функция выполнена успешно, false, если нет
 */
bool userprofile::readHisory(const QDate& date) {
    delete dbmodel;
    dbmodel = new TableAuthLogModel;
    dbmodel->times.clear();
    dbmodel->statuses.clear();
    if (!file.isOpen()) return FAIL;
    QString d = date.toString("yyyy-MM-dd");
    QTextStream in(&file);
    in.setCodec("UTF-8");
    QString line;
    bool isfind = false;
    while (!in.atEnd()) {
        line = in.readLine();
        if (line == d) {
            line = in.readLine();
            unsigned short p;
            unsigned short max = line.count(';');
            for (unsigned short i = 0; i < max; ++i) {
                p = line.indexOf(' ');
                dbmodel->times.push_back(line.mid(0, p));
                line.remove(0, p + 1);
                p = line.indexOf(';');
                dbmodel->statuses.push_back(line.mid(0, p));
                line.remove(0, p + 1);
            }
            isfind = true;
            break;
        }
    }
    file.seek(0);
    loadAuthLog(dbmodel->times, dbmodel->statuses);
    if (!isfind) return FAIL;
    return SUCCESS;
}

void userprofile::on_calendar_clicked(const QDate &date) {
    readHisory(date);
}

void userprofile::on_buttonToday_clicked() {
    ui->calendar->showToday();
    ui->calendar->setSelectedDate(today);
    on_calendar_clicked(today);
}

/**
 * @brief fill
 * Заполнение полей информацией о сотруднике
 */
void userprofile::fillInfo() {
    query.first();
    
    QPixmap pic("../ACMS/Avatars/" + query.value("avatar").toString());
    QSize picsize(ui->avatar->size());
    pic = pic.scaled(picsize, Qt::KeepAspectRatio);
    ui->avatar->setPixmap(pic);
    
    ui->lineName->setText(query.value("fullname").toString());
    ui->lineDepartment->setText(query.value("department").toString());
    ui->lineJob->setText(query.value("job").toString());
    ui->lineSchedule->setText(query.value("schedule").toString());
    ui->lineStatus->setText(query.value("status").toString());
    ui->lineOrderIn->setText(query.value("orderin").toString());
    ui->lineOrderOut->setText(query.value("orderout").toString());
    ui->linePhone->setText(query.value("phone").toString());
    ui->lineEmail->setText(query.value("email").toString());
}

/**
 * @brief fillProjects
 * Заполнение вкладки "Проекты"
 * @param editMode - режим чтения/редактирования
 */
void userprofile::fillProjects(bool editMode) {
    QLabel* labelName = nullptr;
    QLabel* labelStatus = nullptr;
    QComboBox* comboBoxStatus = nullptr;
    const int pointSize = 11;
    for (QHash<QString, QString>::ConstIterator it = projects.cbegin(); it != projects.cend(); ++it) {
        labelName = new QLabel(it.key());
        pProjectNames.push_back(labelName);
        
        QFont font = labelName->font();
        font.setPointSize(pointSize);
        labelName->setFont(font);
        labelName->setMinimumWidth(ui->boxProjects->size().width() / 2);
        
        if (editMode) {
            QStringList statuses = Connection::getProjectStatuses();
            comboBoxStatus = new QComboBox;
            pProjectStatuses.push_back(comboBoxStatus);
            comboBoxStatus->addItems(statuses);
            comboBoxStatus->setCurrentText(it.value());
            connect(comboBoxStatus, &QComboBox::currentTextChanged, [this]() { statusIsChanged = true; });
            
            font = comboBoxStatus->font();
            font.setPointSize(pointSize);
            comboBoxStatus->setFont(font);  
            
            projectsFBox->addRow(labelName, comboBoxStatus);
        }
        else {
            labelStatus = new QLabel(it.value());
            pProjectStatuses.push_back(labelStatus);
            
            font = labelStatus->font();
            font.setPointSize(pointSize);
            labelStatus->setFont(font);  
            
            projectsFBox->addRow(labelName, labelStatus);
        }
        
        projectsFBox->setLabelAlignment(Qt::AlignLeft);
    }
    ui->boxProjects->setLayout(projectsFBox);
    
    // query.clear();
}

/**
 * @brief clearProjectsInfo
 * Очистка вкладки "Проекты"
 */
void userprofile::clearProjectsInfo() {
    while (!pProjectNames.isEmpty()) {
        delete pProjectNames.first();
        pProjectNames.pop_front();
        delete pProjectStatuses.first();
        pProjectStatuses.pop_front();
    }
}

/**
 * @brief on_buttonRefresh_clicked
 * Обновление вкладки "Проекты"
 */
void userprofile::on_buttonRefresh_clicked() {
    projects = Connection::getProjectsByID(query.value("id").toString());
    clearProjectsInfo();
    fillProjects(false);
}

/**
 * @brief on_buttonEdit_toggled
 * Переключение режима редактирования
 * @param checked - состояние зажатия кнопки редактирования
 */
void userprofile::on_buttonEdit_toggled(bool checked) {
    if (checked) {
        clearProjectsInfo();
        fillProjects(true);
    }
    else {
        /* if (statusIsChanged) {
            int check = QMessageBox::question(this, tr("Подтвердите действие"), tr("Есть несохранённые изменения.\nПродолжить?"));
            if (check == QMessageBox::No || check == QMessageBox::Cancel) {
                ui->buttonEdit->setChecked(true);
                return;
            }
            statusIsChanged = false;
        } */
        statusIsChanged = false;
        on_buttonRefresh_clicked();
    }
    
    setSaveCancelButtonsVisible(checked);
    ui->buttonRefresh->setEnabled(!checked);
}

/**
 * @brief on_buttonCancel_clicked
 * Отмена изменений на вкладке "Проекты" и переключение режима на чтение
 */
void userprofile::on_buttonCancel_clicked() {
    ui->buttonEdit->setChecked(false);
}

/**
 * @brief on_buttonSave_clicked
 * Сохранение изменений на вкладке "Проекты"
 */
void userprofile::on_buttonSave_clicked() {
    for (int i = 0; i < pProjectNames.size(); ++i) {
        if (!Connection::setProjectStatus(pProjectNames.at(i)->text(), static_cast<QComboBox*>(pProjectStatuses.at(i))->currentText())) {
            QMessageBox::critical(this, tr("Запрос не выполнен"), tr("Не удалось применить изменения!"));
            return;
        }
    }
    statusIsChanged = false;
}