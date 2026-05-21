#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "sistemparkir.h"

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
    // Slot navigasi halaman menu utama
    void on_btnMenuHalaman1_clicked();
    void on_btnMenuHalaman2_clicked();

    // Slot aksi fitur utama parkir
    void on_btnSimpanMasuk_clicked();
    void on_btnProsesKeluar_clicked();
    void on_btnRefresh_clicked();
    void on_btnHapusData_clicked();
    void on_btnCariLinear_clicked();
    void on_btnCariBST_clicked();

private:
    Ui::MainWindow *ui;

    // Instansiasi objek struktur data dari kodemu
    Parkir parkir;
    Stack histori;
    Queue antrian;
    BST bst;

    // Fungsi pembantu internal GUI
    void updateStatusSlotDisplay();
};
#endif // MAINWINDOW_H