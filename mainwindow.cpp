#include "mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include "endingdialog.h"
#include "codeviewdialog.h"
#include <fmt/printf.h>
#include "generator.h"
#include <QTimer>
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->ui->tabWidget->setDisabled(true);
    this->current_event = -1;
    this->cached_tree_item.item = nullptr;

    // this->project_checking_timer = new QTimer(this);
    // this->project_checking_timer->setInterval(3000);
    // QObject::connect(this->project_checking_timer, &QTimer::timeout, this, &MainWindow::check_current_event);
    // this->project_checking_timer->start();

}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_3_triggered()
{
    this->project = CcProject();
    this->reload_project();
}

void MainWindow::reload_event_list()
{
    this->ui->comboBox_3->clear();
    for (const CcEvent &event : this->project.events) {
        this->ui->comboBox_3->addItem(QString::fromStdString(event.name));
    }
}

void MainWindow::reload_project()
{
    this->clear_insert_inputs();
    this->ui->comboBox_3->clear();
    this->ui->tabWidget->setDisabled(true);
    this->clear_tableWidget();
    this->clear_treeWidget();
    this->reload_event_list();
}


void MainWindow::clear_tableWidget() {
    int rowCount = this->ui->tableWidget->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        this->ui->tableWidget->removeRow(0);
    }
}


void MainWindow::clear_treeWidget()
{
    int topLevelItemCount = this->ui->treeWidget->topLevelItemCount();
    for (int i=0;i<topLevelItemCount;i++) {
        delete this->ui->treeWidget->takeTopLevelItem(0);
    }
}

void MainWindow::reload_ends()
{
    this->ui->comboBox_2->clear();
    for (int i = 0; i < this->ui->tableWidget->rowCount(); i++) {
        this->ui->comboBox_2->addItem(this->ui->tableWidget->item(i, 0)->text());
    }
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

inline void MainWindow::on_treeWidget_changed()
{
    this->clear_insert_inputs();
}

const std::vector<QString> NODE_TYPES = {
    "消息",
    "选项",
    "代码",
    "判定",
    "结束"
};

inline bool MainWindow::is_valid_node(QTreeWidgetItem *item)
{
    const int node_type = get_node_type(item->text(0));
    return (node_type == 1 && (item->parent() == nullptr || get_node_type(item->parent()->text(0)) != 0)) ||
           (node_type == 4 && (this->is_end(item->text(1))));
}

// void MainWindow::_check_current_event(QTreeWidgetItem *item)
// {
//     static QBrush default_color = item->foreground(0);
//     const QBrush red(Qt::red);
//     if (this->is_valid_node(item)) {
//         item->setForeground(0, red);
//         item->setForeground(1, red);
//     } else if (item->foreground(0) == red) {
//         item->setForeground(0, default_color);
//         item->setForeground(1, default_color);
//     }
//     for (int i=0; i<item->childCount(); i++) {
//         this->_check_current_event(item->child(i));
//     }
// }

void MainWindow::on_toolButton_clicked()
{
    if (this->ui->treeWidget->selectedItems().isEmpty()) {
        this->ui->toolButton->setEnabled(false);
        return;
    }
    QTreeWidgetItem* selected_item = this->ui->treeWidget->selectedItems()[0];
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
    EndWithQString *end = ending_dialog.get_end();
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


void MainWindow::on_action_2_triggered()
{
    if (this->project.file.empty()) {
        QString file_path = QFileDialog::getSaveFileName(this, "保存项目 - SCCEX", QDir::homePath(), "Json Files (*.json);;All Files (*)");
        if (file_path.isNull()) {
            return;
        }
        this->project.file = file_path.toStdString();
    }
    OperationResult result = this->project.save_project();
    if (!result.is_success) {
        if (QMessageBox::critical(this, "错误 - SCCEX", QString::fromStdString(result.error)) == QMessageBox::Retry) {
            return this->on_action_2_triggered();
        }
    }
}


void MainWindow::on_action_triggered()
{
    QString file_path = QFileDialog::getOpenFileName(this, "打开项目 - SCCEX", QDir::homePath(), "Json Files (*.json);;All Files (*)");
    if (file_path.isNull()) {
        return;
    }
    this->project = CcProject();
    this->project.file = file_path.toStdString();
    OperationResult result = this->project.load_project();
    if (!result.is_success) {
        if (QMessageBox::critical(this, "错误 - SCCEX", QString::fromStdString(result.error)) == QMessageBox::Retry) {
            return this->on_action_triggered();
        }
    }
    this->reload_event_list();
    this->ui->comboBox_3->setCurrentIndex(-1);
}

void MainWindow::on_comboBox_3_currentIndexChanged(int index)
{
    if (this->current_event >= 0) {
        this->update_currect_event();
    }
    if (index < 0) {
        this->ui->tabWidget->setDisabled(true);
        return;
    }
    this->current_event = index;
    this->update_editor();
}

void MainWindow::update_editor()
{
    CcEvent event = this->project.events[this->current_event];
    this->ui->lineEdit_5->setText(QString::fromStdString(event.name));
    this->ui->textEdit_2->setPlainText(QString::fromStdString(event.description));
    int row=0;
    this->clear_tableWidget();
    for (const End &end : event.ends) {
        this->ui->tableWidget->insertRow(row);
        this->ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(end.id)));
        this->ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(end.description)));
        row++;
    }
    this->clear_treeWidget();
    for (const CcStoryNode &node : event.tree) {
        this->ui->treeWidget->addTopLevelItem(this->get_tree_item(node));
    }
    this->reload_ends();
    this->ui->tabWidget->setEnabled(true);
}

