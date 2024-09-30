#include "onnx/include/UNIPD_main_header.h"// Green function header file

void print_output_data(uint64_t cycle_number, double N, double absolute_t[], std::vector<double> pforce, std::vector<double> rforce, std::vector<double> fforce, double absolute_disp[], std::vector<double> vel, std::vector<double> battery_power, std::string output_path) {

	// Print general results in file "initial_time_output_data.txt"
	std::ofstream file(output_path + std::to_string(cycle_number) + "_output_data.txt"); // Associate file to output_data.txt

	if (file.is_open()) { // If the file is open
		file << "absolute_t\t" << "absolute_disp\t" << "vel\t" << "pforce\t" << "rforce\t" << "fforce\t" << "battery_power\t" << "\n"; // Write the headers
		for (int i = 0; i < N; ++i) {
			// Write the value of the desired states/controls
			file << absolute_t[i] << "\t" << absolute_disp[i] << "\t" << vel[i] << "\t" << pforce[i] << "\t" << rforce[i] << "\t" << fforce[i] << "\t" << battery_power[i] << "\t";
			file << "\n";
		}

		file.close(); // Close the file

		std::cout << "Data printed successfully in '" + output_path + std::to_string(cycle_number) + "_output_data.txt'." << std::endl;
	}
	else { // i.e. if the file cannot be opened
		std::cerr << "\033[1;31mUnable to open file '" + output_path + std::to_string(cycle_number) + "_output_data.txt'.\033[0m" << std::endl;
	}
}