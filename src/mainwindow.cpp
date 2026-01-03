#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QImage>
#include <QPdfDocument>
#include <QTransform>
#include <QPixmap>
#include <QIntValidator>
#include <QDebug>
#include <QFileInfo>
#include <QDir>

extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , photoManager(new PhotoManager(this))
    , photoModel(new PhotoModel(this))
    , settings(Settings::instance())
{
    ui->setupUi(this);

    // Initialize table with model (for row tracking)
    initTableWithModel();

    setupConnections();
    initUI();
    loadSettings();

    // Initialize language checkmarks and menu titles
    QString currentLang = settings.language();
    if (currentLang == "en") {
        ui->actionChinese->setText("简体中文");
        ui->actionEnglish->setText("✓ English");
        // Menu title opposite to current language
        ui->menuLanguage->setTitle("语言");
        ui->actionAbout->setText("About Photo2PDF");
    } else {
        ui->actionChinese->setText("✓ 简体中文");
        ui->actionEnglish->setText("English");
        // Menu title opposite to current language
        ui->menuLanguage->setTitle("Language");
        ui->actionAbout->setText("关于 Photo2PDF");
    }
}

MainWindow::~MainWindow()
{
    // Save window state
    Settings::instance().setWindowState(saveState());
    Settings::instance().setWindowGeometry(saveGeometry());
    Settings::instance().save();

    delete ui;
}

void MainWindow::initTableWithModel()
{
    // Set selection mode for the table
    ui->photoTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->photoTable->setSelectionMode(QAbstractItemView::SingleSelection);
}

void MainWindow::setupConnections()
{
    // Button connections
    connect(ui->openFileButton, &QPushButton::clicked, this, &MainWindow::onOpenImageButtonClicked);
    connect(ui->openPDFButton, &QPushButton::clicked, this, &MainWindow::onOpenPdfButtonClicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteButtonClicked);
    connect(ui->clearButton, &QPushButton::clicked, this, &MainWindow::onClearButtonClicked);
    connect(ui->reverseButton, &QPushButton::clicked, this, &MainWindow::onReverseButtonClicked);
    connect(ui->rightRotationButton, &QPushButton::clicked, this, &MainWindow::onRotateRightButtonClicked);
    connect(ui->leftRotationButton, &QPushButton::clicked, this, &MainWindow::onRotateLeftButtonClicked);
    connect(ui->exportPDFButton, &QPushButton::clicked, this, &MainWindow::onExportPdfButtonClicked);
    connect(ui->selectSavePathButton, &QPushButton::clicked, this, &MainWindow::onSelectSavePathButtonClicked);

    // Table selection change - use currentRowChanged with table row
    connect(ui->photoTable, &QTableWidget::currentCellChanged,
            this, &MainWindow::onCurrentCellChanged);

    // Combo box and other UI controls
    connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onResizeOptionChanged);
    connect(ui->savePathEdit, &QLineEdit::textChanged, this, &MainWindow::onSavePathChanged);
    connect(ui->pixelLineEdit, &QLineEdit::textChanged, this, &MainWindow::onPixelValueChanged);
    connect(ui->noExpandCheckBox, &QCheckBox::toggled, this, &MainWindow::onNoExpandChanged);

    // Model signals - when items are added/removed from model
    connect(photoModel, &QAbstractItemModel::rowsInserted, this, &MainWindow::onRowsInserted);
    connect(photoModel, &QAbstractItemModel::rowsRemoved, this, &MainWindow::onRowsRemoved);
    connect(photoModel, &QAbstractItemModel::modelReset, this, &MainWindow::onModelReset);
    connect(photoModel, &QAbstractItemModel::dataChanged, this, &MainWindow::onModelDataChanged);

    // PhotoManager signals for export
    connect(photoManager, &PhotoManager::exportProgressSignal, this, &MainWindow::onExportProgress);
    connect(photoManager, &PhotoManager::exportFinishedSignal, this, &MainWindow::onExportFinished);
    connect(photoManager, &PhotoManager::exportErrorSignal, this, &MainWindow::onExportError);

    // About menu action
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAboutActionTriggered);

    // Language switching actions
    connect(ui->actionChinese, &QAction::triggered, this, &MainWindow::onChineseTriggered);
    connect(ui->actionEnglish, &QAction::triggered, this, &MainWindow::onEnglishTriggered);

    // TableWidgetUpDown signals (for drag-drop reordering)
    connect(ui->photoTable, &TableWidgetUpDown::swapPhotosSignal,
            this, [this](int src, int dst) {
        if (src < 0 || src >= photoModel->rowCount()) return;
        if (dst < 0 || dst > photoModel->rowCount()) return;

        static bool inDragDrop = false;
        if (inDragDrop) return;
        inDragDrop = true;

        photoModel->blockSignals(true);
        photoModel->moveRows(QModelIndex(), src, 1, QModelIndex(), dst);
        rebuildTableFromModel();
        photoModel->blockSignals(false);
        inDragDrop = false;
    });
    connect(ui->photoTable, &TableWidgetUpDown::showIconSignal,
            this, [this]() {
        int row = ui->photoTable->currentRow();
        if (row >= 0 && row < photoModel->rowCount()) {
            showPreview(photoModel->index(row));
        }
    });
}

