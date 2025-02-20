#include "widget.h"
#include "./ui_widget.h"

WidgetComment::WidgetComment(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WidgetComment)
{
    ui->setupUi(this);
    
    setButtonLineText(Format::UNDERLINE);
    ui->fontComboBox->setCurrentFont(ui->textEdit->font());
    ui->linePointSize->setText(QString::number(ui->textEdit->font().pointSize()));
    
    ui->comboPointSize->setMaxVisibleItems(ui->comboPointSize->count());
    ui->comboPointSize->view()->setMinimumWidth(maximumItemWidth(*ui->comboPointSize));
    
    ui->comboLine->view()->setMinimumWidth(maximumItemWidth(*ui->comboLine));
    ui->comboColor->view()->setMinimumWidth(maximumItemWidth(*ui->comboColor));
    ui->comboColor->view()->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
    fillComboColorItems();
    
    ui->textEdit->setFocus();
    
    f = new Format(ui->textEdit);
    chrformat = f->getFormat();
}

WidgetComment::~WidgetComment() {
    delete f;
    delete ui;
}

void WidgetComment::resizeEvent(QResizeEvent *event) {
    int w = event->size().width();
    int h = event->size().height();
    ui->textEdit->resize(w - qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent) - 9, h - 98);
}

/**
 * @brief maximumItemWidth 
 * Определение максимальной длины элемента ComboBox
 * 
 * @param comboBox Используемый ComboBox
 * @return max_width Максимальная длина элемента
 */
int WidgetComment::maximumItemWidth(const QComboBox& comboBox) {
    int max_width = 0;
    QFontMetrics fm(comboBox.font());
    for(int i = 0; i < comboBox.count(); ++i) {
        int width = fm.horizontalAdvance(comboBox.itemText(i)) + 30;
        if(width > max_width)
            max_width = width;
    }
    return max_width;
}

/**
 * @brief fillComboColorItems
 * Заполнение элементов comboColor разными цветами
 */
void WidgetComment::fillComboColorItems() {
    for (int index = 0, colorid = 2; index < 17; ++index, ++colorid) {
        if (index != Qt::GlobalColor::transparent)
            ui->comboColor->setItemData(index, QColor(QColor::colorNames().at(colorid)), Qt::BackgroundRole);
        else --index;
    }
}

void WidgetComment::setButtonAlignChecked() {
    ui->buttonAlignLeft->setChecked(false);
    ui->buttonAlignCenter->setChecked(false);
    ui->buttonAlignRight->setChecked(false);
    
    switch (ui->textEdit->alignment()) {
    case Qt::AlignLeft:
        ui->buttonAlignLeft->setChecked(true);
        break;
    case Qt::AlignCenter:
        ui->buttonAlignCenter->setChecked(true);
        break;
    case Qt::AlignRight:
        ui->buttonAlignRight->setChecked(true);
        break;
    default:
        break;
    }
}

void WidgetComment::on_textEdit_cursorPositionChanged() {
    if (!ui->textEdit->textCursor().selectedText().isEmpty())
        return;
    
    f->checkFormat(chrformat);
    
    // Изменение формата текста кнопки "черта" в зависимости от нового формата текста
    ui->buttonBold->setChecked(chrformat.font().bold());
    ui->buttonItalic->setChecked(chrformat.font().italic());
    if (chrformat.font().underline()) {
        ui->buttonLine->setChecked(true);
        setButtonLineText(f->UNDERLINE);
        ui->comboLine->setCurrentIndex(f->UNDERLINE);
    }
    else if (chrformat.font().strikeOut()) {
        ui->buttonLine->setChecked(true);
        setButtonLineText(f->STRIKEOUT);
        ui->comboLine->setCurrentIndex(f->STRIKEOUT);
    }
    else if (chrformat.font().overline()) {
        ui->buttonLine->setChecked(true);
        setButtonLineText(f->OVERLINE);
        ui->comboLine->setCurrentIndex(f->OVERLINE);
    }
    else ui->buttonLine->setChecked(false);
    /////////////////////////////////////////////////////////////
    
    ui->fontComboBox->setCurrentFont(chrformat.font());
    ui->comboPointSize->setCurrentText(QString::number(chrformat.font().pointSize()));
    
    // Изменение формата текста кнопки "цвет" в зависимости от нового формата текста
    QBrush brush = chrformat.foreground();
    if (brush == Qt::NoBrush) brush = Qt::black;
    QPalette pal = ui->buttonColor->palette();
    pal.setBrush(QPalette::ColorRole::ButtonText, brush);
    ui->buttonColor->setPalette(pal);
    ui->buttonColor->setChecked(true);
    if (brush == Qt::black) ui->buttonColor->setChecked(false);
    setButtonAlignChecked();
}

