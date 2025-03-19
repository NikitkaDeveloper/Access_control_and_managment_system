#include "managerdbform.h"
#include "ui_managerdbform.h"

managerDBForm::managerDBForm(const QString& id, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::managerDBForm) {
    ui->setupUi(this);
    move(qApp->desktop()->availableGeometry(this).center()-rect().center());
    
    this->setStyleSheet(
        "QComboBox {background-color: rgb(127, 67, 153); color: white;}"
        "QAbstractItemView {background-color: rgb(127, 67, 153); color: white;}"
        "QTableView {background-color: transparent; color: rgb(255, 255, 255);}"
        "QHeaderView::section {background-color: transparent; color: rgb(255, 255, 255);}"
        "QHeaderView {background-color: transparent;}"
        "QTableCornerButton::section {background-color: transparent;}"
        "QScrollBar {background-color: white;}"
        "QScrollBar::handle {min-height: 0px; border: 0px solid white; border-radius: 10px; background-color: orange;}"
        "QScrollBar::add-line {background-color: transparent;}"
        "QScrollBar::sub-line {background-color: transparent;}"
        "QAbstractScrollArea::corner {background: white;}"
        "QLabel {color: rgb(255, 255, 255);}"
        "QPushButton:disabled {color: rgb(100, 100, 100);}"
        "QPushButton:enabled {color: rgb(255, 255, 255);}"
        "QMessageBox QLabel {color: rgb(0, 0, 0);}"
        "QMessageBox QPushButton:enabled {color: rgb(0, 0, 0);}"
        );
    ui->buttonSQLQuery->setStyleSheet("QPushButton { background-color: none; border: none; }");
    
    ui->labelDBName->setAlignment(Qt::AlignCenter);
    ui->labelTableName->setAlignment(Qt::AlignCenter);
    
    db = QSqlDatabase::database("manage");
    model = new QSqlTableModel(this, db);    
    initializeModel(model);
    
    if (Connection::connectToDBWithUser())
        profile = std::make_unique<userprofile>(Connection::getInfoAboutUser(id));
    
    connectDB();
}

managerDBForm::~managerDBForm() {
    querydlg->close();
    delete model;
    QSqlDatabase::removeDatabase("manage");
    delete ui;
}

void managerDBForm::resizeEvent(QResizeEvent* event) {
    QPixmap pixmap(":/Resources/UI/user_background.png");
    pixmap = pixmap.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Window, pixmap);
    this->setPalette(palette);
    
    pixmap = QPixmap(":/Resources/UI/button.png");
    pixmap = pixmap.scaled(ui->buttonAddRow->size(), Qt::IgnoreAspectRatio);
    ui->buttonAddRow->setFlat(true);
    ui->buttonAddRow->setAutoFillBackground(true);
    palette = ui->buttonAddRow->palette();
    palette.setBrush(QPalette::Button, pixmap);
    ui->buttonAddRow->setPalette(palette);
    
    pixmap = QPixmap(":/Resources/UI/button.png");
    pixmap = pixmap.scaled(ui->buttonDelRow->size(), Qt::IgnoreAspectRatio);
    ui->buttonDelRow->setFlat(true);
    ui->buttonDelRow->setAutoFillBackground(true);
    palette = ui->buttonDelRow->palette();
    palette.setBrush(QPalette::Button, pixmap);
    ui->buttonDelRow->setPalette(palette);
    
    pixmap = QPixmap(":/Resources/UI/auth_button.png");
    pixmap = pixmap.scaled(ui->buttonReadOnly->size(), Qt::IgnoreAspectRatio);
    ui->buttonReadOnly->setFlat(true);
    ui->buttonReadOnly->setAutoFillBackground(true);
    palette = ui->buttonReadOnly->palette();
    palette.setBrush(QPalette::Button, pixmap);
    ui->buttonReadOnly->setPalette(palette);
    
    pixmap = QPixmap(":/Resources/UI/auth_button.png");
    pixmap = pixmap.scaled(ui->buttonRefresh->size(), Qt::IgnoreAspectRatio);
    ui->buttonRefresh->setFlat(true);
    ui->buttonRefresh->setAutoFillBackground(true);
    palette = ui->buttonRefresh->palette();
    palette.setBrush(QPalette::Button, pixmap);
    ui->buttonRefresh->setPalette(palette);
    
    pixmap = QPixmap(":/Resources/UI/auth_button.png");
    pixmap = pixmap.scaled(ui->buttonRevertChanges->size(), Qt::IgnoreAspectRatio);
    ui->buttonRevertChanges->setFlat(true);
    ui->buttonRevertChanges->setAutoFillBackground(true);
    palette = ui->buttonRevertChanges->palette();
    palette.setBrush(QPalette::Button, pixmap);
    ui->buttonRevertChanges->setPalette(palette);
    
    pixmap = QPixmap(":/Resources/UI/auth_button.png");
    pixmap = pixmap.scaled(ui->buttonSubmitChanges->size(), Qt::IgnoreAspectRatio);
    ui->buttonSubmitChanges->setFlat(true);
    ui->buttonSubmitChanges->setAutoFillBackground(true);
    palette = ui->buttonSubmitChanges->palette();
    palette.setBrush(QPalette::Button, pixmap);
    ui->buttonSubmitChanges->setPalette(palette);
    
    pixmap = QPixmap(":/Resources/UI/auth_button.png");
    pixmap = pixmap.scaled(ui->buttonInfoCard->size(), Qt::IgnoreAspectRatio);
    ui->buttonInfoCard->setFlat(true);
    ui->buttonInfoCard->setAutoFillBackground(true);
    palette = ui->buttonInfoCard->palette();
    palette.setBrush(QPalette::Button, pixmap);
    ui->buttonInfoCard->setPalette(palette);
    
    QWidget::resizeEvent(event);
}

