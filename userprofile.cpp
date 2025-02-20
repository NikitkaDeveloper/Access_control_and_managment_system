#include "userprofile.h"
#include "ui_userprofile.h"

userprofile::userprofile(QSqlQuery q, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::userprofile) {
    ui->setupUi(this);
    move(qApp->desktop()->availableGeometry(this).center()-rect().center());
    
    db = QSqlDatabase::database("user");
    query = q;
    
    window()->setStyleSheet(
        "QLineEdit {background-color: rgba(0, 0, 0, 0); qproperty-frame: false;}"
        );
    
    fill();
    today = ui->calendar->selectedDate();
    
    file.setFileName("../ACMS/History/" + ui->lineName->text() + ".txt");
    file.open(QIODevice::ReadOnly);
    readHisory(today);
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
    delete ui;
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
void userprofile::fill() {
    query.first();
    
    QPixmap pic("../ACMS/" + query.value("avatar").toString());
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
    
    query.clear();
}

void userprofile::on_buttonOpenTextEditor_clicked() {
    txtEdit.show();
}