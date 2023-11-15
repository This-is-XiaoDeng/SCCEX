#include "mainwindow.h"
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
    this->ui->treeWidget->clear();
    this->ui->textBrowser->clear();
    this->clear_insert_inputs();
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
    this->ui->lineEdit_3->clear();
    this->ui->lineEdit_4->clear();
}



void MainWindow::on_pushButton_clicked()
{
    QTreeWidgetItem* widget_item = new QTreeWidgetItem();
    widget_item->setText(0, NODE_TYPES[this->ui->comboBox->currentIndex()]);
    if (this->ui->comboBox->currentIndex() == 0) {
        widget_item->setText(1, this->ui->textEdit->toPlainText());
    } else {
        const std::vector<QLineEdit*> input_widget_list = {
            this->ui->lineEdit,
            this->ui->lineEdit_4,
            this->ui->lineEdit_2,
            this->ui->lineEdit_3
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
        this->ui->textEdit_2->toPlainText().toStdString()
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

