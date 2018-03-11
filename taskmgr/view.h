#ifndef VIEW_H
#define VIEW_H

#include <QDialog>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QVector>

namespace Ui {
class view;
}

class view : public QDialog
{
    Q_OBJECT

public:
    explicit view(QWidget *parent = 0);
     void fill_checkboxes(QStringList);
     QStringList get_checked();
    ~view();

private slots:


     void on_pushButton_clicked();

private:
    Ui::view *ui;
    QVector<QCheckBox*> checkboxes;
    QStringList features;
};

#endif // VIEW_H
