#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <fstream>
#include <vector>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);

    //background
    QPixmap background("lol44_resources/menu/background.png");
    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Window, background);
    this->setPalette(palette);
}

MainWindow::~MainWindow()
{
    delete ui;
}

std::vector<int> MainWindow::getVectorFromFile(){
    std::ifstream input("lol44_resources/menu_input/vector.txt");
    std::stringstream buffer;
    buffer << input.rdbuf();
    std::string hi = buffer.str();
    std::stringstream ss(hi);
    std::vector<int> champions;

    for (int i; ss >> i;) {
        champions.push_back(i);
        if (ss.peek() == ',')
            ss.ignore();
     }

    return champions;
}

void MainWindow::on_enterChampion_textChanged(const QString &arg1){
    std::ofstream myfile;
    myfile.open("lol44_resources/menu_output/champion.txt");
    std::string champion = arg1.toStdString();
    myfile << champion;
    myfile.close();
}

void MainWindow::on_enterChampion_2_textChanged(const QString &arg1){
    std::ofstream myfile;
    myfile.open("lol44_resources/menu_output/champion2.txt");
    std::string champion = arg1.toStdString();
    myfile << champion;
    myfile.close();
}

void MainWindow::on_enterChampion_3_textChanged(const QString &arg1){
    std::ofstream myfile;
    myfile.open("lol44_resources/menu_output/champion3.txt");
    std::string champion = arg1.toStdString();
    myfile << champion;
    myfile.close();
}


void MainWindow::on_loadChampions_clicked(){
    std::vector<int> champions = getVectorFromFile();

    //store paths in vector to draw later
    std::vector<QString> paths;
    for (int i = 0; i < champions.size() - 1; i++){
        QString path = "lol44_resources/champions/";
        path = path + QString::fromStdString(std::to_string(champions[i])) + ".jpg";
        paths.push_back(path);
    }

    //draw to 6 different labels here
    int w = ui->Champ1->width();
    int h = ui->Champ1->height();
    QPixmap Champ1Pix(paths[0]);
    ui->Champ1->setPixmap(Champ1Pix.scaled(w,h,Qt::KeepAspectRatio));

    QPixmap Champ2Pix(paths[1]);
    ui->Champ2->setPixmap(Champ2Pix.scaled(w,h,Qt::KeepAspectRatio));

    QPixmap Champ3Pix(paths[2]);
    ui->Champ3->setPixmap(Champ3Pix.scaled(w,h,Qt::KeepAspectRatio));

    QPixmap Champ4Pix(paths[3]);
    ui->Champ4->setPixmap(Champ4Pix.scaled(w,h,Qt::KeepAspectRatio));

    QPixmap Champ5Pix(paths[4]);
    ui->Champ5->setPixmap(Champ5Pix.scaled(w,h,Qt::KeepAspectRatio));

    QPixmap Champ6Pix(paths[5]);
    ui->Champ6->setPixmap(Champ6Pix.scaled(w,h,Qt::KeepAspectRatio));
}


void MainWindow::on_Dijkstras_clicked(){
    if(!dijkstrasButton){
        std::ifstream input("lol44_resources/menu_input/Dijkstra.txt");
        std::stringstream buffer;
        buffer << input.rdbuf();
        std::string hi = buffer.str();
        ui->textBrowser_4->setPlainText(QString::fromStdString(hi));
        dijkstrasButton = true;
    }
    else{
        ui->textBrowser_4->setPlainText("");
        dijkstrasButton = false;
    }

}


void MainWindow::on_BellmanFord_clicked(){
    if(!bellmanFordButton){
        std::ifstream input("lol44_resources/menu_input/BellmanFord.txt");
        std::stringstream buffer;
        buffer << input.rdbuf();
        std::string hi = buffer.str();
        ui->textBrowser_4->setPlainText(QString::fromStdString(hi));
        bellmanFordButton = true;
    }
    else{
        ui->textBrowser_4->setPlainText("");
        bellmanFordButton = false;
    }
}


void MainWindow::on_Faker_clicked(){
    if(!fakerButton){
        int w = ui->faker1->width();
        int h = ui->faker1->height();
        QPixmap fakerPix1("lol44_resources/menu/FAKER.jpg");
        ui->faker1->setPixmap(fakerPix1.scaled(w,h,Qt::KeepAspectRatio));

        QPixmap fakerPix2("lol44_resources/menu/FAKER2.jpeg");
        ui->faker2->setPixmap(fakerPix2.scaled(w,h,Qt::KeepAspectRatio));
        fakerButton = true;
    }
    else{
         ui->faker1->clear();
         ui->faker2->clear();
         fakerButton = false;
    }
}






