#ifndef DIALOG_PRIORY_H
#define DIALOG_PRIORY_H

#include <QDialog>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QVector>

namespace Ui {
class dialog_priory;
}

class dialog_priory : public QDialog
{
    Q_OBJECT

public:
    explicit dialog_priory(QWidget *parent = 0);
    void fill_radiobuttons(QString Priory);
    int get_checked();
    ~dialog_priory();

private slots:
    void on_pushButton_clicked();

private:
    Ui::dialog_priory *ui;
    QStringList priory_list;
    QVector<QRadioButton*> radiobuttons;
    void set_current_priory(QString cur_priory);

};

#endif // DIALOG_PRIORY_H
