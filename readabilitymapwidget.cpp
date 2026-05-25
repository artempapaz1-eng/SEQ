#include "readabilitymapwidget.h"
#include <QTextCharFormat>
#include <QTextCursor>
#include <QRegularExpression>
#include <QDebug>

ReadabilityMapWidget::ReadabilityMapWidget(QWidget *parent)
    : QTextEdit(parent)
{
    setReadOnly(true);
    setFontFamily("Arial");
    setFontPointSize(12);
}

void ReadabilityMapWidget::setAnalyzedText(const QString& text)
{
    m_sourceText = text;
    setPlainText(text); // очищает все форматы
    analyzeAndHighlight();
}

void ReadabilityMapWidget::analyzeAndHighlight()
{
    QTextCursor cursor(document());

    // Разбиваем на предложения (очень просто для примера)
    QRegularExpression sentenceRegex("([^.!?]+[.!?])");
    auto it = sentenceRegex.globalMatch(m_sourceText);

    int pos = 0;
    while (it.hasNext()) {
        auto match = it.next();
        QString sent = match.captured(1);
        int start = match.capturedStart();
        int end = match.capturedEnd();

        // Для предложения считаем количество слов и среднюю длину слова
        QStringList words = sent.split(QRegularExpression("\\b"), Qt::SkipEmptyParts);
        int wordCount = 0;
        int totalWordLen = 0;
        for (const QString &w : words) {
            if (w.trimmed().size() > 1 && w.trimmed().at(0).isLetter()) {
                wordCount++;
                totalWordLen += w.trimmed().size();
            }
        }
        double avgWordLen = wordCount ? double(totalWordLen)/wordCount : 0;

        // Подкрашиваем всё предложение согласно метрике
        QTextCharFormat fmt;
        fmt.setBackground(colorForSentence(wordCount, avgWordLen));

        QTextCursor tc = cursor;
        tc.setPosition(start);
        tc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, end - start);
        tc.setCharFormat(fmt);

        // Дополнительно длинные слова можно отдельно выделять:
        QRegularExpression wordRegex("\\w{8,}"); // слова длиннее 8 букв
        auto wordIt = wordRegex.globalMatch(sent);
        while (wordIt.hasNext()) {
            auto wmatch = wordIt.next();
            int wstart = start + wmatch.capturedStart();
            int wlen = wmatch.capturedLength();

            QTextCharFormat wfmt;
            wfmt.setBackground(colorForWord(wmatch.captured()));
            wfmt.setFontWeight(QFont::Bold);
            QTextCursor wc = cursor;
            wc.setPosition(wstart);
            wc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, wlen);
            wc.setCharFormat(wfmt);
        }
    }
}

QColor ReadabilityMapWidget::colorForSentence(int wordCount, double avgWordLen) const
{
    // ПРОСТО: короткие предложения и слова — зелёный, длиннее — к красному
    if (wordCount < 10 && avgWordLen < 6) return QColor(190,255,190);        // очень просто
    if (wordCount < 20 && avgWordLen < 7) return QColor(255,255,180);        // средне
    if (wordCount < 30 && avgWordLen < 8) return QColor(255,220,146);        // сложно
    return QColor(255,180,180);                                              // очень сложно
}

QColor ReadabilityMapWidget::colorForWord(const QString& word) const
{
    // Выделяем длинные и "феноменально нечастые" слова, пример
    if (word.length() >= 12) return QColor(255,80,80);       // выделяем красным особенно длинные
    if (word.length() >= 8) return QColor(255,160,60);       // просто длинные — оранжевым
    return QColor(); // по умолчанию не выделяем
}