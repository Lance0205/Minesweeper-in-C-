/*
Author: Lance Cheong Youne
Description: Main file that runs the Qapplication and mainwindow
Date: February 6, 2024
*/
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MainWindow w;
    w.show();
    
    return a.exec();
}