void MainWindow::initUI()
{
    setWindowTitle("Photo2PDF");

    // Setup icons
    QIcon rightRotationIcon(QPixmap(":/resources/right_rotation_icon.png"));
    QIcon leftRotationIcon(QPixmap(":/resources/left_rotation_icon.png"));
    ui->rightRotationButton->setIcon(rightRotationIcon);
    ui->leftRotationButton->setIcon(leftRotationIcon);

    // Configure table columns (first column takes all space since page is now in displayName)
    updateTableColumnWidths();
    ui->photoTable->setColumnWidth(0, ui->photoTable->width() * 1.0);
    ui->photoTable->hideColumn(1);  // Hide second column since it's combined into first

    // Setup validators
    ui->pixelLineEdit->setValidator(new QIntValidator(ui->pixelLineEdit));

    // Restore window geometry/state
    if (!settings.windowGeometry().isEmpty()) {
        restoreGeometry(settings.windowGeometry());
    }
    if (!settings.windowState().isEmpty()) {
        restoreState(settings.windowState());
    }
}

void MainWindow::loadSettings()
{
    // Load saved output path
    QString savedPath = settings.outputPath();
    if (!savedPath.isEmpty()) {
        ui->savePathEdit->setText(savedPath);
    }

    // Load resize option
    int resizeOption = static_cast<int>(settings.resizeOption());
    if (resizeOption >= 0 && resizeOption < ui->comboBox->count()) {
        ui->comboBox->setCurrentIndex(resizeOption);
    }

    // Load pixel value
    int pixelValue = settings.configPixel();
    if (pixelValue > 0) {
        ui->pixelLineEdit->setText(QString::number(pixelValue));
    }

    // Load no-expand setting
    ui->noExpandCheckBox->setChecked(settings.noExpand());

    // Apply config to PhotoManager
    photoManager->setExportConfig(settings.exportConfig());
}

void MainWindow::updateTableColumnWidths()
{
    if (!ui->photoTable) return;
    int tableWidth = ui->photoTable->width();
    if (tableWidth > 0) {
        ui->photoTable->setColumnWidth(0, tableWidth * 1.0);
    }
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    updateTableColumnWidths();
}

