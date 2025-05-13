#include <iostream>
#include <vector>
#include <time.h>

#include "utils.h"

void Lax_Wendroff(std::vector<double>& u_vec, double t_start, double t_end, double h, double tau)
{
    std::vector<double> u_new = u_vec;
    double t_cur = t_start;
    
    while(t_cur < t_end)
    {
        for (size_t i = 1; i < u_vec.size() - 1; ++i)
        {
            u_new[i] = u_vec[i] - (tau / (2 * h)) * (u_vec[i+1] - u_vec[i-1]) + ((0.5 * tau * tau) / (h * h)) * 
                (u_vec[i+1] - 2 * u_vec[i] + u_vec[i-1]); 
        }

        t_cur += tau;
        u_vec = u_new;
    }
}


int main()
{
    const double x_start = 0;
    const double x_end = 10;
    const double t_start = 0;
    const double t_end = 3;
    const double h = 0.0001;
    const double tau = h / 2; // Таким образом схема будет устойчивой

    // Параметры для начального профиля
    const double a = 2;
    const double b = 4;
    
    std::vector<double> x_vec;
    for (double x = x_start; x < x_end; x += h)
        x_vec.push_back(x);

    std::vector<double> u_vec;
    for (auto&& x : x_vec)
        u_vec.push_back(rectangle(x, a, b));

    clock_t start_time = clock(); 
    Lax_Wendroff(u_vec, t_start, t_end, h, tau);
    clock_t end_time = clock();

    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("Время выполнения: %.6f с\n", time_spent);

    write_vec_to_file("u.txt", u_vec);
    write_vec_to_file("x.txt", x_vec);
    return 0;
}
