#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , parkir(10) // Set kapasitas slot parkir = 10
{
    ui->setupUi(this);

    // Konfigurasi awal kolom tabel utama di Halaman 1
    ui->tableParkir->setColumnCount(5);
    ui->tableParkir->setHorizontalHeaderLabels({"Tiket", "Plat Nomor", "Waktu Masuk", "Waktu Keluar", "Status"});

    // Melebarkan kolom secara otomatis agar rapi memenuhi layar
    ui->tableParkir->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Set halaman pertama (Indeks 0) saat aplikasi pertama dibuka
    ui->stackedWidget->setCurrentIndex(0);

    // Tampilkan kapasitas slot pertama kali
    on_btnRefresh_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ====================================== 1. NAVIGASI 2 HALAMAN (STACKED WIDGET) ======================================

void MainWindow::on_btnMenuHalaman1_clicked() {
    ui->stackedWidget->setCurrentIndex(0); // Pindah ke Hal 1 (Pendaftaran & Data)
    on_btnRefresh_clicked();               // Otomatis segarkan isi tabel
}

void MainWindow::on_btnMenuHalaman2_clicked() {
    ui->stackedWidget->setCurrentIndex(1); // Pindah ke Hal 2 (Proses Keluar)
}

// ====================================== 2. KENDARAAN MASUK (HALAMAN 1) ======================================

void MainWindow::on_btnSimpanMasuk_clicked()
{
    string plat = ui->inputPlatMasuk->text().trimmed().toUpper().toStdString();
    if(plat.empty()) {
        QMessageBox::warning(this, "Validasi", "Masukkan Plat Nomor Terlebih Dahulu!");
        return;
    }

    if (parkir.cekPlatSudahAda(plat)) {
        QMessageBox::warning(this, "Error", "Kendaraan dengan plat tersebut sudah terparkir!");
        return;
    }

    string waktuSekarang = parkir.getWaktuSekarang();
    string kodeDihasilkan;
    int slotDihasilkan;

    if (parkir.insertKendaraan(plat, waktuSekarang, kodeDihasilkan, slotDihasilkan)) {
        Node* head = parkir.getHead();
        bst.insert(head->data); // Simpan ke pohon BST untuk pencarian cepat

        QMessageBox::information(this, "Sukses Masuk",
                                 QString("Kendaraan Berhasil Parkir!\n\nKode Tiket: %1\nSlot: M%2\nJam: %3")
                                     .arg(QString::fromStdString(kodeDihasilkan))
                                     .arg(slotDihasilkan)
                                     .arg(QString::fromStdString(waktuSekarang)));
    } else {
        antrian.enqueue(plat); // Masuk antrian otomatis (Queue FIFO) jika penuh
        QMessageBox::information(this, "Antrian", "Parkiran Penuh! Kendaraan otomatis masuk daftar Antrian (Queue).");
    }

    ui->inputPlatMasuk->clear();
    on_btnRefresh_clicked();
}

// ====================================== 3. KENDARAAN KELUAR (HALAMAN 2) ======================================

void MainWindow::on_btnProsesKeluar_clicked()
{
    string kode = ui->inputTiketKeluar->text().trimmed().toStdString();
    string plat = ui->inputPlatKeluar->text().trimmed().toUpper().toStdString();

    if(kode.empty() || plat.empty()) {
        QMessageBox::warning(this, "Input Kurang", "Isi Kode Tiket dan Plat Nomor kendaraan dengan lengkap.");
        return;
    }

    string waktuKeluar;
    if (parkir.keluarKendaraan(kode, plat, waktuKeluar)) {
        Node* found = parkir.searchKendaraan(kode);
        if (found != nullptr) {
            histori.push(found->data); // Masukkan ke Stack histori
        }

        QMessageBox::information(this, "Sukses Keluar", QString("Kendaraan dengan Tiket %1 Berhasil Keluar.").arg(QString::fromStdString(kode)));

        // Tarik kendaraan terdepan di antrian jika ada slot kosong baru (Queue Dequeue)
        if (!antrian.isEmpty()) {
            Kendaraan dariAntrian = antrian.dequeue();
            string waktuMasukAntrian = parkir.getWaktuSekarang();
            string kBaru; int sBaru;
            if (parkir.insertKendaraan(dariAntrian.platNomor, waktuMasukAntrian, kBaru, sBaru)) {
                bst.insert(parkir.getHead()->data);
                QMessageBox::information(this, "Antrian Masuk",
                                         QString("Slot Kosong Terisi! Kendaraan antrian [%1] otomatis masuk ke Slot M%2")
                                             .arg(QString::fromStdString(dariAntrian.platNomor)).arg(sBaru));
            }
        }

        ui->inputTiketKeluar->clear();
        ui->inputPlatKeluar->clear();
    } else {
        QMessageBox::critical(this, "Gagal Keluar", "Kode Tiket tidak ditemukan atau Plat Nomor tidak cocok!");
    }
}

// ====================================== 4. MANIPULASI DATA (HALAMAN 1) ======================================

void MainWindow::on_btnRefresh_clicked()
{
    vector<Kendaraan> data = parkir.getAllKendaraan();

    // Urutkan data secara kronologis sebelum ditampilkan menggunakan Merge Sort bawaan kodemu
    if(!data.empty()) {
        mergeSort(data, 0, data.size() - 1);
    }

    ui->tableParkir->setRowCount(0);
    for(const auto& k : data) {
        int r = ui->tableParkir->rowCount();
        ui->tableParkir->insertRow(r);
        ui->tableParkir->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(k.kodeTiket)));
        ui->tableParkir->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(k.platNomor)));
        ui->tableParkir->setItem(r, 2, new QTableWidgetItem(QString::fromStdString(k.waktuMasuk)));
        ui->tableParkir->setItem(r, 3, new QTableWidgetItem(QString::fromStdString(k.waktuKeluar)));
        ui->tableParkir->setItem(r, 4, new QTableWidgetItem(k.status ? "MASIH PARKIR" : "SUDAH KELUAR"));
    }
    updateStatusSlotDisplay();
}