/**
 * @brief resizeUIElementByContent
 * Изменение размера элемента под его текст
 * @param el   - изменяемый элемент
 * @param text - его содержимый текст
 */
void managerDBForm::resizeUIElementByContent(QWidget& el, const QString &text) {
    QFontMetrics fm(el.fontMetrics());
    QRect rect = el.geometry();
    rect.setWidth(fm.horizontalAdvance(text));
    el.setGeometry(rect);
}

/**
 * @brief getListFromQueryColumn
 * Сохранение результата SQL-функции ARRAY_AGG
 * в QStringList
 * @param list        - Лист для сохранения данных
 * @param query       - Выполненный SQL-запрос
 * @param column_name - Имя столбца в базе данных со списком
 */
void managerDBForm::getListFromQueryColumn(QStringList &list, QSqlQuery& query, const QString& column_name) {
    query.first();
    QString str;
    str = query.value(column_name).toString();    
    str.remove(0, 1);
    str.chop(1);
    list = str.split(',');
}

/**
 * @brief initializeModel
 * Инициализация модели tableView
 * @param model - модель tableView
 */
void managerDBForm::initializeModel(QSqlTableModel* model) {
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    
    ui->tableView->setModel(model);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    
    connect(ui->tableView->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            // SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
            this,
            &managerDBForm::selectedRowChanged);
            // SLOT(on_tableView_selectionChanged()));
    
    connect(ui->tableView->model(),
            SIGNAL(dataChanged(QModelIndex, QModelIndex)),
            this,
            SLOT(tableDataChanged()));
}

/**
 * @brief setTableInModel
 * Установка таблицы в модель tableView
 * @param table_name - имя таблицы базы данных
 */
void managerDBForm::setTableInModel(const QString &table_name) {
    model->setTable(table_name);
    model->select();
    
    if (ui->buttonReadOnly->isChecked())
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    else ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
    
    ui->buttonDelRow->setEnabled(false);
    ui->tableView->resizeColumnsToContents();
}

/**
 * @brief loadTable
 * Получение информации и загрузка таблицы
 * @param table_name - имя таблицы
 * @return true, если функция выполнена успешно, false, если нет
 */
bool managerDBForm::loadTable(const QString& table_name) {
    QSqlQuery query(db);
    query.prepare(QString("SELECT ARRAY_AGG(column_name) AS columns, "
                       "ARRAY_AGG(data_type) AS types "
                       "FROM information_schema.columns "
                       "WHERE table_name = :table_name;"));
    query.bindValue(":table_name", table_name);
    
    if (!query.exec()) {
        QMessageBox::critical(this, tr("Ошибка подключения"), tr("Таблица не существует!"));
        return FAIL;
    }
    
    getListFromQueryColumn(headers, query, "columns");
    getListFromQueryColumn(types, query, "types");
    query.clear();
    
    setTableInModel(table_name);
    
    return SUCCESS;
}