QTreeWidgetItem* MainWindow::get_tree_item(CcStoryNode node)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, NODE_TYPES[node.type]);
    item->setText(1, QString::fromStdString(node.content));
    for (const CcStoryNode &child : node.children) {
        item->addChild(this->get_tree_item(child));
    }
    return item;
}

// #define event (this->project.events[this->currect_event])
void MainWindow::update_currect_event()
{
    CcEvent& event = this->project.events[this->current_event];
    event.name = this->ui->lineEdit_5->text().toStdString();
    event.description = this->ui->textEdit_2->toPlainText().toStdString();
    event.ends.clear();
    for (int row=0; row<this->ui->tableWidget->rowCount(); row++) {
        End end;
        end.id = this->ui->tableWidget->item(row, 0)->text().toStdString();
        end.description = this->ui->tableWidget->item(row, 1)->text().toStdString();
        event.ends.push_back(end);
    }
    std::vector<QTreeWidgetItem*> top_level_items;
    for (int i=0; i<this->ui->treeWidget->topLevelItemCount(); i++) {
        top_level_items.push_back(this->ui->treeWidget->topLevelItem(i));
    }
    event.tree = get_story_tree(top_level_items);
    // std::cout << "Event:" << event.description << " | " << this->ui->textEdit_2->toPlainText().toStdString() << std::endl;
}
// #undef event

int get_node_type(QString string_type)
{
    int i=0;
    for (const QString &type : NODE_TYPES) {
        if (type == string_type) {
            return i;
        }
        i++;
    }
    return -1;
}

std::vector<CcStoryNode> get_story_tree(std::vector<QTreeWidgetItem*> items)
{
    std::vector<CcStoryNode> nodes;
    for (const auto &item : items) {
        CcStoryNode node;
        node.type = get_node_type(item->text(0));
        node.content = item->text(1).toStdString();
        std::vector<QTreeWidgetItem*> child_items;
        for (int i=0; i<item->childCount(); i++) {
            child_items.push_back(item->child(i));
        }
        node.children = get_story_tree(child_items);
        nodes.push_back(node);
    }
    return nodes;
}

void MainWindow::on_toolButton_3_clicked()
{
    CcEvent event;
    event.name = "<未命名事件>";
    this->project.events.push_back(event);
    this->reload_event_list();
    this->ui->comboBox_3->setCurrentIndex(this->project.events.size() - 1);
}

void MainWindow::on_lineEdit_5_textChanged(const QString &arg1)
{

    this->project.events[this->current_event].name = arg1.toStdString();
    this->ui->comboBox_3->setItemText(this->current_event, arg1);
}

void MainWindow::on_action_8_triggered()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(
        this,
        "删除确认 - SCCEX",
        "确定要删除当前选中事件吗？",
        QMessageBox::Yes|QMessageBox::No
    );
    if (reply == QMessageBox::Yes) {
        int index_of_event = this->current_event;
        this->current_event = -1;
        std::vector<CcEvent> cached_events;
        int s = this->project.events.size() - 1;
        for (int i=0; i<(s-index_of_event); i++) {
            cached_events.push_back(this->project.events.back());
            this->project.events.pop_back();
        }
        this->project.events.pop_back();
        for (const auto &item : cached_events) {
            this->project.events.push_back(item);
        }
        this->reload_event_list();
        this->ui->comboBox_3->setCurrentIndex(-1);

    }
}

void MainWindow::on_action_SCCEX_2_triggered()
{
    QMessageBox::about(
        this,
        "关于 - SCCEX",
        "Sugar ContChat Editor X\nBy This is XiaoDeng\n\nhttps://github.com/This-is-XiaoDeng/SCCEX\n"
    );
}


void MainWindow::on_action_4_triggered()
{
    this->update_currect_event();
    Generator generator(this->project);
    CodeViewDialog code_view_dialog;
    code_view_dialog.set_code(generator.get_code());
    code_view_dialog.show();
    code_view_dialog.exec();
}


void MainWindow::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if (!(item->flags() & Qt::ItemIsEditable)) {
        item->setFlags(item->flags() | Qt::ItemIsEditable);
    }
    this->tree_widget_original_text = item->text(column);
    this->ui->treeWidget->editItem(item, column);
}

