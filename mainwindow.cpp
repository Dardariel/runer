#include "mainwindow.h"

#include <QSettings>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi(this);
    sa_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_contents = new QWidget;
    m_contentsLayout = new QVBoxLayout(m_contents);
    sa_area->setWidget(m_contents);

// read config file ------------------------------------
    QSettings settings("runerconfig.ini", QSettings::IniFormat);
    settings.beginGroup("main");
    QStringList keys = settings.allKeys();
    QStringList lst;
    for(int i=0; i<keys.count(); i++)
    {
        ItemProcess *item = new ItemProcess();
        lst.clear();
        item->setOptions(keys.at(i), settings.value(keys.at(i)).toString(), lst);
        lstItm.append(item);
        m_contentsLayout->addWidget(item);
    }
    settings.endGroup();
    m_contentsLayout->addStretch();
//------------------------------------------------------
    m_contentsLayout->setSizeConstraint(QLayout::SetMinimumSize);

    connect(pb_start_all, &QPushButton::released, this, &MainWindow::slotStartAll);
    connect(pb_restart_all, &QPushButton::released, this, &MainWindow::slotRestartAll);
    connect(pb_stop_all, &QPushButton::released, this, &MainWindow::slotStopAll);
}
MainWindow::~MainWindow()
{
    slotStopAll();
}
void MainWindow::slotStartAll()
{
    for(int i=0; i<lstItm.count(); i++)
    {
        lstItm.at(i)->slotStart();
    }
}
void MainWindow::slotRestartAll()
{
    for(int i=0; i<lstItm.count(); i++)
    {
        lstItm.at(i)->slotRestart();
    }
}
void MainWindow::slotStopAll()
{
    for(int i=0; i<lstItm.count(); i++)
    {
        lstItm.at(i)->slotStop();
    }
}
