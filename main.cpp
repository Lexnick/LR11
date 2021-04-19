#include <thread>
#include <iostream>
#include <ctime>
#include <vector>
#include <mutex>

std::mutex Mytex;


void GetMin(std::vector<int> &Vector, int &Begin, int &End, int Number, int &Out) {
    int Min = Vector[Begin];
    for (int i = Begin; i <= End; i++)
        if (Vector[i] < Min)
            Min = Vector[i];
    Mytex.lock();
    std::cout << "Thread's number: " << Number << std::endl;
    std::cout << "Min = " << Min << std::endl;
    Mytex.unlock();
    Out = Min;
}

int main() {
    using namespace std::literals;
    std::srand(std::time(nullptr));
    int VecSize;
    std::cout << "Enter the vector's size" << std::endl;
    std::cin >> VecSize;
    std::vector<int> Vector(VecSize);
    for (auto &i:Vector)
        i = rand();
    int NumTreads;
    std::cout << "Enter the number of treads" << std::endl;
    std::cin >> NumTreads;

    const std::chrono::time_point<std::chrono::steady_clock> start =
            std::chrono::steady_clock::now();

    int *ThreadSize = new int[NumTreads];
    std::thread **Thread = new std::thread *[NumTreads - 1];
    int *Extremums = new int[NumTreads];
    for (int i = 0; i < NumTreads; i++) {
        if (VecSize % NumTreads != 0) {
            ThreadSize[i] = static_cast<int>(VecSize / NumTreads) + 1;
            VecSize -= 1;
        } else ThreadSize[i] = VecSize / NumTreads;
    }

    int Count = 0;
    for (int i = 0; i < NumTreads; i++) {
        ThreadSize[i] = ThreadSize[i] + Count - 1;
        Count = ThreadSize[i] + 1;
    }
    int Begin = 0;

    for (int i = 0; i < NumTreads - 1; i++) {
        Thread[i] = new std::thread(GetMin, std::ref(Vector), std::ref(Begin), std::ref(ThreadSize[i]), i,
                                    std::ref(Extremums[i]));
        Begin = ThreadSize[i] + 1;
    }
    GetMin(Vector, Begin, ThreadSize[NumTreads - 1], NumTreads - 1, Extremums[NumTreads - 1]);
    for (int i = 0; i < NumTreads - 1; i++) {
        Thread[i]->join();
        delete Thread[i];
    }

    int Extremum = Extremums[0];
    for (int i = 0; i < NumTreads; i++)
        if (Extremums[i] < Extremum)
            Extremum = Extremums[i];
    std::cout << "Extremum among thread minimums = " << Extremum << std::endl;
    const auto end = std::chrono::steady_clock::now();
    std::cout
            << "Calculations took "
            << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "µs ≈ "
            << (end - start) / 1ms << "ms ≈ "
            << (end - start) / 1s << "s.\n";
    //the smallest time was with 4 threads(VecSize=1000000)
    delete[] Extremums;
    delete[] Thread;
    delete[] ThreadSize;
    return 0;
}