/**
 * @brief connectDB
 * Подключение к базе данных
 * @return true, если функция выполнена успешно, false, если нет
 */
bool managerDBForm::connectDB() {
    ui->labelDBName->setText(db.databaseName());
    resizeUIElementByContent(*ui->labelDBName, ui->labelDBName->text());
    
    ui->comboBoxTables->clear();
    
    QSqlQuery query(db);
    query.exec(QString("SELECT ARRAY_AGG(t.table_name) AS tables FROM ("
                       "SELECT table_name FROM information_schema.tables "
                       "WHERE table_schema NOT IN ('information_schema','pg_catalog')) AS t;"));
    if (!query.first()) {
        QMessageBox::critical(this, tr("Ошибка подключения"), tr("База данных не имеет таблиц!"));
        return FAIL;
    }
    
    getListFromQueryColumn(tables, query, "tables");
    query.clear();
    
    ui->comboBoxTables->addItems(tables);
    ui->comboBoxTables->setEnabled(true);
    
    if (loadTable(ui->comboBoxTables->currentText())) {
        currentTable = ui->comboBoxTables->currentText();
        
        ui->labelTableName->setText(currentTable);
        resizeUIElementByContent(*ui->labelTableName, ui->labelTableName->text());
        
        ui->buttonSQLQuery->setEnabled(true);
        ui->buttonInfoCard->setEnabled(true);
        if (!ui->buttonReadOnly->isChecked())
            ui->buttonAddRow->setEnabled(true);
    }
    
    return SUCCESS;
}

/**
 * @brief on_buttonInfoCard_clicked
 * Отображение личной карточки менеджера БД
 */
void managerDBForm::on_buttonInfoCard_clicked() {
    profile->show();
}

/**
 * @brief on_comboBoxTables_currentTextChanged
 * Слот изменения текущей таблицы в comboBoxTables
 * @param table_name - имя выбранной таблицы
 */
void managerDBForm::on_comboBoxTables_currentTextChanged(const QString& table_name) {
    // Выбор действий, если в таблице есть изменения
    if (model->isDirty()) {
        switch (QMessageBox::question(this, tr("Подтвердите действие"), tr("Сохранить изменённые данные?"))) {
        case QMessageBox::Yes:
            on_buttonSubmitChanges_clicked();
            break;
        case QMessageBox::No:
            on_buttonRevertChanges_clicked();
            break;
        default:
            return;
            break;
        }
    }
    
    if (loadTable(ui->comboBoxTables->currentText())) {
        currentTable = ui->comboBoxTables->currentText();
        
        ui->labelTableName->setText(currentTable);
        resizeUIElementByContent(*ui->labelTableName, ui->labelTableName->text());
    }
    else ui->comboBoxTables->setCurrentText(currentTable);
}

/**
 * @brief on_buttonReadOnly_toggled
 * Включение/отключение режима "только чтение"
 * для таблицы
 * @param checked
 */
void managerDBForm::on_buttonReadOnly_toggled(bool checked) {
    // Выбор действий, если в таблице есть изменения
    if (checked && model->isDirty()) {
        switch (QMessageBox::question(this, tr("Подтвердите действие"), tr("Сохранить изменённые данные?"))) {
        case QMessageBox::Yes:
            on_buttonSubmitChanges_clicked();
            break;
        case QMessageBox::No:
            on_buttonRevertChanges_clicked();
            break;
        default:
            break;
        }
    }
    
    if (checked) {
        ui->buttonReadOnly->setIcon(QIcon(":/Resources/Locked.png"));
        ui->buttonDelRow->setEnabled(false);
    }
    else ui->buttonReadOnly->setIcon(QIcon(":/Resources/Unlocked.png"));
    ui->buttonAddRow->setEnabled(!checked);
    
    setTableInModel(currentTable);
}

/**
 * @brief on_buttonRefresh_clicked
 * Обновление информации о базе данных
 */
