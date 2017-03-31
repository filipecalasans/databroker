#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "../ModuleExample/communication/communication.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_resetButton_clicked();

    void on_readyButton_clicked();

    void on_startButton_clicked();

    void on_pauseButton_clicked();

    void on_resumeButton_clicked();

    void on_disconnectButton_clicked();

    void on_connectButton_clicked();

private:
    Ui::MainWindow *ui;

    Communication *comm = nullptr;
};

#endif // MAINWINDOW_H
