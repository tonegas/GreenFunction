/*
To include some functions
*/

#include <vector>
#include <tuple>
#include <string>
#include <iostream>
#include <fstream>

#ifndef UNIPD_MAIN_HEADER_H
#define UNIPD_MAIN_HEADER_H

void print_processing_time(double simulation_time, std::string output_path);
void print_output_data(uint64_t cycle_number, double N, double absolute_t[], std::vector<double> pforce, std::vector<double> rforce, std::vector<double> fforce, double absolute_disp[], std::vector<double> vel, std::vector<double> battery_power, std::string output_path);
void print_speed_limit(uint64_t cycle_number, std::vector<double> speed_limit_dist, std::vector<double> speed_limit_values, std::string output_path);

#endif