void MainWindow::showPreview(const QModelIndex& index)
{
    if (!index.isValid() || index.row() >= photoModel->rowCount()) {
        ui->iconLabel->clear();
        ui->photoPixelInfoLabel->setText(tr("当前图片大小为："));
        return;
    }

    PhotoItem item = photoModel->getItem(index.row());
    QImage image;

    if (item.type() == PhotoItem::Type::Image) {
        image = QImage(item.filePath());
    } else {
        QPdfDocument document;
        document.load(item.filePath());
        QSizeF size = document.pagePointSize(item.pageNumber());
        image = document.render(item.pageNumber(), QSize(size.width(), size.height()));
    }

    if (image.isNull()) {
        QMessageBox::information(this, tr("Open Image"), tr("Cannot load the image."));
        return;
    }

    // Update pixel info
    ui->photoPixelInfoLabel->setText(tr("当前图片大小为：") +
                                      QString::number(image.width()) + "×" +
                                      QString::number(image.height()));

    // Apply rotation
    if (!iconLabelFlag) {
        iconLabelFlag = true;
        iconLabelSize = ui->iconLabel->size();
    }

    QTransform transform;
    transform.rotate(photoModel->getRotation(index.row()));
    QPixmap pixmap = QPixmap::fromImage(image);
    QPixmap rotatedPixmap = pixmap.transformed(transform);
    QPixmap scaledPixmap = rotatedPixmap.scaled(iconLabelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->iconLabel->setPixmap(scaledPixmap);
}

void MainWindow::showProgressDialog(int maxValue)
{
    progressDialog = new QProgressDialog("任务进行中...", "取消", 0, maxValue, this);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setAutoClose(true);
    progressDialog->setCancelButton(nullptr);
    progressDialog->show();
}

// Button action handlers
void MainWindow::onOpenImageButtonClicked()
{
    updateTableColumnWidths();
    photoModel->addImages();
}

void MainWindow::onOpenPdfButtonClicked()
{
    updateTableColumnWidths();
    photoModel->addPdfs();
}

void MainWindow::onDeleteButtonClicked()
{
    int row = ui->photoTable->currentRow();
    if (row >= 0) {
        photoModel->removeRow(row);
    }
}

void MainWindow::onClearButtonClicked()
{
    QMessageBox::StandardButton box = QMessageBox::question(
        this,
        tr("提示"),
        tr("确定要清空所有图片吗？"),
        QMessageBox::Yes | QMessageBox::No
    );

    if (box == QMessageBox::Yes) {
        photoModel->clear();
    }
}

void MainWindow::onReverseButtonClicked()
{
    photoModel->reverse();
}

void MainWindow::onRotateRightButtonClicked()
{
    rotateSelectedItem(90);
}

void MainWindow::onRotateLeftButtonClicked()
{
    rotateSelectedItem(-90);
}

void MainWindow::onExportPdfButtonClicked()
{
    exportToPdf();
}

void MainWindow::onSelectSavePathButtonClicked()
{
    QString path = QFileDialog::getSaveFileName(
        this,
        tr("选择保存路径"),
        settings.lastSaveDirectory(),
        tr("PDF文件 (*.pdf)")
    );

    if (path.isEmpty()) {
        return;
    }

    if (!path.endsWith(".pdf", Qt::CaseInsensitive)) {
        path += ".pdf";
    }

    ui->savePathEdit->setText(path);

    // Save directory for future use
    QFileInfo info(path);
    settings.setLastSaveDirectory(info.absolutePath());
}

// UI event handlers
void MainWindow::onCurrentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    Q_UNUSED(currentColumn);
    Q_UNUSED(previousRow);
    Q_UNUSED(previousColumn);
    if (currentRow >= 0 && currentRow < photoModel->rowCount()) {
        showPreview(photoModel->index(currentRow));
    } else {
        ui->iconLabel->clear();
        ui->photoPixelInfoLabel->setText(tr("当前图片大小为："));
    }
}

void MainWindow::onResizeOptionChanged(int index)
{
    ui->pixelLineEdit->setEnabled(index != 0);
    ui->noExpandCheckBox->setEnabled(index != 0);

    ResizeOption option = static_cast<ResizeOption>(index);
    settings.setResizeOption(option);
    photoManager->setResizeOption(option);
}

void MainWindow::onSavePathChanged(const QString& path)
{
    settings.setOutputPath(path);
    photoManager->setOutputPath(path);
}

void MainWindow::onPixelValueChanged(const QString& value)
{
    bool ok;
    int pixel = value.toInt(&ok);
    if (ok) {
        settings.setConfigPixel(pixel);
        photoManager->setConfigPixel(pixel);
    }
}

void MainWindow::onNoExpandChanged(bool checked)
{
    settings.setNoExpand(checked);
    photoManager->setNoExpand(checked);
}

// Model signal handlers
void MainWindow::onRowsInserted(const QModelIndex& parent, int first, int last)
{
    Q_UNUSED(parent);
    // Add rows to table
    for (int i = first; i <= last; ++i) {
        PhotoItem item = photoModel->getItem(i);
        int row = ui->photoTable->rowCount();
        ui->photoTable->insertRow(row);
        ui->photoTable->setRowHeight(row, 40);
        ui->photoTable->setItem(row, 0, new QTableWidgetItem(item.displayName()));
        ui->photoTable->setItem(row, 1, new QTableWidgetItem(item.displayPage()));
    }
    updateTableColumnWidths();

    // Select first new item
    if (first == 0 && photoModel->rowCount() > 0) {
        ui->photoTable->setCurrentCell(0, 0);
    }
}

void MainWindow::onRowsRemoved(const QModelIndex& parent, int first, int last)
{
    Q_UNUSED(parent);
    Q_UNUSED(first);
    Q_UNUSED(last);
    // Rebuild table from model
    rebuildTableFromModel();
}

void MainWindow::onModelReset()
{
    rebuildTableFromModel();
}

void MainWindow::onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight,
                                     const QVector<int>& roles)
{
    Q_UNUSED(roles);
    // Update changed rows in table
    for (int row = topLeft.row(); row <= bottomRight.row(); ++row) {
        PhotoItem item = photoModel->getItem(row);
        if (ui->photoTable->item(row, 0)) {
            ui->photoTable->item(row, 0)->setText(item.displayName());
        }
        if (ui->photoTable->item(row, 1)) {
            ui->photoTable->item(row, 1)->setText(item.displayPage());
        }
    }
    // Refresh preview
    int currentRow = ui->photoTable->currentRow();
    if (currentRow >= 0 && currentRow < photoModel->rowCount()) {
        showPreview(photoModel->index(currentRow));
    }
}

