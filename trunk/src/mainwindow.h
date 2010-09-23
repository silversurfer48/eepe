#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;

private slots:
    void on_listWidget_itemDoubleClicked(QListWidgetItem* item);
    void on_actionAbout_activated();
    void on_actionSave_activated();
    void on_actionOpen_activated();
    void on_actionQuit_activated();
};

#endif // MAINWINDOW_H
