// exportpdfworker.h
#ifndef EXPORTPDFWORKER_H
#define EXPORTPDFWORKER_H

#include <QThread>
#include <QStringList>
#include <QImage>
#include <QPdfWriter>
#include <QPainter>
#include <QTransform>
#include <QDebug>

#include "transformers.h"

class ExportPDFWorker : public QThread
{
    Q_OBJECT

public:
    explicit ExportPDFWorker(const QStringList &fileNames, const QString &outputPdfPath,QList<Transformers> &transformersList, QObject *parent = nullptr)
        : QThread(parent), fileNames(fileNames),outputPdfPath(outputPdfPath), transformersList(transformersList) {}

protected:
    void run() override {
        ExportPDF();
    }

signals:
    void ProgressUpdatedSignal(int current, int total);
    void FinishedSignal();
    void ErrorOccurredSignal(const QString &message);

private:
    void ExportPDF() {
        QPdfWriter pdfWriter(outputPdfPath);
        pdfWriter.setResolution(300);  // 设置 DPI 为 300
        pdfWriter.setPageMargins(QMarginsF(0, 0, 0, 0));

        QPainter painter;
        QString imagePath;
        int totalFiles = fileNames.size();
        for (int i = 0; i < totalFiles; ++i) {
            imagePath = fileNames[i];
            QImage image(imagePath);
            if (image.isNull()) {
                emit ErrorOccurredSignal("Failed to load image: " + imagePath);
                continue;
            }

            emit ProgressUpdatedSignal(i + 1, totalFiles);  // 更新进度

            QTransform transform;
            transform.rotate(transformersList[i].rotation);  // 使用默认的旋转角度，如果有变换需求可以更改
            QImage transformedImage = image.transformed(transform);

            qreal dpi = 300.0;
            qreal widthInMm = transformedImage.width() * 25.4 / dpi;
            qreal heightInMm = transformedImage.height() * 25.4 / dpi;

            pdfWriter.setPageSize(QPageSize(QSizeF(widthInMm, heightInMm), QPageSize::Millimeter));

            if (i == 0) {
                if (!painter.begin(&pdfWriter)) {
                    emit ErrorOccurredSignal("Failed to begin painting PDF.");
                    return;
                }
                painter.setRenderHint(QPainter::Antialiasing);
            } else {
                pdfWriter.newPage();
            }

            painter.drawImage(0, 0, transformedImage);
        }
        painter.end();

        emit FinishedSignal();
    }

private:
    QStringList fileNames;
    QString outputPdfPath;
    QList<Transformers> transformersList;
};

#endif // EXPORTPDFWORKER_H
