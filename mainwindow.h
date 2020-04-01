#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLayout>

#include "ui_mainwindow.h"

#include "itemprocess.h"
#include <QList>

class MainWindow : public QMainWindow , public Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void slotStartAll();
    void slotRestartAll();
    void slotStopAll();

private:
    QList<ItemProcess*> lstItm;

    QVBoxLayout *m_layout, *m_contentsLayout;
    QWidget *m_contents;


};
#endif // MAINWINDOW_H
