#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void ReadJson();

    void slotTimerAlarm();

    void on_intervalPushButton_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    int interval;
};
#endif // MAINWINDOW_H
