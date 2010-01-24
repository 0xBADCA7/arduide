/**
 * \file MainWindow.h
 * \author Denis Martinez
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <ui_MainWindow.h>

#include <QComboBox>
#include <QToolButton>

class QUrl;
class Browser;
class Editor;
class QTabWidget;
class QCloseEvent;

class MainWindow : public QMainWindow, Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow();
    void initialize();

private:
    void setupActions();
    void createBrowserAndTabs();
    void createDeviceChooser();

    QStringList names;
    QString createUniqueName(const QString &name);

    QTabWidget* tabWidget;
    Browser *browser;
    QComboBox *deviceBox;
    QComboBox *boardBox;
    QAction *refreshDevicesAct;

private slots:
    Editor *currentEditor();

    void closeTab(int index = -1);
    void newProject(const QString &code = QString(), Editor **pEditor = NULL);
    void open(const QString &fileName = QString());
    void save();
    void fillDeviceBox();
    void fillBoardsBox();
    void copy();
    void cut();
    void paste();
    void build();
    void upload();
    void toggleDock();
    void setDeviceAtIndex(int index);
    void setBoardAtIndex(int index);

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H