#include "codeviewdialog.h"
#include <QString>
#include "ui_codeviewdialog.h"

CodeViewDialog::CodeViewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CodeViewDialog)
{
    ui->setupUi(this);
}

CodeViewDialog::~CodeViewDialog()
{
    delete ui;
}

void CodeViewDialog::set_code(std::string code)
{
    this->ui->textBrowser->setPlainText(QString::fromStdString(code));
}
