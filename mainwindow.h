#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    std::vector<int> getVectorFromFile();
    bool fakerButton = false;
    bool dijkstrasButton = false;
    bool bellmanFordButton = false;


private slots:
    void on_enterChampion_textChanged(const QString &arg1);
    void on_loadChampions_clicked();
    void on_Dijkstras_clicked();
    void on_BellmanFord_clicked();
    void on_Faker_clicked();

    void on_enterChampion_2_textChanged(const QString &arg1);

    void on_enterChampion_3_textChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
