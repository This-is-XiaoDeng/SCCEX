#include "mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include "contchatfm.h"
#include "endingdialog.h"
#include "generator.h"
#include "ui_mainwindow.h"

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
    // this->ui->tableWidget->clear();
    // this->ui->treeWidget->clear();
    this->ui->textBrowser->clear();
    this->ui->comboBox_2->clear();
    this->clear_insert_inputs();

    for (int i = 0; i < this->ui->tableWidget->rowCount(); ++i) {
        for (int j = 0; j < this->ui->tableWidget->columnCount(); ++j) {
            this->ui->tableWidget->setItem(i, j, nullptr);
        }
    }
    while (this->ui->treeWidget->topLevelItemCount() > 0) {
        QTreeWidgetItem *item = this->ui->treeWidget->takeTopLevelItem(0);
        delete item;
    }
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
    QTreeWidgetItem* selected_item = this->ui->treeWidget->selectedItems()[0];
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
    QTreeWidgetItem* selected_item = this->ui->treeWidget->selectedItems()[0];
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


void MainWindow::on_pushButton_2_clicked()
{
    QString file_path = QFileDialog::getSaveFileName(this, "保存项目 - SCCEX", QDir::homePath(), "Json Files (*.json);;All Files (*)");
    if (file_path.isNull()) {
        return;
    }
    ContChatProject project;
    project.name = this->ui->lineEdit_5->text().toStdString();
    project.description = this->ui->textEdit_2->toPlainText().toStdString();
    project.ends = get_ends(this->ui->tableWidget);
    project.story_tree = this->ui->treeWidget;
    while (!save_project(project, file_path.toStdString())) {
        if (QMessageBox::critical(this, "错误 - SCCEX", "保存项目失败：打开文件时出现错误！") == QMessageBox::Cancel) {
            break;
        }
    }
}


void MainWindow::on_pushButton_3_clicked()
{
    QString file_path = QFileDialog::getOpenFileName(this, "打开项目 - SCCEX", QDir::homePath(), "Json Files (*.json);;All Files (*)");
    if (file_path.isNull()) {
        return;
    }
    ContChatProjectWithVectorTree project;
    while (true) {
        project = load_project(file_path.toStdString());
        if (project.is_success) {
            break;
        }
        if (QMessageBox::critical(this, "错误 - SCCEX", "打开项目失败：打开文件时出现错误！") == QMessageBox::Cancel) {
            return;
        }
    }
    this->ui->pushButton_4->click();
    this->ui->lineEdit_5->setText(QString::fromStdString(project.name));
    this->ui->textEdit_2->setPlainText(QString::fromStdString(project.description));
    int row=0;
    for (const auto &end : project.ends) {
        this->ui->tableWidget->setItem(++row, 0, new QTableWidgetItem(QString::fromStdString(end.id)));
        this->ui->tableWidget->setItem(++row, 1, new QTableWidgetItem(QString::fromStdString(end.description)));
    }
    this->reload_ends();
    for (const auto &item : project.story_tree) {
        this->ui->treeWidget->addTopLevelItem(item);
    }
    this->on_treeWidget_changed();



}

