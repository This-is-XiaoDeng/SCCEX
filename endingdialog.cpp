#include "endingdialog.h"
#include "ui_endingdialog.h"

EndingDialog::EndingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EndingDialog)
{
    ui->setupUi(this);
    this->is_confirmed = false;
}

EndingDialog::~EndingDialog()
{
    delete ui;
}

End *EndingDialog::get_end()
{
    if (!this->is_confirmed) {
        return nullptr;
    }
    End *end = new End;
    end->id = this->ui->lineEdit->text();
    end->description = this->ui->textEdit->toPlainText();
    return end;
}

void EndingDialog::on_buttonBox_accepted()
{
    this->is_confirmed = true;
    this->close();
}


void EndingDialog::on_buttonBox_rejected()
{
    this->is_confirmed = false;
    this->close();
}

