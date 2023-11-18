#include "mainwindow.h"
#include "./endingdialog.h"
#include "./generator.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_4_clicked()
{
    this->ui->lineEdit_5->clear();
    this->ui->textEdit_2->clear();
    this->ui->tableWidget->clear();
    this->ui->treeWidget->clear();
    this->ui->textBrowser->clear();
    this->ui->comboBox_2->clear();
    this->clear_insert_inputs();
}

void MainWindow::reload_ends()
{
    QStringList end_list;
    for (int i = 0; i < this->ui->tableWidget->rowCount(); i++) {
        end_list.push_back(this->ui->tableWidget->item(i, 0)->text());
    }
    this->ui->comboBox_2->clear();
    this->ui->comboBox_2->addItems(end_list);
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    this->ui->stackedWidget->setCurrentIndex(index);
}

void MainWindow::clear_insert_inputs()
{
    this->ui->textEdit->clear();
    this->ui->lineEdit->clear();
    this->ui->lineEdit_2->clear();
    this->ui->lineEdit_4->clear();
}



void MainWindow::on_pushButton_clicked()
{
    QTreeWidgetItem* widget_item = new QTreeWidgetItem();
    widget_item->setText(0, NODE_TYPES[this->ui->comboBox->currentIndex()]);
    if (this->ui->comboBox->currentIndex() == 0) {
        widget_item->setText(1, this->ui->textEdit->toPlainText());
    } else if (this->ui->comboBox->currentIndex() == 4) {
        widget_item->setText(1, this->ui->comboBox_2->currentText());
    } else {
        const std::vector<QLineEdit*> input_widget_list = {
            this->ui->lineEdit,
            this->ui->lineEdit_4,
            this->ui->lineEdit_2
        };
        widget_item->setText(1, input_widget_list[this->ui->comboBox->currentIndex() - 1]->text());
    }

    if (this->ui->treeWidget->selectedItems().isEmpty()) {
        this->ui->treeWidget->addTopLevelItem(widget_item);
        this->on_treeWidget_changed();
        return;
    }
    QTreeWidgetItem* selected_item = this->ui->treeWidget->selectedItems().toVector().toStdVector()[0];
    if (this->ui->comboBox->currentIndex() == 1 || selected_item->text(0) == "选项" || selected_item->text(0) == "判定") {
        selected_item->addChild(widget_item);
        selected_item->setExpanded(true);
    } else if (selected_item->parent() == nullptr) {
        this->ui->treeWidget->addTopLevelItem(widget_item);
    } else {
        selected_item->parent()->addChild(widget_item);
        selected_item->parent()->setExpanded(true);
    }
    this->on_treeWidget_changed();
}

void MainWindow::on_treeWidget_changed()
{
    this->clear_insert_inputs();
    Generator *generator = new Generator(
        this->ui->lineEdit_5->text().toStdString(),
        this->ui->textEdit_2->toPlainText().toStdString(),
        this->ui->tableWidget
    );
    generator->generate(this->ui->treeWidget);
    this->ui->textBrowser->setText(QString::fromStdString(generator->get_code()));
    delete generator;
}

const std::vector<QString> NODE_TYPES = {
    "消息",
    "选项",
    "代码",
    "判定",
    "结束"
};


void MainWindow::on_toolButton_clicked()
{
    QTreeWidgetItem* selected_item = this->ui->treeWidget->selectedItems().toVector().toStdVector()[0];
    if (selected_item == nullptr) {
        this->ui->toolButton->setEnabled(false);
        return;
    }
    if (selected_item->parent() == nullptr) {
        this->ui->treeWidget->takeTopLevelItem(this->ui->treeWidget->indexOfTopLevelItem(selected_item));
    } else {
        selected_item->parent()->removeChild(selected_item);
    }
    delete selected_item;
}


void MainWindow::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    this->ui->toolButton->setEnabled(true);
}


void MainWindow::on_pushButton_5_clicked()
{
    this->ui->tabWidget->setCurrentIndex(2);
}


void MainWindow::on_pushButton_6_clicked()
{
    EndingDialog ending_dialog;
    ending_dialog.show();
    ending_dialog.exec();
    End *end = ending_dialog.get_end();
    if (end == nullptr) {
        return;
    }
    int latest_row = this->ui->tableWidget->rowCount();
    this->ui->tableWidget->setRowCount(latest_row + 1);
    this->ui->tableWidget->setItem(latest_row, 0, new QTableWidgetItem(end->id));
    this->ui->tableWidget->setItem(latest_row, 1, new QTableWidgetItem(end->description));
    delete end;
    this->reload_ends();
}


void MainWindow::on_toolButton_2_clicked()
{
    int current_row = this->ui->tableWidget->currentRow();
    if (current_row == -1) {
        this->ui->toolButton_2->setEnabled(false);
        return;
    }
    this->ui->tableWidget->removeRow(current_row);
}


void MainWindow::on_tableWidget_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    this->ui->toolButton_2->setEnabled(true);
}

