#include "onnx/include/UNIPD_main_header.h"// Green function header file

void print_speed_limit(uint64_t cycle_number, std::vector<double> speed_limit_dist, std::vector<double> speed_limit_values, std::string output_path) {
	
	// Print the resulting states in file "cycle_number_output_speed_limit.txt"
	std::ofstream file(output_path + std::to_string(cycle_number) + "_output_speed_limit.txt"); // Associate file to cycle_number_output_speed_limit.txt

	if (file.is_open()) { // If the file is open
		file << "Traveled_distance\t" << "Speed_limit\t" << "\n"; // Write the headers
		for (int i = 0; i < speed_limit_dist.size(); i++) {
			// Write the value of the speed limit
			file << speed_limit_dist[i] << "\t" << speed_limit_values[i] << "\t";
			file << "\n";
		}

		file.close(); // Close the file

		std::cout << "Data printed successfully in '" + output_path + std::to_string(cycle_number) + "_output_speed_limit.txt'." << std::endl;
	}
	else { // i.e. if the file cannot be opened
		std::cerr << "\033[1;31mUnable to open file '" + output_path + std::to_string(cycle_number) + "_output_speed_limit.txt'.\033[0m" << std::endl;
	}

	//---------------------------------------------------------------------------------------------
}