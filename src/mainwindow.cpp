#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mPlayer = new CGLPlayer(ui->frmGL);
    mPlayer->resize(ui->frmGL->size());
}

MainWindow::~MainWindow()
{
    delete mPlayer;
    delete ui;
}
