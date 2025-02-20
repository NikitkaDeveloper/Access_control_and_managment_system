#include "format.h"

Format::Format(QTextEdit* txtEdit) {
    m_txtEdit = txtEdit;
}

void Format::checkFormat(QTextCharFormat& chrformat) {
    chrformat = getFormat();
    setFormat(chrformat);
}

QTextCharFormat Format::getFormat() {
    return m_txtEdit->currentCharFormat();
}

void Format::setFormat(const QTextCharFormat& chrformat) {
    m_txtEdit->setCurrentCharFormat(chrformat);
    m_txtEdit->setFocus();
}

void Format::setBold(QTextCharFormat& chrformat, const bool& checked) {
    if (checked) chrformat.setFontWeight(QFont::Bold);
    else chrformat.setFontWeight(QFont::Normal);
    setFormat(chrformat);
}

void Format::setItalic(QTextCharFormat& chrformat, const bool& checked) {
    chrformat.setFontItalic(checked);
    setFormat(chrformat);
}

void Format::setLine(QTextCharFormat& chrformat, const bool& checked, const int& index) {
    chrformat.setFontUnderline(false);
    chrformat.setFontStrikeOut(false);
    chrformat.setFontOverline(false);
    switch (index) {
    case UNDERLINE:
        chrformat.setFontUnderline(checked);
        break;
    case STRIKEOUT:
        chrformat.setFontStrikeOut(checked);
        break;
    case OVERLINE:
        chrformat.setFontOverline(checked);
        break;
    default:
        break;
    }
    setFormat(chrformat);
}

void Format::setColor(QTextCharFormat& chrformat, const bool& checked, const QBrush& brush) {
    if (checked) chrformat.setForeground(brush);
    else chrformat.setForeground(Qt::black);
    setFormat(chrformat);
}

void Format::setFontFamily(QTextCharFormat& chrformat, const QFont& font) {
    chrformat.setFontFamily(font.family());
    setFormat(chrformat);
}

void Format::setFontPointSize(QTextCharFormat& chrformat, const int& ps) {
    chrformat.setFontPointSize(ps);
    setFormat(chrformat);
}

void Format::setAlignment(const Qt::Alignment& align) {
    m_txtEdit->setAlignment(align);
    m_txtEdit->setFocus();
}

void Format::undo() {
    m_txtEdit->undo();
    m_txtEdit->setFocus();
}

void Format::redo() {
    m_txtEdit->redo();
    m_txtEdit->setFocus();
}

bool Format::pasteTextFromBuffer() {
    m_txtEdit->setFocus();
    if (!cpb->text().isEmpty())
        m_txtEdit->insertPlainText(cpb->text());
    else return FAIL;
    return SUCCESS;
}

bool Format::setTextFromFile(const QString& filePath) {
    m_txtEdit->setFocus();
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        QString str = stream.readAll();
        file.close();
        m_txtEdit->setText(str);
    }
    else return FAIL;
    return SUCCESS;
}