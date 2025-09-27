#pragma once

double rectangle(double x, double a, double b);

void print_vector(const std::vector<double> vec);

bool write_vec_to_file(const std::string& filename, const std::vector<double>& data, const std::string& delimiter = " ");
