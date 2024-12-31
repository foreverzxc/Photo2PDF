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
#include <QPdfDocument>
//user
#include "transformers.h"
#include "config.h"

class ExportPDFWorker : public QThread
{
    Q_OBJECT

public:
    explicit ExportPDFWorker(const QStringList &fileNames,const QStringList &pages, QList<Transformers> &transformersList,const ExportPDFConfig &config, QObject *parent = nullptr)
        : QThread(parent), fileNames(fileNames), pages(pages),transformersList(transformersList), config(config) {}

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
        QPdfWriter pdfWriter(config.outputPath);
        pdfWriter.setResolution(72);  // 设置 DPI
        pdfWriter.setPageMargins(QMarginsF(0, 0, 0, 0));

        QPainter painter;
        QString imagePath;
        int totalFiles = fileNames.size();
        for (int i = 0; i < totalFiles; ++i) {
            imagePath = fileNames[i];
            QImage image;
            if(pages[i]=="")
            {
                image = QImage(imagePath);
            }
            else
            {
                // qDebug()<<i+1<<'/'<<totalFiles;
                int page = pages[i].toInt()-1;
                QPdfDocument* document = new QPdfDocument;
                document->load(imagePath);
                QSizeF size = document->pagePointSize(page);
                image = document->render(page, QSize(size.width(), size.height()));
                document->close();
            }

            emit ProgressUpdatedSignal(i + 1, totalFiles);  // 更新进度
            if (image.isNull()) {
                emit ErrorOccurredSignal("Failed to load image: " + imagePath);
                continue;
            }

            QTransform transform;
            transform.rotate(transformersList[i].rotation);
            QImage transformedImage = image.transformed(transform);


            double scale = 1.0;

            if(config.resizeOption==ResizeOption::ResizeShortSide)
            {
                double shortLen = fmin(transformedImage.width(),transformedImage.height());
                scale = config.configPixel/shortLen;
                if(config.noExpand)
                {
                    scale = fmin(1,scale);
                }
            }
            if(config.resizeOption==ResizeOption::ResizeLongSide)
            {
                double longtLen = fmax(transformedImage.width(),transformedImage.height());
                scale = config.configPixel/longtLen;
                if(config.noExpand)
                {
                    scale = fmin(1,scale);
                }
            }
            if(scale != 1)
            {
                transformedImage = transformedImage.scaled(transformedImage.size()*scale);
            }

            qreal dpi = config.dpi;
            qreal widthInMm = transformedImage.width() * 25.4 / dpi;
            qreal heightInMm = transformedImage.height() * 25.4 / dpi;
            // qDebug()<<widthInMm<<" "<<heightInMm;

            // pdfWriter.setPageSize(QPageSize(QSizeF(widthInMm, heightInMm), QPageSize::Millimeter,"",QPageSize::ExactMatch));
            pdfWriter.setPageSize(QPageSize(transformedImage.size(),QPageSize::Point,"",QPageSize::ExactMatch));

            if (i == 0) {
                if (!painter.begin(&pdfWriter)) {
                    emit ErrorOccurredSignal("Failed to begin painting PDF.");
                    return;
                }
                painter.setRenderHint(QPainter::Antialiasing);
            } else {
                pdfWriter.newPage();
            }
            qDebug() << transformedImage.size();
            painter.drawImage(QPoint(0,0), transformedImage);
        }
        painter.end();

        emit FinishedSignal();
    }

private:
    QStringList fileNames;
    QStringList pages;
    QList<Transformers> transformersList;
    ExportPDFConfig config;
};

#endif // EXPORTPDFWORKER_H
