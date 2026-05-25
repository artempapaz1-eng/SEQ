#include "mainwindow.h"
#include "heatmapwidget.h"
#include "readabilitymapwidget.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QFont>
#include <QColor>
#include <QQuickItem> // Добавь этот инклуд для работы с QML объектами
#include "readabilitymapwidget.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Подгружаем наш 3D мир в виджет калейдоскопа
    ui->kaleidoskop->setSource(QUrl("qrc:/Kaleidoscope3D.qml"));
    ui->kaleidoskop->setResizeMode(QQuickWidget::SizeRootObjectToView);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_doButton_clicked()
{
    QString inputText = ui->lineEdit->text();

    if (inputText.isEmpty()) {
        ui->hexViewer->setPlainText("Введите какой-нибудь текст!");
        return;
    }

    // ==========================================
    // ЛОГИКА ДЛЯ TAB 3: HEX VIEWER (Оставляем как было)
    // ==========================================
    QFont monoFont("Courier New", 10);
    ui->hexViewer->setFont(monoFont);

    QByteArray bytes = inputText.toUtf8();
    QString hexResult = "";

    int index = 0;
    int bytesCount = bytes.size();

    while (index < bytesCount) {
        int rowAddress = index;
        int lineBytesCount = 0;

        while (lineBytesCount < 16 && (index + lineBytesCount) < bytesCount) {
            if (lineBytesCount == 15) {
                unsigned char nextByte = (unsigned char)bytes[index + lineBytesCount];
                if (nextByte >= 0xD0 && nextByte <= 0xD3) {
                    break;
                }
            }
            lineBytesCount++;
        }

        QByteArray lineBytes = bytes.mid(index, lineBytesCount);
        index += lineBytesCount;

        QString addressStr = QString("%1: ").arg(rowAddress, 8, 16, QChar('0')).toUpper();

        QString hexPart = "";
        for (int j = 0; j < lineBytes.size(); ++j) {
            unsigned char byte = (unsigned char)lineBytes[j];
            hexPart += QString("%1 ").arg(byte, 2, 16, QChar('0')).toUpper();
        }

        if (lineBytesCount < 16) {
            hexPart += QString(" ").repeated((16 - lineBytesCount) * 3);
        }

        QString textPart = QString::fromUtf8(lineBytes);

        for (int j = 0; j < textPart.length(); ++j) {
            if (textPart[j].isSpace() && textPart[j] != ' ') {
                textPart[j] = '.';
            }
        }

        hexResult += addressStr + hexPart + " | " + textPart + "\n";
    }

    ui->hexViewer->setPlainText(hexResult);

    // ==========================================
    // НОВАЯ ЛОГИКА ДЛЯ TAB 1: 3D КАЛЕЙДОСКОП
    // ==========================================

    // 1. Находим корневой объект нашего QML (файл Kaleidoscope3D.qml)
    QObject *rootObject = ui->kaleidoskop->rootObject();

    if (rootObject) {
        // 2. Вызываем функцию updateText внутри QML и передаем туда строку из lineEdit
        QMetaObject::invokeMethod(rootObject, "updateText",
                                  Q_ARG(QVariant, inputText));
    }

    HeatmapWidget *heatmap = qobject_cast<HeatmapWidget*>(ui->heatmap);
    if (heatmap) {
        heatmap->buildHeatmap(inputText);
    }


    ReadabilityMapWidget *readabilityMap = qobject_cast<ReadabilityMapWidget*>(ui->ReadabilityMapWidget);
    if (readabilityMap) {
        readabilityMap->setAnalyzedText(inputText);
    }
}