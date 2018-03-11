#include "dialog_priory.h"
#include "ui_dialog_priory.h"

/// ---------------------------------------------------------------
///        Constructor
/// ---------------------------------------------------------------
dialog_priory::dialog_priory(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialog_priory)
{
    ui->setupUi(this);

    priory_list<<"Low"<<"Lower-middle"<<"Middle"<<"Upper-middle"<<"Hight"<<"Real time";

    QRadioButton *radiobutton;
    QVBoxLayout *vbox = new QVBoxLayout;

    foreach (QString priory, priory_list)
    {
        radiobutton= new QRadioButton(tr((const char *)((QByteArray)(priory.toLatin1()).data())));
        radiobuttons.append(radiobutton);

        vbox->addWidget(radiobutton);
    }

    vbox->addStretch(1);
    ui->PrioryBox->setLayout(vbox);

}

/// ---------------------------------------------------------------
///        Fill radiobuttons
/// ---------------------------------------------------------------
void dialog_priory::fill_radiobuttons(QString Priory)
{
    set_current_priory(Priory);

}
/// ---------------------------------------------------------------
///        Get checked
/// ---------------------------------------------------------------
int dialog_priory::get_checked()
{
    int i=0;
    foreach (QRadioButton *radibutton, radiobuttons)
    {
        if(radibutton->isChecked())
        {
            return i;
        }
        i++;
    }

}
/// ---------------------------------------------------------------
///        Set current priory
/// ---------------------------------------------------------------
void dialog_priory::set_current_priory(QString cur_priory)
{
    int i=0;
    foreach (QString priory, priory_list)
    {
        radiobuttons[i]->setChecked(false);
        if(cur_priory==priory)
        {
            radiobuttons[i]->setChecked(true);
        }
        i++;
    }
}

/// ---------------------------------------------------------------
///        Destructor
/// ---------------------------------------------------------------
dialog_priory::~dialog_priory()
{
    foreach (QRadioButton *radiobutton, radiobuttons)
    {
        delete radiobutton;
    }
    radiobuttons.clear();
    delete ui;
}

void dialog_priory::on_pushButton_clicked()
{
    this->close();
}
