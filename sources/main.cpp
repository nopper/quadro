#include <QApplication>
#include "quadro.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Quadro *mw =new Quadro();
    mw->show();

    return app.exec();
}
