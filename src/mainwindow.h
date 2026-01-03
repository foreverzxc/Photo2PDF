#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QSize>
#include <QProgressDialog>
#include <QModelIndex>

#include "photomanager.h"
#include "photomodel.h"
#include "settings.h"

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

    // UI methods
    void initUI();
    void showPreview(const QModelIndex& index);
    void showProgressDialog(int maxValue);

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    // Button actions
    void onOpenImageButtonClicked();
    void onOpenPdfButtonClicked();
    void onDeleteButtonClicked();
    void onClearButtonClicked();
    void onReverseButtonClicked();
    void onRotateRightButtonClicked();
    void onRotateLeftButtonClicked();
    void onExportPdfButtonClicked();
    void onSelectSavePathButtonClicked();

    // UI events
    void onCurrentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
    void onResizeOptionChanged(int index);
    void onSavePathChanged(const QString& path);
    void onPixelValueChanged(const QString& value);
    void onNoExpandChanged(bool checked);

    // Model signals
    void onRowsInserted(const QModelIndex& parent, int first, int last);
    void onRowsRemoved(const QModelIndex& parent, int first, int last);
    void onModelReset();
    void onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);

    // PhotoManager signals
    void onExportProgress(int current, int total);
    void onExportFinished();
    void onExportError(const QString& message);

private:
    Ui::MainWindow* ui;
    PhotoManager* photoManager;
    PhotoModel* photoModel;
    Settings& settings;

    QSize iconLabelSize;
    bool iconLabelFlag = false;
    QProgressDialog* progressDialog = nullptr;

    // Helper methods
    void setupConnections();
    void updateTableColumnWidths();
    void deleteSelectedItem();
    void clearAllItems();
    void reverseItems();
    void rotateSelectedItem(int angle);
    void exportToPdf();
    bool validateExportSettings();
    void loadSettings();
    void initTableWithModel();
    void rebuildTableFromModel();
};
#endif // MAINWINDOW_H
