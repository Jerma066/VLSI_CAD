#include <QCoreApplication>
#include "MyMST/mymst.h"

int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);
    Q_UNUSED(argc)
    QString path = argv[1];
    MyMST mst(path);
    //C:/Users/Islam/Desktop/body/VLSI_CAD/programming_assigment/Test/FirstTest.xml

    return 0;
    //return a.exec();
}