void MainWindow::rebuildTableFromModel()
{
    ui->photoTable->clearContents();
    ui->photoTable->setRowCount(0);

    for (int i = 0; i < photoModel->rowCount(); ++i) {
        PhotoItem item = photoModel->getItem(i);
        int row = ui->photoTable->rowCount();
        ui->photoTable->insertRow(row);
        ui->photoTable->setRowHeight(row, 40);
        ui->photoTable->setItem(row, 0, new QTableWidgetItem(item.displayName()));
        ui->photoTable->setItem(row, 1, new QTableWidgetItem(item.displayPage()));
    }
    updateTableColumnWidths();
}

// PhotoManager signal handlers
void MainWindow::onExportProgress(int current, int total)
{
    if (progressDialog) {
        progressDialog->setValue(current);
        progressDialog->setMaximum(total);
    }
}

void MainWindow::onExportFinished()
{
    if (progressDialog) {
        progressDialog->close();
        delete progressDialog;
        progressDialog = nullptr;
    }
    QMessageBox::information(this, tr("完成"), tr("PDF导出完成！"));
}

void MainWindow::onExportError(const QString& message)
{
    if (progressDialog) {
        progressDialog->close();
        delete progressDialog;
        progressDialog = nullptr;
    }
    QMessageBox::warning(this, tr("错误"), tr("导出失败：") + message);
}

void MainWindow::onAboutActionTriggered()
{
    QMessageBox::about(this, tr("关于 Photo2PDF"),
        tr("Photo2PDF\n\n"
           "一个简单易用的图片转PDF工具。\n\n"
           "支持功能：\n"
           "• 导入图片文件 (PNG, JPG, BMP等)\n"
           "• 导入PDF文件 (每一页都会解析成一张图片)\n"
           "• 调整图片顺序\n"
           "• 旋转图片\n"
           "• 导出为PDF\n\n"
           "Version 1.0"));
}

void MainWindow::onChineseTriggered()
{
    switchLanguage("zh_CN");
}

void MainWindow::onEnglishTriggered()
{
    switchLanguage("en");
}

void MainWindow::switchLanguage(const QString& language)
{
    // Remove old translator
    qApp->removeTranslator(&translator);

    // Save language preference
    settings.setLanguage(language);

    // Load new translator from embedded resources
    if (translator.load(":/i18n/translations/Photo2PDF_" + language + ".qm")) {
        qApp->installTranslator(&translator);
        qDebug() << "Switched to language:" << language;
    } else {
        qDebug() << "Failed to load translation for:" << language;
    }

    // Force UI retranslation first (translates all standard UI elements)
    ui->retranslateUi(this);

    // Then update checkmarks for language menu items (after retranslateUi)
    if (language == "zh_CN") {
        ui->actionChinese->setText("✓ 简体中文");
        ui->actionEnglish->setText("English");
        // Menu title opposite to current language
        ui->menuLanguage->setTitle("Language");
        ui->actionAbout->setText("关于 Photo2PDF");
    } else {
        ui->actionChinese->setText("简体中文");
        ui->actionEnglish->setText("✓ English");
        // Menu title opposite to current language
        ui->menuLanguage->setTitle("语言");
        ui->actionAbout->setText("About Photo2PDF");
    }

    // Update window title
    setWindowTitle("Photo2PDF");
}

// Helper methods
void MainWindow::rotateSelectedItem(int angle)
{
    int row = ui->photoTable->currentRow();
    if (row >= 0) {
        photoModel->rotateItem(row, angle);
    }
}

void MainWindow::exportToPdf()
{
    QString savePath = ui->savePathEdit->text();

    if (savePath.isEmpty()) {
        QMessageBox::information(this, tr("错误"), tr("请输入保存路径"));
        return;
    }

    QFileInfo fileInfo(savePath);
    QDir dir = fileInfo.dir();

    if (dir.exists() && fileInfo.isWritable()) {
        QMessageBox::information(this, tr("错误"), tr("保存路径无效或无写入权限"));
        return;
    }

    if (photoModel->rowCount() == 0) {
        QMessageBox::information(this, tr("错误"), tr("请添加至少一张图片"));
        return;
    }

    showProgressDialog(photoModel->rowCount());
    photoManager->exportFromModel(photoModel);
}
