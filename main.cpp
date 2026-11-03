#include <QtWidgets/QApplication>
#include "GUIPanel.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GUIPanel w;
    w.show();
    
    return a.exec();
}
