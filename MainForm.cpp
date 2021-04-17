#include "MainForm.h"

#include <QtCore/QString>
#include <QtWidgets/QMessageBox>
#include <QUrl>
#include <QDesktopServices>

#include <QFile>
#include <QTextStream>

#include "Word.h"
#include "ThesaurusDao.h"
#include "DataStore.h"

MainForm::MainForm() {
    widget.setupUi(this);
    setWindowIcon(QIcon(":/images/application-split.png"));
    // https://evileg.com/en/post/73/
    widget.splitter->setStretchFactor(0, 0);
    widget.splitter->setStretchFactor(1, 1);
    // https://www.qtcentre.org/threads/26520-initial-size-of-widgets-in-a-QSplitter
    QList<int> sizes;
    sizes << 320 << 450;
    widget.splitter->setSizes(sizes);
    //    /* Also, just I want to show you how to choose the color separator. 
    //     * To do this we need to use a class QPallete, for which you choose the background color.
    //     * */
    //    QPalette p;
    //    p.setColor(QPalette::Background, Qt::gray);
    //    /* And sets the palette QSplitter
    //     * */
    //    widget.splitter->setPalette(p);
    QObject::connect(widget.pushButton_1, SIGNAL(clicked()), this, SLOT(clickedSlot_1()));
    QObject::connect(widget.pushButton_2, SIGNAL(clicked()), this, SLOT(clickedSlot_2()));
    QObject::connect(widget.pushButton_3, SIGNAL(clicked()), this, SLOT(clickedSlot_3()));
    QObject::connect(widget.listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(itemDoubleClicked(QListWidgetItem*)));
    // https://stackoverflow.com/questions/11503313/handling-single-click-and-double-click-separately-in-qtablewidget
    QObject::connect(widget.tableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(cellDoubleClicked(int, int)));
    //////////////////////////////////////////
    QStringList headers;
    headers << "Related" << "Notes";
    widget.tableWidget->setColumnCount(2);
    widget.tableWidget->setColumnWidth(0, 240);
//    widget.tableWidget->setColumnWidth(1, 0);
    widget.tableWidget->setShowGrid(true);
    widget.tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    widget.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    widget.tableWidget->setHorizontalHeaderLabels(headers);
    widget.tableWidget->horizontalHeader()->setStretchLastSection(true);
    // http://doc.qt.io/qt-5/resources.html
    widget.pushButton_1->setIcon(QIcon(":/images/133.png"));
    widget.pushButton_2->setIcon(QIcon(":/images/131.png"));
    widget.pushButton_3->setIcon(QIcon(":/images/094.png"));
}

MainForm::~MainForm() {
}

void MainForm::itemDoubleClicked(QListWidgetItem *item) {
    if (item == NULL) {
        QMessageBox::information(this, "Error", "Select an item");
        return;
    }
    // https://stackoverflow.com/questions/15848086/how-to-delete-all-rows-from-qtablewidget
    widget.tableWidget->setRowCount(0);
    int i = 0;
    widget.tableWidget->insertRow(i);
    auto table_item = new QTableWidgetItem("");
    widget.tableWidget->setItem(i, 0, table_item);
    item->setFlags(table_item->flags() ^ Qt::ItemIsEditable);
    widget.tableWidget->setItem(i, 1, new QTableWidgetItem(item->text()));
    widget.tableWidget->setItem(i, 2, new QTableWidgetItem(""));
    CDataStore ds;
    if (!ds.open()) {
        QMessageBox::information(this, "Error", ds.getLastErrors());
        return;
    }
    ThesaurusDao dao(ds);

    struct : CListReader<RelatedWord> {
        QTableWidget *tableWidget;
        int i = 1;

        void read(RelatedWord &w) {
            tableWidget->insertRow(i);
            auto table_item = new QTableWidgetItem(w.RGW_WORD.Value);
            tableWidget->setItem(i, 0, table_item);
            table_item->setFlags(table_item->flags() ^ Qt::ItemIsEditable);
            auto notes = QString("%1 %2").arg(w.RG_PART_OF_SPEECH.Value, w.RGW_NOTE.Value);
            tableWidget->setItem(i, 1, new QTableWidgetItem(notes));
            i++;
        }
    } reader;
    reader.tableWidget = widget.tableWidget;
    QVariant v = item->data(Qt::UserRole);
    int id = v.value<int>();
    CLong ID;
    ID.Value = id;
    int res = dao.getRelatedWords(ID, reader);
    if (res < 0) {
        QMessageBox::information(this, "Error", ds.getLastErrors());
    }
    ds.close();
}

void MainForm::clickedSlot_2() {
    QListWidgetItem *item = widget.listWidget->currentItem();
    itemDoubleClicked(item);
}

void MainForm::clickedSlot_1() {
    QString key_text = widget.lineEdit->text();
    search_by_key(key_text);
}

void MainForm::search_by_key(const QString &key_text) {
    if (key_text.length() == 0) {
        QMessageBox::information(this, "Error", "Specify a search key");
        return;
    }
    CDataStore ds;
    if (!ds.open()) {
        QMessageBox::information(this, "Error", ds.getLastErrors());
        return;
    }
    ThesaurusDao dao(ds);
    CText key;
    key.Value = key_text;
    key.Value.append("%");

    struct : CListReader<Word> {
        QListWidget *listWidget;

        void read(Word &w) {
            QListWidgetItem *item = new QListWidgetItem(w.W_WORD.Value);
            QVariant v;
            v.setValue(w.W_ID.Value);
            item->setData(Qt::UserRole, v);
            listWidget->addItem(item);
        }
    } reader;
    widget.listWidget->clear();
    reader.listWidget = widget.listWidget;
    int res = dao.getWordsByKey(key, reader);
    if (res < 0) {
        QMessageBox::information(this, "Error", ds.getLastErrors());
    }
    ds.close();
}

void MainForm::clickedSlot_3() {
    int row = widget.tableWidget->currentRow();
    if (row < 0) {
        return;
    }
    cellDoubleClicked(row, 0);
}

void MainForm::cellDoubleClicked(int row, int /*column*/) {
    QString link;
    QString fn = QCoreApplication::applicationDirPath() + "/url.txt";
    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, "Error", fn.append("\r\n").append(file.errorString()));
    } else {
        QTextStream in(&file);
        link = in.readLine();
        QString key_text = widget.tableWidget->item(row, 1)->text();
        key_text.replace(" ", "+");
        link.append(key_text);
        QDesktopServices::openUrl(QUrl(link));
    }
}
