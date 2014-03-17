#ifndef DEBUGDIALOG_H
#define DEBUGDIALOG_H

#include <QDialog>
#include <QString>
#include <gpsmsg.h>

namespace Ui {
class debugDialog;
}

class debugDialog : public QDialog
{
    Q_OBJECT

public:
    explicit debugDialog(QWidget *parent = 0, gpsMsg *gps = 0);
    ~debugDialog();
    void writeText(int level, QString text);

private:
    Ui::debugDialog *ui;

    bool showErrorMsgs;
    bool showWarningMsgs;
    bool showInfoMsgs;

private slots:
    void gpsIsConnected_change(bool newState);
    void gpsPacketReceived(int length);
    void gpsRecordReceived(int id);
    void gpsDataError(int error);

    void on_infoCheckbox_toggled(bool checked);
    void on_warningCheckbox_toggled(bool checked);
    void on_errorCheckbox_toggled(bool checked);
    void on_clearButton_clicked();

};

#endif // DEBUGDIALOG_H
