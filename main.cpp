#include "qpathevaluationdlg.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QPathEvaluationDlg w;
    w.showMaximized();

    return app.exec();
}
