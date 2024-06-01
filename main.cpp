#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

void task_1(int& total, int a, int b) {
    total += a + b;
}

void task_2(int* ptr_total, int a, int b) {
    *ptr_total += a + b;
}

void ejemplo_1() {
    int total = 0;
    std::jthread t(task_1, std::ref(total), 10, 20);
    t.join();
    std::cout << total << std::endl;
}

void ejemplo_2() {
    int total = 0;
    std::jthread t([&total]() { task_1(total, 10, 20); });
    t.join();
    std::cout << total << std::endl;
}

void ejemplo_3() {
    int total = 0;
    std::jthread t(task_2, &total, 10, 20);
    t.join();
    std::cout << total << std::endl;
}

void ejemplo_contenedor_de_hilos() {
    int n = 3;
    std::vector<std::jthread> vt;
    vt.reserve(n);
    int total = 0;

//    Incorrecto
//    for (int i = 0; i < n; ++i) {
//        vt.emplace_back(task_1, std::ref(total), 1, 1);
//        vt[i].join();
//    }

    for (int i = 0; i < n; ++i)
        vt.emplace_back(task_1, std::ref(total), 1, 1);
    for (int i = 0; i < n; ++i)
        vt[i].join();
}

std::mutex mtx;

void task_incrementar(int& total, int inc) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
//    mtx.lock();
    std::unique_lock ul(mtx);
    total += inc;
//    mtx.unlock();
}

int incrementar_total(int n, int inc) {
    std::vector<std::jthread> vt;
    vt.reserve(n);
    int result = 0;
    for (int i = 0; i < n; ++i) {
        vt.emplace_back(task_incrementar, std::ref(result), inc);
    }
    for (int i = 0; i < n; ++i) {
        vt[i].join();
    }
    return result;
}

int incrementar_total_2(int n, int inc) {
    std::vector<std::jthread> vt;
    vt.reserve(n);
    std::atomic<int> result = 0;
    for (int i = 0; i < n; ++i) {
        vt.emplace_back([&result, &inc]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            result += inc;
        });
    }
    for (int i = 0; i < n; ++i) {
        vt[i].join();
    }
    return result;
}


void generar_totales() {
    int n = 20;     // Numero de hilos
    int m = 100;    // Cantidad de valores
    int inc = 1;    // incremento
    std::vector<int> vtotales;
    vtotales.reserve(m);
    for (int i = 0; i < m; ++i)
//        vtotales.emplace_back(incrementar_total(n, inc));
        vtotales.emplace_back(incrementar_total_2(n, inc));

    for (int i = 0; i < m; ++i)
        std::cout << vtotales[i] << " ";
}

int main() {
//    ejemplo_1();
//    ejemplo_2();
//    ejemplo_3();
//    std::cout << incrementar_total(20, 2) << std::endl;
    generar_totales();
    return 0;
}
