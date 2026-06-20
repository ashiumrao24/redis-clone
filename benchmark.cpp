#include <iostream>
#include <chrono>
#include <string>

#include "database.h"

int main()
{
    Database db(2000000);

    const int N = 1000000;

    using Clock =
        std::chrono::high_resolution_clock;

    using Seconds =
        std::chrono::duration<double>;

    auto start =
        Clock::now();

    for(int i = 0; i < N; i++)
    {
        db.set(
            "key" + std::to_string(i),
            "value"
        );
    }

    auto end =
        Clock::now();

    Seconds elapsed =
        end - start;

    std::cout
        << "SET Benchmark\n";

    std::cout
        << "Operations: "
        << N
        << "\n";

    std::cout
        << "Time: "
        << elapsed.count()
        << " sec\n";

    std::cout
        << "Throughput: "
        << static_cast<long long>(
            N / elapsed.count()
        )
        << " ops/sec\n";



    start =
        Clock::now();

    for(int i = 0; i < N; i++)
    {
        db.get(
            "key" + std::to_string(i)
        );
    }

    end =
        Clock::now();

    elapsed =
        end - start;

    std::cout
        << "\nGET Benchmark\n";

    std::cout
        << "Operations: "
        << N
        << "\n";

    std::cout
        << "Time: "
        << elapsed.count()
        << " sec\n";

    std::cout
        << "Throughput: "
        << static_cast<long long>(
            N / elapsed.count()
        )
        << " ops/sec\n";



    start =
        Clock::now();

    for(int i = 0; i < N; i++)
    {
        db.exists(
            "key" + std::to_string(i)
        );
    }

    end =
        Clock::now();

    elapsed =
        end - start;

    std::cout
        << "\nEXISTS Benchmark\n";

    std::cout
        << "Operations: "
        << N
        << "\n";

    std::cout
        << "Time: "
        << elapsed.count()
        << " sec\n";

    std::cout
        << "Throughput: "
        << static_cast<long long>(
            N / elapsed.count()
        )
        << " ops/sec\n";

    return 0;
}