#include "onnx/include/UNIPD_main_header.h"// Green function header file

void print_processing_time(double simulation_time, std::string output_path) {

	// Print processing time in file "processing_time.txt"
	
	// Open "processing_time.txt" in reading mode
	std::ifstream inputFile(output_path + "processing_time.txt");

	// Check if the file is open
	if (!inputFile.is_open()) {
		std::cout << "\033[1;31mUnable to open " + output_path + "processing_time.txt in reading mode.\033[0m" << std::endl;
	}

	std::vector<double> lines;
	std::string line;
	while (getline(inputFile, line)) { // Read the lines of the file in lines vector
		try {
			lines.push_back(stod(line)); // Try to convert the string "line" to a double, to add at the end of vector "lines"
		}
		catch (std::invalid_argument) { // If cannot convert:					
			std::cout << "\033[1;31mUnable to convert " << line << " in double format!\033[0m" << std::endl;
			break;
		}
	}

	// Close the file
	inputFile.close();

	// Add the current processing time at lines vector
	lines.push_back(simulation_time);

	// Open "processing_time.txt" in writing mode
	std::ofstream outputFile(output_path + "processing_time.txt");

	// Check if the file is open
	if (!outputFile.is_open()) {
		std::cout << "\033[1;31mUnable to open " + output_path + "processing_time.txt in writing mode.\033[0m" << std::endl;
	}
	else {
		std::cout << "Data printed successfully in '" + output_path + "processing_time.txt', it took " << simulation_time << " second(s) to find the optimal solution." << std::endl;
	}

	// Write the vector lines in the file
	for (double& newline : lines) {
		outputFile << newline << std::endl;
	}

	// Close the file
	outputFile.close();

}