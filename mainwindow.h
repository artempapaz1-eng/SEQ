#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QtQuickWidgets/QQuickWidget>
#include "readabilitymapwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_doButton_clicked();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *kaleidoScene;
};
#endif // MAINWINDOW_H
