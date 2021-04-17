#ifndef _MAINFORM_H
#define _MAINFORM_H

#include "ui_MainForm.h"

class MainForm : public QMainWindow {
    Q_OBJECT
public:
    MainForm();
    virtual ~MainForm();
private:
    Ui::MainForm widget;
    
    void search_by_key(const QString &key_text);
    
public slots:
  void clickedSlot_1();    
  void clickedSlot_2();  
  void clickedSlot_3();
  void itemDoubleClicked(QListWidgetItem *item);
  void cellDoubleClicked(int row, int column);
};

#endif /* _MAINFORM_H */
