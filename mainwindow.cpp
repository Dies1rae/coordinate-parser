#include "mainwindow.h"
#include "ui_mainwindow.h"

std::vector<QString> MainWindow::reg_ex_pattern_ = {
    "((\\d+)\\s?.?\\s?(\\d+)\\s?.?\\s?(\\d{1,}\\.?\\,?\\d{0,}?).?\\s?(N|W|S|E|n|w|s|e)(,?| ?|, )(\\d+)\\s?.?\\s?(\\d+)\\s?.?\\s?(\\d{1,}\\.?\\,?\\d{0,}?).?\\s?(N|W|S|E|n|w|s|e))",
    "([+-]?\\d+\\.?\\d+)\\s*,\\s*([+-]?\\d+\\.?\\d+)"
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("COORDINATE PARSER");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pb_load_db_clicked()
{
    this->db_file_path_ =  QFileDialog::getOpenFileName(this, tr("Choose DB file"), "./", tr("Images (*.txt *.xml *.json)"));
}


void MainWindow::on_pb_export_csv_clicked()
{

    if(this->db_file_path_.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setText("DB file not load");
        msgBox.exec();
        return;
    }

    QFile file(this->db_file_path_);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    std::vector<QString> coordinates;

    while (!in.atEnd()) {
        QString line = in.readLine();
        for(const auto& r : reg_ex_pattern_) {
            QRegularExpression re(r);
            for (const QRegularExpressionMatch &match : re.globalMatch(line)) {
                for(const auto& cpt : match.capturedTexts()) {
                    if((cpt.contains(',') || cpt.contains(' ')) && cpt.size() > 5) {
                        coordinates.push_back(cpt);
                    }
                }
            }
        }
    }

    file.close();
    for(auto& coord : coordinates) {
        if((coord.contains("° N") || coord.contains("° E")) && !coord.contains("'") && !coord.contains(" \" ") ) {
            coord.remove("° N");
            coord.remove("° E");
        }
        else if((coord.contains("’") || coord.contains("'")) && coord.contains("°")) {
            QRegularExpression doubs("(\\d+)?\\.?(\\d+)?");
            QRegularExpressionMatchIterator ptr = doubs.globalMatch(coord);
            std::vector<double> numbers;
            double lat = 0.0;
            double lon = 0.0;
            while(ptr.hasNext()) {
                QRegularExpressionMatch match = ptr.next();
                if(match.captured(0).toDouble() != 0) {
                    numbers.push_back(match.captured(0).toDouble());
                }
            }
            lat = numbers[0] + ((numbers[1] / 60) + numbers[2] / 3600);
            lon = numbers[3] + ((numbers[4] / 60) + numbers[5] / 3600);
            coord = QString::number(lat) + ", " + QString::number(lon);
        }
    }
    this->coords_db_ = { coordinates.begin(), coordinates.end() };
    this->saveDataToCSV();
}

void MainWindow::saveDataToCSV() {
    this->refreshTbvCoords();
    QString filename = QString("./coords_.csv");
    QFile dst_file(filename);
    if (dst_file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&dst_file);
        size_t id = 1;
        for (auto ptr = this->coords_db_.begin(); ptr != this->coords_db_.end(); ++ptr, ++id) {
            stream << id << ';' << *ptr << Qt::endl;
        }
    }
    dst_file.close();
}

void MainWindow::refreshTbvCoords() {
    QMap<int, QString> id_coord;
    size_t id = 1;
    for (auto ptr = this->coords_db_.begin(); ptr != this->coords_db_.end(); ++ptr, ++id ) {
        id_coord[id] =  (*ptr);
    }
    QStandardItemModel* model = new QStandardItemModel(id_coord.size(), 1);
    for (int row = 0, ids = 1; row < model->rowCount(); ++row, ++ids) {
        for (int column = 0; column < model->columnCount(); ++column) {
            QStandardItem *item = new QStandardItem( QString("%0").arg(id_coord[ids]));
            item->setEditable(false);
            model->setItem(row, column, item);
        }
    }
    model->setHorizontalHeaderItem( 0, new QStandardItem( "COORDINATES" ) );
    this->ui->tbv_coord->setModel(model);
    this->ui->tbv_coord->resizeColumnsToContents();
    this->ui->tbv_coord->show();
}

void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::quit();
}


void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(
        this,
        tr("NORMANS RIDERS MC COORDINATE PARSER"),
        tr("Hello friend, this small simple programm on C++ and QT framework.\nMain theme is grab text information and search coordinate datatype like:\n 60.150330, 30.467381 or 59°52'17\"N 30°25'25\"E.\nThen structre it and export to standart CSV file\nCreated by Sense_Net") );
}

