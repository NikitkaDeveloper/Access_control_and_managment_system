#ifndef FORMAT_H
#define FORMAT_H

#include <QApplication>
#include <QClipboard>
#include <QTextEdit>
#include <QComboBox>
#include <QFileDialog>
#include <QTextStream>
#include <QFont>

class Format {
public:
    Format(QTextEdit* txtEdit);
    
    void checkFormat(QTextCharFormat& chrformat);
    QTextCharFormat getFormat();
    void setFormat(const QTextCharFormat& chrformat);
    
    void setBold(QTextCharFormat& chrformat, const bool& checked);
    void setItalic(QTextCharFormat& chrformat, const bool& checked);
    void setLine(QTextCharFormat& chrformat, const bool& checked, const int& index);
    void setColor(QTextCharFormat& chrformat, const bool& checked, const QBrush& brush);
    
    void setFontFamily(QTextCharFormat& chrformat, const QFont& font);
    void setFontPointSize(QTextCharFormat& chrformat, const int& ps);
    
    void setAlignment(const Qt::Alignment& align);
    
    void undo();
    void redo();
    
    bool pasteTextFromBuffer();
    bool setTextFromFile(const QString& filePath);
    
    enum lineindex {
        UNDERLINE = 0,
        STRIKEOUT,
        OVERLINE
    };
    
private:
    const bool FAIL = 0;
    const bool SUCCESS = 1;
    
    QTextEdit* m_txtEdit = nullptr;
    QClipboard* cpb = QApplication::clipboard();
};

#endif // FORMAT_H