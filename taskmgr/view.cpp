#include "view.h"
#include "ui_view.h"

view::view(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::view)
{
    ui->setupUi(this);

}

///----------------------------------------------------------------
///        Fill checkboxes
/// ---------------------------------------------------------------
void view::fill_checkboxes(QStringList features)
{
    QCheckBox *checkbox;
    QVBoxLayout *vbox = new QVBoxLayout;
    this->features=features;
    foreach (QString feature, features)
    {
        checkbox= new QCheckBox(tr((const char *)((QByteArray)(feature.toLatin1()).data())));
        checkboxes.append(checkbox);
        checkbox->setChecked(true);
        vbox->addWidget(checkbox);
    }

    vbox->addStretch(1);
    ui->groupBox->setLayout(vbox);

}


///----------------------------------------------------------------
///        Get checked
/// ---------------------------------------------------------------
QStringList view::get_checked()
{
    QStringList active_puncts;
    int i=0;
    foreach (QCheckBox *checkbox, checkboxes)
    {
        if(checkbox->isChecked())
        {
            active_puncts.append(features[i]);
        }
        i++;
    }

    return active_puncts;

}

///----------------------------------------------------------------
///        Destructor
/// ---------------------------------------------------------------
view::~view()
{
    foreach (QCheckBox *checkbox, checkboxes)
    {
        delete checkbox;
    }
    checkboxes.clear();
    delete ui;
}




void view::on_pushButton_clicked()
{
    this->close();
}
