#ifndef OPENTABLEDIALOG_H
#define OPENTABLEDIALOG_H

#include <QDialog>

namespace Ui {
class OpenTableDialog;
}

class OpenTableDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OpenTableDialog(QWidget *parent = nullptr);
    ~OpenTableDialog();

signals:
    void returnValue(const QString &data);

private slots:
    void on_numBtn_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::OpenTableDialog *ui;
};

#endif // OPENTABLEDIALOG_H
