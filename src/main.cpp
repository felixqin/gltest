#include <stdio.h>
#include <QApplication>
#include <mainwindow.h>

int main(int argc, char* argv[])
{
    printf("hello\n");
    QApplication app(argc, argv);

    MainWindow win;
    win.show();

    app.exec();

    return 0;
}