void MainWindow::on_btnHapusData_clicked()
{
    string kode = ui->inputCariTiket->text().trimmed().toStdString();
    if(kode.empty()) {
        QMessageBox::warning(this, "Input Kosong", "Masukkan kode tiket di kolom input pencarian terlebih dahulu.");
        return;
    }

    if(parkir.hapusKendaraan(kode)) {
        QMessageBox::information(this, "Sukses", "Data tiket berhasil dihapus permanen.");
        on_btnRefresh_clicked();
        ui->inputCariTiket->clear();
    } else {
        QMessageBox::critical(this, "Gagal", "Kode tiket tidak ditemukan.");
    }
}

void MainWindow::on_btnCariLinear_clicked()
{
    string kode = ui->inputCariTiket->text().trimmed().toStdString();
    Node* result = parkir.searchKendaraan(kode); // Menggunakan Linear Search Linked List

    if(result != nullptr) {
        QMessageBox::information(this, "Hasil Linear Search",
                                 QString("Ditemukan!\n\nPlat Nomor: %1\nLokasi Slot: M%2\nStatus: %3")
                                     .arg(QString::fromStdString(result->data.platNomor))
                                     .arg(result->data.lokasiSlot)
                                     .arg(result->data.status ? "Masih Parkir" : "Sudah Keluar"));
    } else {
        QMessageBox::warning(this, "Hasil", "Data tidak ditemukan via Linear Search.");
    }
}

void MainWindow::on_btnCariBST_clicked()
{
    string kode = ui->inputCariTiket->text().trimmed().toStdString();
    Kendaraan* res = bst.search(kode); // Menggunakan Binary Search Tree Rekursif

    if(res != nullptr) {
        QMessageBox::information(this, "Hasil BST Search",
                                 QString("Ditemukan di Node Pohon BST!\n\nPlat Nomor: %1\nSlot Parkir: M%2\nJam Masuk: %3")
                                     .arg(QString::fromStdString(res->platNomor))
                                     .arg(res->lokasiSlot)
                                     .arg(QString::fromStdString(res->waktuMasuk)));
    } else {
        QMessageBox::warning(this, "Hasil", "Data tidak ditemukan di struktur Tree BST.");
    }
}

void MainWindow::updateStatusSlotDisplay()
{
    int sKosong = parkir.getKapasitas() - parkir.getJumlahKendaraan();
    ui->labelStatusSlot->setText(QString("Ketersediaan Tempat: %1 / %2 Slot").arg(sKosong).arg(parkir.getKapasitas()));
}