void managerDBForm::on_buttonRefresh_clicked() {
    // Выбор действий, если в таблице есть изменения
    if (model->isDirty()) {
        switch (QMessageBox::question(this, tr("Подтвердите действие"), tr("Сохранить изменённые данные?"),
                                      QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel)) {
        case QMessageBox::Yes:
            on_buttonSubmitChanges_clicked();
            break;
        case QMessageBox::No:
            on_buttonRevertChanges_clicked();
            break;
        case QMessageBox::Cancel:
            return;
            break;
        default:
            return;
            break;
        }
    }
    
    // Сброс настроек элементов до стандартных в случае неуспешного подключения к БД
    if (!connectDB()) {
        ui->labelDBName->setText("Database name");
        resizeUIElementByContent(*ui->labelDBName, ui->labelDBName->text());
        
        ui->comboBoxTables->setEnabled(false);
        
        ui->labelTableName->setText("Table name");
        resizeUIElementByContent(*ui->labelTableName, ui->labelTableName->text());
        
        ui->buttonReadOnly->setChecked(true);
        ui->buttonSQLQuery->setEnabled(false);
        ui->buttonInfoCard->setEnabled(false);
        
        ui->tableView->reset();
    }
}

/**
 * @brief on_buttonAddRow_clicked
 * Добавление строки в таблицу
 */
void managerDBForm::on_buttonAddRow_clicked() {
   QSqlQuery query(db);
   query.prepare("INSERT INTO " + currentTable + " DEFAULT VALUES;");
   // query.bindValue(":currentTable", currentTable);
   
   if (!query.exec())
       QMessageBox::critical(this, tr("Запрос не выполнен"), tr("Не удалось создать строку!"));
   else setTableInModel(currentTable);
}

void managerDBForm::selectedRowChanged() {
    if (!ui->buttonReadOnly->isChecked() && !ui->tableView->selectionModel()->selectedRows().isEmpty())
        ui->buttonDelRow->setEnabled(true);
    else ui->buttonDelRow->setEnabled(false);
}

/**
 * @brief on_buttonDelRow_clicked
 * Удаление выделенной строки из таблицы
 */
void managerDBForm::on_buttonDelRow_clicked() {
    if (QMessageBox::question(this, tr("Подтвердите действие"), tr("Вы точно хотите удалить выбранную строку?")) == QMessageBox::No)
        return;
    
    int rowIndex = ui->tableView->selectionModel()->selectedRows().first().row() + 1;
    int rowCount = ui->tableView->model()->rowCount();
    
    QSqlQuery query(db);
    // Удаление строки
    query.prepare("DELETE FROM " + currentTable + " WHERE id = :rowIndex;");
    // query.bindValue(":currentTable", currentTable);
    query.bindValue(":rowIndex", QString::number(rowIndex));
    
    if (!query.exec())
        QMessageBox::critical(this, tr("Запрос не выполнен"), tr("Не удалось удалить строку!"));
    else /* if (rowIndex == rowCount) */ {
        // Сброс счётчика id
        // query.prepare("ALTER SEQUENCE " + currentTable + "_id_seq RESTART WITH :rowCount;");
        // query.bindValue(":currentTable", currentTable);
        // query.bindValue(":rowCount", QString::number(rowCount));
        
        // if (!query.exec())
        //    QMessageBox::critical(this, tr("Запрос не выполнен"), tr("Не удалось убавить счётчик!"));
        
        setTableInModel(currentTable);
    }
}

void managerDBForm::tableDataChanged() {
    ui->buttonSubmitChanges->setEnabled(true);
    ui->buttonRevertChanges->setEnabled(true);
}

/**
 * @brief on_buttonSubmitChanges_clicked
 * Сохранение изменений в таблице
 */
void managerDBForm::on_buttonSubmitChanges_clicked() {
    if (QMessageBox::question(this, tr("Подтвердите действие"), tr("Вы точно хотите применить изменения?")) == QMessageBox::Yes) {
        if (model->submitAll()) {
            ui->buttonSubmitChanges->setEnabled(false);
            ui->buttonRevertChanges->setEnabled(false);
        }
        else QMessageBox::critical(this, tr("Запрос не выполнен"), tr("Не удалось применить изменения!"));
    }
}

/**
 * @brief on_buttonRevertChanges_clicked
 * Отмена изменений в таблице
 */
void managerDBForm::on_buttonRevertChanges_clicked() {
    if (QMessageBox::question(this, tr("Подтвердите действие"), tr("Вы точно хотите отменить изменения?")) == QMessageBox::Yes) {
        model->revertAll();
        ui->buttonSubmitChanges->setEnabled(false);
        ui->buttonRevertChanges->setEnabled(false);
        setTableInModel(currentTable);
    }
}

void managerDBForm::on_buttonSQLQuery_clicked() {
    querydlg->show();
}