#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QMdiArea>
#include "resources.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    void OnSettingsTriggered();
    void OnLineCounterTriggered();
private:
    Ui::MainWindow *ui;
    QMdiArea *workspace{};
    utilities::AppSettings& app_settings;
    QMenu *languages_menu;
};

#endif // MAINWINDOW_HPP