bool MainWindow::is_end(QString end)
{
    for (int i=0; i<this->ui->comboBox_2->count(); i++) {
        if (this->ui->comboBox_2->itemText(i) == end) {
            return true;
        }
    }
    return false;
}

inline bool MainWindow::is_valid_tree_item_edit(QTreeWidgetItem *item, int column)
{
    return (column == 0 && get_node_type(item->text(0)) == -1) ||
           (column == 1 && item->text(0) == "结束" && !(this->is_end(item->text(1))));

}

void MainWindow::on_treeWidget_itemChanged(QTreeWidgetItem *item, int column)
{
    if (this->is_valid_tree_item_edit(item, column)) {
        item->setText(column, this->tree_widget_original_text);
    }
}


void MainWindow::on_tableWidget_itemChanged(QTableWidgetItem *item)
{
    if (item->column() == 0) {
        this->reload_ends();
    }
}

// void MainWindow::on_treeWidget_itemPressed(QTreeWidgetItem *item, int column)
// {
//     std::cout << "nmsl" << std::endl;
//     this->cached_tree_item.parent = item->parent();
//     if (this->cached_tree_item.parent == nullptr) {
//         this->cached_tree_item.index = this->ui->treeWidget->indexOfTopLevelItem(item);
//     } else {
//         this->cached_tree_item.index = this->cached_tree_item.parent->indexOfChild(item);
//     }
//     this->cached_tree_item.item = item;
// }



void MainWindow::check_treeWidget_changes()
{
    if (this->ui->treeWidget->selectedItems().empty()) {
        return;
    }
    QTreeWidgetItem *item = this->cached_tree_item.item;
    QTreeWidgetItem *selected_item = this->ui->treeWidget->selectedItems()[0];
    if (item == nullptr) {
        item = selected_item;
    }
    // if (is_valid_node(item)) {
    //     if (item->parent() == nullptr) {
    //         this->ui->treeWidget->takeTopLevelItem(this->ui->treeWidget->indexOfTopLevelItem(item));
    //     } else {
    //         item->parent()->takeChild(item->parent()->indexOfChild(item));
    //     }
    //     if (this->cached_tree_item.parent == nullptr) {
    //         this->ui->treeWidget->insertTopLevelItem(this->cached_tree_item.index, item);
    //     } else {
    //         this->cached_tree_item.parent->insertChild(this->cached_tree_item.index, item);
    //     }
    // }
    this->on_treeItem_moved(item);
    this->cached_tree_item.parent = selected_item->parent();
    if (selected_item->parent() == nullptr) {
        this->cached_tree_item.index = this->ui->treeWidget->indexOfTopLevelItem(selected_item);
    } else {
        this->cached_tree_item.index = selected_item->parent()->indexOfChild(selected_item);
    }
    this->cached_tree_item.item = selected_item;
}

void MainWindow::undo_item_movement(QTreeWidgetItem *item)
{
    if (item->parent() == nullptr) {
        this->ui->treeWidget->takeTopLevelItem(this->ui->treeWidget->indexOfTopLevelItem(item));
    } else {
        item->parent()->takeChild(item->parent()->indexOfChild(item));
    }
    if (this->cached_tree_item.parent == nullptr) {
        this->ui->treeWidget->insertTopLevelItem(this->cached_tree_item.index, item);
    } else {
        this->cached_tree_item.parent->insertChild(this->cached_tree_item.index, item);
    }
}

void MainWindow::move_item_to_parent_level(QTreeWidgetItem *item) {
    if (item->parent() == nullptr) {
        return;
    }
    QTreeWidgetItem *ancestor = item->parent()->parent();
    item->parent()->takeChild(item->parent()->indexOfChild(item));
    if (ancestor != nullptr) {
        const int index = ancestor->indexOfChild(item->parent());
        ancestor->insertChild(index + 1, item);
    } else {
        const int index = this->ui->treeWidget->indexOfTopLevelItem(item->parent());
        this->ui->treeWidget->insertTopLevelItem(index + 1, item);
    }

}

void MainWindow::on_treeItem_moved(QTreeWidgetItem *item)
{
    const int node_type = get_node_type(item->text(0));
    int parent_type = -1;
    if (item->parent() != nullptr) {
       parent_type = get_node_type(item->parent()->text(0));
    }
    if (node_type == 1 && parent_type != 0) {
        this->undo_item_movement(item);
    } else if (node_type != 1 && (parent_type != 1 || parent_type != 3)) {
        this->move_item_to_parent_level(item);
    }
}

void MainWindow::on_treeWidget_itemSelectionChanged()
{
    QTimer::singleShot(100, this, [=]() {
        this->check_treeWidget_changes();
    });
}

void MainWindow::on_toolButton_4_clicked()
{
    this->ui->tabWidget->setCurrentIndex(2);
}

