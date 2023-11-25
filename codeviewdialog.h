#ifndef CODEVIEWDIALOG_H
#define CODEVIEWDIALOG_H

#include <QDialog>
#include <iostream>

namespace Ui {
class CodeViewDialog;
}

class CodeViewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CodeViewDialog(QWidget *parent = nullptr);
    ~CodeViewDialog();

    void set_code(std::string code);

private:
    Ui::CodeViewDialog *ui;
};

#endif // CODEVIEWDIALOG_H
