#ifndef SISTEMPARKIR_H
#define SISTEMPARKIR_H

#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

// ====================================== STRUCT KENDARAAN ======================================
struct Kendaraan {
    string kodeTiket;
    string platNomor;
    string waktuMasuk;
    string waktuKeluar;
    bool status;
    int lokasiSlot;
};

// ====================================== NODE LINKED LIST ======================================
struct Node {
    Kendaraan data;
    Node* next;
    Node(Kendaraan d) {
        data = d;
        next = nullptr;
    }
};

// ====================================== MERGE SORT ======================================
inline void merge(vector<Kendaraan>& arr, int left, int mid, int right) {
    vector<Kendaraan> temp;
    int i = left, j = mid + 1;
    while (i <= mid && j <= right) {
        if (arr[i].waktuMasuk <= arr[j].waktuMasuk) temp.push_back(arr[i++]);
        else temp.push_back(arr[j++]);
    }
    while (i <= mid) temp.push_back(arr[i++]);
    while (j <= right) temp.push_back(arr[j++]);
    for (int idx = left, k = 0; idx <= right; idx++, k++) arr[idx] = temp[k];
}

inline void mergeSort(vector<Kendaraan>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

// ====================================== CLASS PARKIR (LINKED LIST) ======================================
class Parkir {
private:
    Node* head;
    int kapasitas;
    int jumlahKendaraan;
    vector<bool> slotMobil;

public:
    Parkir(int cap = 10) {
        head = nullptr;
        kapasitas = cap;
        jumlahKendaraan = 0;
        slotMobil.resize(cap, false);
    }

    string getWaktuSekarang() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        char buf[80];
        strftime(buf, sizeof(buf), "%H:%M:%S", ltm);
        return string(buf);
    }

    bool cekPlatSudahAda(string plat) {
        Node* current = head;
        while (current != nullptr) {
            if (current->data.platNomor == plat && current->data.status) return true;
            current = current->next;
        }
        return false;
    }

    int cariSlotKosong() {
        for (int i = 0; i < kapasitas; i++) {
            if (!slotMobil[i]) return i;
        }
        return -1;
    }

    bool insertKendaraan(string plat, string waktu, string& outKode, int& outSlot) {
        if (jumlahKendaraan >= kapasitas) return false;
        int slot = cariSlotKosong();
        if (slot == -1) return false;

        outKode = "TKT" + to_string(100 + rand() % 100);
        outSlot = slot + 1;

        Kendaraan k;
        k.kodeTiket = outKode;
        k.platNomor = plat;
        k.waktuMasuk = waktu;
        k.waktuKeluar = "-";
        k.status = true;
        k.lokasiSlot = outSlot;

        Node* newNode = new Node(k);
        newNode->next = head;
        head = newNode;

        slotMobil[slot] = true;
        jumlahKendaraan++;
        return true;
    }

    bool keluarKendaraan(string kode, string platInput, string& outWaktuKeluar) {
        Node* current = head;
        while (current != nullptr) {
            if (current->data.kodeTiket == kode) {
                if (!current->data.status || current->data.platNomor != platInput) return false;

                outWaktuKeluar = getWaktuSekarang();
                current->data.waktuKeluar = outWaktuKeluar;
                current->data.status = false;
                slotMobil[current->data.lokasiSlot - 1] = false;
                jumlahKendaraan--;
                return true;
            }
            current = current->next;
        }
        return false;
    }

    bool hapusKendaraan(string kode) {
        if (head == nullptr) return false;
        if (head->data.kodeTiket == kode) {
            Node* temp = head;
            if (head->data.status) slotMobil[head->data.lokasiSlot - 1] = false;
            head = head->next;
            delete temp;
            return true;
        }
        Node* current = head;
        while (current->next != nullptr) {
            if (current->next->data.kodeTiket == kode) {
                Node* temp = current->next;
                if (temp->data.status) slotMobil[temp->data.lokasiSlot - 1] = false;
                current->next = temp->next;
                delete temp;
                return true;
            }
            current = current->next;
        }
        return false;
    }

    Node* searchKendaraan(string kode) {
        Node* current = head;
        while (current != nullptr) {
            if (current->data.kodeTiket == kode) return current;
            current = current->next;
        }
        return nullptr;
    }

    Node* getHead() { return head; }
    int getJumlahKendaraan() { return jumlahKendaraan; }
    int getKapasitas() { return kapasitas; }

    vector<Kendaraan> getAllKendaraan() {
        vector<Kendaraan> result;
        Node* current = head;
        while (current != nullptr) {
            result.push_back(current->data);
            current = current->next;
        }
        return result;
    }
};

// ====================================== CLASS STACK (HISTORI) ======================================
class Stack {
private:
    struct StackNode {
        Kendaraan data;
        StackNode* next;
        StackNode(Kendaraan d) { data = d; next = nullptr; }
    };
    StackNode* top;

public:
    Stack() { top = nullptr; }
    void push(Kendaraan k) {
        StackNode* newNode = new StackNode(k);
        newNode->next = top;
        top = newNode;
    }
};

// ====================================== CLASS QUEUE (ANTRIAN) ======================================
class Queue {
private:
    struct QNode {
        Kendaraan data;
        QNode* next;
        QNode(Kendaraan d) { data = d; next = nullptr; }
    };
    QNode* front;
    QNode* rear;

public:
    Queue() { front = rear = nullptr; }
    void enqueue(string plat) {
        Kendaraan k;
        k.platNomor = plat;
        k.kodeTiket = "ANTRIAN";
        QNode* newNode = new QNode(k);
        if (rear == nullptr) { front = rear = newNode; }
        else { rear->next = newNode; rear = newNode; }
    }
    Kendaraan dequeue() {
        if (front == nullptr) { Kendaraan empty; empty.platNomor = "EMPTY"; return empty; }
        QNode* temp = front;
        Kendaraan data = front->data;
        front = front->next;
        if (front == nullptr) rear = nullptr;
        delete temp;
        return data;
    }
    bool isEmpty() { return front == nullptr; }
};

// ====================================== CLASS BST ======================================
class BST {
private:
    struct TreeNode {
        Kendaraan data;
        TreeNode *left, *right;
        TreeNode(Kendaraan d) { data = d; left = right = nullptr; }
    };
    TreeNode* root;

    TreeNode* insertRecursive(TreeNode* node, Kendaraan k) {
        if (node == nullptr) return new TreeNode(k);
        if (k.kodeTiket < node->data.kodeTiket) node->left = insertRecursive(node->left, k);
        else if (k.kodeTiket > node->data.kodeTiket) node->right = insertRecursive(node->right, k);
        return node;
    }

    TreeNode* searchRecursive(TreeNode* node, string kode) {
        if (node == nullptr || node->data.kodeTiket == kode) return node;
        if (kode < node->data.kodeTiket) return searchRecursive(node->left, kode);
        return searchRecursive(node->right, kode);
    }

public:
    BST() { root = nullptr; }
    void insert(Kendaraan k) { root = insertRecursive(root, k); }
    Kendaraan* search(string kode) {
        TreeNode* res = searchRecursive(root, kode);
        return (res != nullptr) ? &(res->data) : nullptr;
    }
};

#endif // SISTEMPARKIR_H