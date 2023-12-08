#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>
#include <vector>
#include <QWidget>
#include "ccproject.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct CachedTreeItem {
    int index;
    QTreeWidgetItem *parent;
    QTreeWidgetItem *item;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_comboBox_currentIndexChanged(int index);

    void on_pushButton_clicked();


    void on_toolButton_clicked();

    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_pushButton_5_clicked();


    void on_pushButton_6_clicked();

    void on_toolButton_2_clicked();

    void on_tableWidget_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);


    void on_action_triggered();

    void on_action_2_triggered();

    void on_action_3_triggered();

    void on_comboBox_3_currentIndexChanged(int index);

    void on_toolButton_3_clicked();


    void on_lineEdit_5_textChanged(const QString &arg1);


    void on_action_8_triggered();

    void on_action_SCCEX_2_triggered();

    void on_action_4_triggered();

    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_treeWidget_itemChanged(QTreeWidgetItem *item, int column);

    void on_tableWidget_itemChanged(QTableWidgetItem *item);



    // void on_treeWidget_itemPressed(QTreeWidgetItem *item, int column);

    void on_treeWidget_itemSelectionChanged();

    // void on_treeWidget_objectNameChanged(const QString &objectName);

    void on_toolButton_4_clicked();

    // void on_action_6_triggered();

private:
    Ui::MainWindow *ui;
    void clear_insert_inputs();
    inline void on_treeWidget_changed();
    void reload_ends();
    void reload_project();
    void reload_event_list();
    void clear_treeWidget();
    QTreeWidgetItem* get_tree_item(CcStoryNode node);
    void update_currect_event();
    void clear_tableWidget();
    void update_editor();
    bool is_end(QString end);
    inline bool is_valid_tree_item_edit(QTreeWidgetItem *item, int column);
    // void _check_current_event(QTreeWidgetItem *item);
    // void check_current_event();
    inline bool is_valid_node(QTreeWidgetItem *item);
    void check_treeWidget_changes();
    void on_treeItem_moved(QTreeWidgetItem *item);
    void undo_item_movement(QTreeWidgetItem *item);
    void move_item_to_parent_level(QTreeWidgetItem *item);

    CcProject project;
    int current_event;
    // QTimer *project_checking_timer;
    QString tree_widget_original_text;
    CachedTreeItem cached_tree_item;
};



extern const std::vector<QString> NODE_TYPES;
int get_node_type(QString string_type);
std::vector<CcStoryNode> get_story_tree(std::vector<QTreeWidgetItem*> items);
// bool check_treeView_item(QTreeWidgetItem *item);

#endif // MAINWINDOW_H
