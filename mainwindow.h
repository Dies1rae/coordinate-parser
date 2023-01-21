#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <vector>
#include <QStringList>
#include <iostream>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QByteArray>
#include <QRegularExpression>
#include <set>
#include <QMap>
#include <QModelIndex>
#include <QStandardItemModel>
#include <QTableView>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pb_load_db_clicked();

    void on_pb_export_csv_clicked();
    void on_actionExit_triggered();

    void on_actionAbout_triggered();

private:
    void createFile();

    void saveDataToCSV();

    void refreshTbvCoords();

    static std::vector<QString> reg_ex_pattern_;
    std::set<QString>           coords_db_;
    QString                     db_file_path_;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
