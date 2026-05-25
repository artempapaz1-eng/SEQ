#pragma once

#include <QTextEdit>
#include <QVector>

/**
 * Виджет, подсвечивающий сложные слова и предложения в тексте.
 * Цветовая разметка Readability Map.
 */
class ReadabilityMapWidget : public QTextEdit
{
    Q_OBJECT
public:
    explicit ReadabilityMapWidget(QWidget *parent = nullptr);

    /// Установить текст и выполнить подсветку
    void setAnalyzedText(const QString& text);

protected:
    /// Механика разметки - вызывается внутри setAnalyzedText
    void analyzeAndHighlight();
    /// Примеры граничных значений - подгоняй под задачу
    QColor colorForSentence(int wordCount, double avgWordLen) const;
    QColor colorForWord(const QString& word) const;

private:
    QString m_sourceText;
};