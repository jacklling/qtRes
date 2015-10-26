#include "openfile.h"

openFile::openFile(QWidget *parent): QLineEdit(parent)
{
    setAcceptDrops(true);
    setEnabled(false);
    //    this->setText("12345");
}

