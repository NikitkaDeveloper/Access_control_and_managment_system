#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QResizeEvent>
#include <QAbstractItemView>

#include "format.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class WidgetComment;
}
QT_END_NAMESPACE

class WidgetComment : public QWidget {
    Q_OBJECT

public:
    WidgetComment(QWidget *parent = nullptr);
    ~WidgetComment();
    
private slots:
    void on_textEdit_cursorPositionChanged();
    
    void on_buttonBold_toggled(bool checked);
    void on_buttonItalic_toggled(bool checked);
    void on_buttonLine_toggled(bool checked);
    void on_comboLine_currentIndexChanged(int index);
    
    void on_buttonColor_toggled(bool checked);
    void on_comboColor_currentIndexChanged(int index);
    
    void on_fontComboBox_currentFontChanged(const QFont &font);
    void on_linePointSize_editingFinished();
    void on_comboPointSize_currentIndexChanged(int index);
    
    void on_buttonAlignLeft_clicked();
    void on_buttonAlignCenter_clicked();
    void on_buttonAlignRight_clicked();
    
    void on_textEdit_undoAvailable(bool b);
    void on_buttonUndo_clicked();
    
    void on_textEdit_redoAvailable(bool b);
    void on_buttonRedo_clicked();
    
    void on_buttonPaste_clicked();
    void on_buttonOpenFile_clicked();
    
private:
    void setButtonLineText(const int& index);
    int maximumItemWidth(const QComboBox& comboBox);
    void setButtonAlignChecked();
    void fillComboColorItems();
    
    virtual void resizeEvent(QResizeEvent *event) override;
    
    Ui::WidgetComment *ui;
    Format* f;
    QTextCharFormat chrformat;
};
#endif // WIDGET_H