void WidgetComment::on_buttonBold_toggled(bool checked) {
    f->setBold(chrformat, checked);
}

void WidgetComment::on_buttonItalic_toggled(bool checked) {
    f->setItalic(chrformat, checked);
}

void WidgetComment::on_buttonLine_toggled(bool checked) {
    f->setLine(chrformat, checked, ui->comboLine->currentIndex());
}

/**
 * @brief setButtonLineText
 * Изменение формата текста кнопки "черта"
 * после выбора нового элемента comboLine
 * 
 * @param index Индекс выбранного элемента comboLine
 */
void WidgetComment::setButtonLineText(const int& index) {
    QFont font = ui->buttonLine->font();
    font.setUnderline(false);
    font.setStrikeOut(false);
    font.setOverline(false);
    switch (index) {
    case f->UNDERLINE:
        font.setUnderline(true);
        break;
    case f->STRIKEOUT:
        font.setStrikeOut(true);
        break;
    case f->OVERLINE:
        font.setOverline(true);
        break;
    default:
        break;
    }
    ui->buttonLine->setFont(font);
}

void WidgetComment::on_comboLine_currentIndexChanged(int index) {
    ui->buttonLine->setChecked(true);
    setButtonLineText(index);
    f->setLine(chrformat, ui->buttonLine->isChecked(), index);
}

void WidgetComment::on_buttonColor_toggled(bool checked) {
    QBrush brush = ui->buttonColor->palette().buttonText();
    if (checked && brush == Qt::black) { ui->buttonColor->setChecked(false); return; }
    f->setColor(chrformat, checked, brush);
}

void WidgetComment::on_comboColor_currentIndexChanged(int index) {
    QVariant var = ui->comboColor->itemData(index, Qt::BackgroundRole);
    QBrush brush = var.value<QBrush>();
    
    QPalette pal;
    pal.setBrush(QPalette::ColorRole::ButtonText, brush);
    ui->buttonColor->setPalette(pal);
    
    ui->buttonColor->setChecked(true);
    if (brush == Qt::black) ui->buttonColor->setChecked(false);
    f->setColor(chrformat, ui->buttonColor->isChecked(), brush);
}

void WidgetComment::on_fontComboBox_currentFontChanged(const QFont &font) {
    f->setFontFamily(chrformat, font);
}

void WidgetComment::on_linePointSize_editingFinished() {
    ui->comboPointSize->setCurrentIndex(ui->comboPointSize->findText(ui->linePointSize->text()));
    f->setFontPointSize(chrformat, ui->linePointSize->text().toInt());
}

void WidgetComment::on_comboPointSize_currentIndexChanged(int index) {
    ui->linePointSize->setText(ui->comboPointSize->currentText());
    f->setFontPointSize(chrformat, ui->comboPointSize->currentText().toInt());
}

void WidgetComment::on_buttonAlignLeft_clicked() {
    f->setAlignment(Qt::AlignLeft);
    setButtonAlignChecked();
}

void WidgetComment::on_buttonAlignCenter_clicked() {
    f->setAlignment(Qt::AlignCenter);
    setButtonAlignChecked();
}

void WidgetComment::on_buttonAlignRight_clicked() {
    f->setAlignment(Qt::AlignRight);
    setButtonAlignChecked();
}

void WidgetComment::on_textEdit_undoAvailable(bool b) {
    ui->buttonUndo->setEnabled(b);
}

void WidgetComment::on_buttonUndo_clicked() {
    f->undo();
}

void WidgetComment::on_textEdit_redoAvailable(bool b) {
    ui->buttonRedo->setEnabled(b);
}

void WidgetComment::on_buttonRedo_clicked() {
    f->redo();
}

void WidgetComment::on_buttonPaste_clicked() {
    f->pasteTextFromBuffer();
}

void WidgetComment::on_buttonOpenFile_clicked() {
    QString filePath = QFileDialog::getOpenFileName(nullptr, tr("Выберите файл"), "", "Text files (*.txt);;All files (*)");
    if (!filePath.isEmpty())
        f->setTextFromFile(filePath);
}