#ifndef ENDINGDIALOG_H
#define ENDINGDIALOG_H

#include <QDialog>

namespace Ui {
class EndingDialog;
}

struct End {
    QString id;
    QString description;
};

class EndingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EndingDialog(QWidget *parent = nullptr);
    End *get_end();
    ~EndingDialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::EndingDialog *ui;
    bool is_confirmed;
};

#endif // ENDINGDIALOG_H
