/*

Receive FlatBuffer data over ZMQ, this emulates a slow subscriber to showcase
how to solve the slow subscriber problem.

*/
//*
//#define _CRT_SECURE_NO_WARNINGS // Aggiunta per evitare errore sicurezza di sterror
#include "UNIPD_main_header.h" // Green function header file
//#include "MatlabEngine.hpp"
//#include "MatlabDataArray.hpp"

#include "onnxruntime_cxx_api.h"

#include "green_function_generated.h"
#include "zmq.h"
#include "zmq_recv_queue.hpp"
#include <assert.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <signal.h>
#include <string>
#include <thread>
#include <vector>

using namespace GreenFunction;

int running = 1;

//*/
//#include <acado_optimal_control.hpp>

int main(int argc, const char* argv[]) {

    // Control if there are exactly 2 inputs
    if (argc != 2) {
        std::cout << std::endl << "\033[1;31mWRONG number of arguments!\033[0m" << std::endl;
        return -1;
    }

    //*
   // Create a ZMQ context
   void *context = zmq_ctx_new();

   // Create the subscriber socket
   void *subscriber = zmq_socket(context, ZMQ_REP);
   // Set LINGER to 0 to avoid blocking when closing the socket
   int linger = 0;
   zmq_setsockopt(subscriber, ZMQ_LINGER, &linger, sizeof(linger));

   // Connect to the endpoint
   int rc = zmq_connect(subscriber, "tcp://127.0.0.1:5555");
   assert(rc == 0);

   // Wait for 1 s to avoid the slow joiner problem
   std::this_thread::sleep_for(std::chrono::seconds(1));

   // Handle SIGINT
   signal(SIGINT, [](int) { running = 0; });

   // Received Date
   // Packet Identifier. Must be related to the packet GreenStructIn
   uint64_t cycle_number;
   // Means system up-time second [s]
   double ecu_up_time;
   // Ego velocity [m/s]
   double ego_velocity;
   // Number of filled element inside CurvatureDist and CurvatureValues
   int32_t adasis_curvature_nrs;
   // Vector of distances for the curvatures in meters [m]
   std::vector<double> adasis_curvature_dist;
   // Vector of curvatures (positive for left curves) [1/m]
   std::vector<double> adasis_curvature_values;
   // Width of the lane in meters [m]
   double lane_width;
   // Number of filled element inside AdasisSpeedLimitDist and
   // AdasisSpeedLimitValues
   int32_t adasis_speed_limits_nrs;
   // Vector of distance for the speed limits if is negative means that the
   // vehicle in inside a speed limit [m]
   std::vector<double> adasis_speed_limit_dist;
   // Vector of speedlimit in km [km/h]
   std::vector<double> adasis_speed_limit_values;

   //---------------------------------------------------------------------------------------------
   // Output and resources files paths
   std::string output_path = argv[1]; // Output files path
   //std::string resources_path = argv[2]; // Resources files path

   //---------------------------------------------------------------------------------------------
      // Create/clear "processing_time.txt"
   std::ofstream file(output_path + "processing_time.txt"); // Associate file to "processing_time.txt"

   if (file.is_open()) { // If the file is open
       file.close(); // Close the file
   }
   else {
       std::cerr << "\033[1;31mUnable to open file '" + output_path + "processing_time.txt'.\033[0m" << std::endl;
   }

   // LOAD ONNX
   std::cout << "Load ONNX model" << std::endl;
   Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test");
   Ort::SessionOptions session_options;
   //Ort::Session session(env, "../onnx_models/saved_model.onnx", session_options);
   Ort::Session session(env, "../onnx_models/net.onnx", session_options);
   std::cout << "ONNX model loaded" << std::endl;

   // Receive data
   bool ok = false;
   std::cout << "Receiving data..." << std::endl;

   int k = 1; // Relative cycle number

   while (running) {
      // Start time
      auto start = std::chrono::high_resolution_clock::now();

      // Read message data
      zmq_msg_t received_data;
      rc = zmq_msg_init(&received_data);
      assert(rc == 0);
      rc = zmq_msg_recv(&received_data, subscriber, 0);
      assert(rc != -1);

      // Fill in the flatbuffer
      auto data = flatbuffers::GetRoot<IGreen>(zmq_msg_data(&received_data));

      // Get data
      auto adasis_curvature_dist_aux = data->in()->adasis_curvature_dist();
      auto adasis_curvature_values_aux = data->in()->adasis_curvature_values();
      auto adasis_speed_limit_dist_aux = data->in()->adasis_speed_limit_dist();
      auto adasis_speed_limit_values_aux = data->in()->adasis_speed_limit_values();

      // Data
      cycle_number = data->cycle_number();
      ecu_up_time = data->in()->ecu_up_time();
      ego_velocity = data->in()->ego_velocity();
      adasis_curvature_nrs = data->in()->adasis_curvature_nrs();
      adasis_curvature_dist = std::vector<double>(adasis_curvature_dist_aux->begin(), adasis_curvature_dist_aux->end());
      adasis_curvature_values = std::vector<double>(adasis_curvature_values_aux->begin(), adasis_curvature_values_aux->end());
      lane_width = data->in()->lane_width();
      adasis_speed_limits_nrs = data->in()->adasis_speed_limits_nrs();
      adasis_speed_limit_dist = std::vector<double>(adasis_speed_limit_dist_aux->begin(), adasis_speed_limit_dist_aux->end());
      adasis_speed_limit_values = std::vector<double>(adasis_speed_limit_values_aux->begin(), adasis_speed_limit_values_aux->end());

      std::cout << "received <- cycle_number: " << cycle_number << std::endl;

      // ---------------------- WRITE YOUR CODE HERE ---------------------- 
      //*

      //---------------------------------------------------------------------------------------------
      // Choose the vehicle
      //std::string vehicle_type = resources_path + "vehicle_config_500"; // Choose the 500
      //std::string vehicle_type = resources_path + "vehicle_config_Tesla"; // Choose the Tesla

      //---------------------------------------------------------------------------------------------
      // Initial conditions
      double x0 = 0; // Absolute initial position
      double v0 = ego_velocity; // Initial speed
      double initial_time = ecu_up_time; // Absolute initial time of the current simulation 

      //---------------------------------------------------------------------------------------------
      // Simulation options
      double t_start = 0; // Initial time, relative to simulation
      double t_end = 30; // Final time, relative to simulation
      const int N = 20; // Number of discretization simulation points

      //---------------------------------------------------------------------------------------------
      // Lead vehicle
      double v_lead = 20; // Lead vehicle constant speed
      double s0 = 2000; // Lead vehicle initial spacing from ego-vehicle

      //---------------------------------------------------------------------------------------------
      // Choose eco_weight
      double w = 0.5; // Eco-weight: 0 for only drivermodel, 1 for only consumption minimization

      //---------------------------------------------------------------------------------------------
      // Choose the initial guesses
      //std::string initial_guess_x = "no_initial_guess"; // Select no_initial_guess for auto-initialization, otherwise choose the initial guess file name
      //std::string initial_guess_u = "no_initial_guess"; // Select no_initial_guess for auto-initialization, otherwise choose the initial guess file name

      //---------------------------------------------------------------------------------------------
      // Input files names and paths (actually in resources folder)
      //std::string drivermodel_file_name = resources_path + "driver_model_config.tsv"; // Select the correct driver model file name and path

      //---------------------------------------------------------------------------------------------
      // Read road vectors
      std::vector<double> adasis_lane_width_dist = { 0 };
      std::vector<double> adasis_lane_width_values = { lane_width };

      if(adasis_curvature_dist.size() > adasis_curvature_nrs) {
          adasis_curvature_dist.erase(adasis_curvature_dist.begin() + adasis_curvature_nrs, adasis_curvature_dist.end());
      }

      if (adasis_curvature_values.size() > adasis_curvature_nrs) {
          adasis_curvature_values.erase(adasis_curvature_values.begin() + adasis_curvature_nrs, adasis_curvature_values.end());
      }

      if (adasis_speed_limit_dist.size() > adasis_speed_limits_nrs) {
          adasis_speed_limit_dist.erase(adasis_speed_limit_dist.begin() + adasis_speed_limits_nrs, adasis_speed_limit_dist.end());
      }

      if (adasis_speed_limit_values.size() > adasis_speed_limits_nrs) {
          adasis_speed_limit_values.erase(adasis_speed_limit_values.begin() + adasis_speed_limits_nrs, adasis_speed_limit_values.end());
      }

      if (adasis_curvature_dist[0] > 0) {
          adasis_curvature_dist.insert(adasis_curvature_dist.begin(), 0);
          adasis_curvature_values.insert(adasis_curvature_values.begin(), adasis_curvature_values[0]);
      }

      if (adasis_speed_limit_dist[0] > 0) {
          adasis_speed_limit_dist.insert(adasis_speed_limit_dist.begin(), 0);
          adasis_speed_limit_values.insert(adasis_speed_limit_values.begin(), 40);
      }

      //---------------------------------------------------------------------------------------------
      // Print curvature in file "cycle_number_curvature.txt"
      std::ofstream file_curvature(output_path + std::to_string(cycle_number) + "_curvature.txt"); // Associate file to cycle_number_curvature.txt

      if (file_curvature.is_open()) { // If the file is open
          file_curvature << "dist\t" << "values\t" << "\n"; // Write the headers
          for (int i = 0; i < adasis_curvature_dist.size(); ++i) {
              // Write the value of the desired states/controls
              file_curvature << adasis_curvature_dist[i] << "\t" << adasis_curvature_values[i] << "\t";
              file_curvature << "\n";
          }

          file_curvature.close(); // Close the file

          std::cout << "Data printed successfully in '" + output_path + std::to_string(cycle_number) + "_curvature.txt'." << std::endl;
      }
      else { // i.e. if the file cannot be opened
          std::cerr << "Unable to open file '" + output_path + std::to_string(cycle_number) + "_curvature.txt'." << std::endl;
      }


      //---------------------------------------------------------------------------------------------
      // If speed_limit_value = 0, change it with 25
      for (int i = 0; i < adasis_speed_limit_dist.size(); ++i) {

          if (adasis_speed_limit_values[i] == 0) { adasis_speed_limit_values[i] = 50; }

      }

      //---------------------------------------------------------------------------------------------
      // Print legal_speed_limit in file "cycle_number_legal_speed_limit.txt"
      std::ofstream file_lsl(output_path + std::to_string(cycle_number) + "_legal_speed_limit.txt"); // Associate file to cycle_number_legal_speed_limit.txt

      if (file_lsl.is_open()) { // If the file is open
          file_lsl << "dist\t" << "values\t" << "\n"; // Write the headers
          for (int i = 0; i < adasis_speed_limit_dist.size(); ++i) {
              // Write the value of the desired states/controls
              file_lsl << adasis_speed_limit_dist[i] << "\t" << adasis_speed_limit_values[i] << "\t";
              file_lsl << "\n";
          }

          file_lsl.close(); // Close the file

          std::cout << "Data printed successfully in '" + output_path + std::to_string(cycle_number) + "_legal_speed_limit.txt'." << std::endl;
      }
      else { // i.e. if the file cannot be opened
          std::cerr << "Unable to open file '" + output_path + std::to_string(cycle_number) + "_legal_speed_limit.txt'." << std::endl;
      }

      //---------------------------------------------------------------------------------------------
      // ONNX
//      std::cout << "START ONNX" << std::endl;
//      Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test");
//      Ort::SessionOptions session_options;
//      //Ort::Session session(env, "../onnx_models/saved_model.onnx", session_options);
//      Ort::Session session(env, "../onnx_models/saved_model.onnx", session_options);
//
//      // Create a 3x3 matrix of ones
//      std::vector<float> input_tensor_values(9, 1.0f);
//      std::vector<int64_t> input_tensor_shape = {3, 3};
//      std::vector<float> output_tensor_values(12, 0.0f);
//      std::vector<int64_t> output_tensor_shape = {3, 4};
//
//      // Create input and output names
//      std::vector<const char*> input_names = {"dense_input"};
//      std::vector<const char*> output_names = {"dense"};
//
//      // Create input and output tensors
//      Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
//      Ort::Value input_tensor_onnx = Ort::Value::CreateTensor<float>(memory_info, input_tensor_values.data(), input_tensor_values.size(), input_tensor_shape.data(), input_tensor_shape.size());
//      Ort::Value output_tensors = Ort::Value::CreateTensor<float>(memory_info, output_tensor_values.data(), output_tensor_values.size(), output_tensor_shape.data(), output_tensor_shape.size());
//
//
//
//      // Run inference
//      Ort::RunOptions run_options;
//      session.Run(run_options, input_names.data(), &input_tensor_onnx, 1, output_names.data(), &output_tensors, 1);
//
//      // Get output tensor data
//      std::vector<float> output_data(output_tensor_values);
//
//      // Print output tensor data
//      for (int i = 0; i < output_data.size(); i++) {
//         std::cout << output_data[i] << " ";
//         if ((i + 1) % 4 == 0) {
//            std::cout << std::endl;
//         }
//      }
//      std::cout << "END ONNX" << std::endl;
      //---------------------------------------------------------------------------------------------
      //ONNX Mauro
      std::vector<const char*> input_names = {"Vo","VL"};
      std::vector<float> Vo(1, 20.0f);
      std::vector<int64_t> Vo_shape = {1, 1};
      float example[] = {50.0f,50.0f,50.2f,30.1f,15.0f};
      std::vector<float> VL (example, example + sizeof(example) / sizeof(float) );
      std::vector<int64_t> VL_shape = {1, sizeof(example) / sizeof(float), 1};

      std::vector<const char*> output_names = {"Vg"};
      std::vector<float> Vg(5, 0.0f);
      std::vector<int64_t> Vg_shape = {1, sizeof(example) / sizeof(float), 1};

      Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
      std::vector<Ort::Value> inputTensors;
      inputTensors.push_back(
            Ort::Value::CreateTensor<float>(memory_info, Vo.data(), Vo.size(), Vo_shape.data(), Vo_shape.size())
      );
      inputTensors.push_back(
            Ort::Value::CreateTensor<float>(memory_info, VL.data(), VL.size(), VL_shape.data(), VL_shape.size())
      );
      std::vector<Ort::Value> outputTensors;
      outputTensors.push_back(
            Ort::Value::CreateTensor<float>(memory_info, Vg.data(), Vg.size(), Vg_shape.data(), Vg_shape.size())
      );

      // Run inference
      Ort::RunOptions run_options;
      session.Run(run_options, input_names.data(), inputTensors.data(), 2, output_names.data(), outputTensors.data(), 1);

      // Print output tensor data
      for (int i = 0; i < Vg.size(); i++) {
         std::cout << Vg[i] << " ";
      }
      std::cout << "END ONNX" << std::endl;
      //---------------------------------------------------------------------------------------------

      //---------------------------------------------------------------------------------------------
      // MATLAB:
      // using namespace matlab::engine;

      // Connect to MATLAB engine synchronously
      //std::vector<String> name = findMATLAB();
      //std::unique_ptr<MATLABEngine> matlabPtr = connectMATLAB(name[0]);

      // Create MATLAB data array factory
      //matlab::data::ArrayFactory factory;

//      auto start_rh = std::chrono::high_resolution_clock::now();
      //vehicle_type, drivermodel_file_name, initial_guess_x, initial_guess_u;

      //std::cout << v_lead << std::endl << s0 << std::endl << w << std::endl << alpha_x << std::endl;

      // Converting vectors from std::vector to matlab::data:Array
//      matlab::data::Array curvature_dist_mat = factory.createArray<double>({ adasis_curvature_dist.size(), 1 }, adasis_curvature_dist.data(), adasis_curvature_dist.data() + adasis_curvature_dist.size());
//      matlab::data::Array curvature_values_mat = factory.createArray<double>({ adasis_curvature_values.size(), 1 }, adasis_curvature_values.data(), adasis_curvature_values.data() + adasis_curvature_values.size());
//      matlab::data::Array speed_limit_dist_mat = factory.createArray<double>({ adasis_speed_limit_dist.size(), 1 }, adasis_speed_limit_dist.data(), adasis_speed_limit_dist.data() + adasis_speed_limit_dist.size());
//      matlab::data::Array speed_limit_values_mat = factory.createArray<double>({ adasis_speed_limit_values.size(), 1 }, adasis_speed_limit_values.data(), adasis_speed_limit_values.data() + adasis_speed_limit_values.size());
//      matlab::data::Array lane_width_dist_mat = factory.createArray<double>({ adasis_lane_width_dist.size(), 1 }, adasis_lane_width_dist.data(), adasis_lane_width_dist.data() + adasis_lane_width_dist.size());
//      matlab::data::Array lane_width_values_mat = factory.createArray<double>({ adasis_lane_width_values.size(), 1 }, adasis_lane_width_values.data(), adasis_lane_width_values.data() + adasis_lane_width_values.size());
//      matlab::data::Array N_mat = factory.createScalar<int>(N);
//      matlab::data::Array t_start_mat = factory.createScalar<double>(t_start);
//      matlab::data::Array t_end_mat = factory.createScalar<double>(t_end);
//      matlab::data::Array v0_mat = factory.createScalar<double>(v0);
//      matlab::data::Array v_lead_mat = factory.createScalar<double>(v_lead);
//      matlab::data::Array s0_mat = factory.createScalar<double>(s0);
//      matlab::data::Array w_mat = factory.createScalar<double>(w);
//      matlab::data::Array x0_mat = factory.createScalar<double>(x0);
//      matlab::data::Array k_mat = factory.createScalar<int>(k);

      // Define the arguments "args" of the matlab function   
//      std::vector<matlab::data::Array> args({ curvature_dist_mat, curvature_values_mat,
//                                              speed_limit_dist_mat, speed_limit_values_mat,
//                                              lane_width_dist_mat, lane_width_values_mat,
//                                              N_mat, t_start_mat, t_end_mat,
//                                              v0_mat, v_lead_mat, s0_mat,
//                                              w_mat, x0_mat, k_mat });

//       Call MATLAB function and return result
//      const size_t numReturned = 14;

      //---------------------------------------------------------------------------------------------
//      std::cout << "START SIMULATION" << std::endl;
//      std::vector<matlab::data::Array> result = matlabPtr->feval(u"gpops2_reciding_horizon", numReturned, args);
//      std::cout << "END SIMULATION" << std::endl;
      //---------------------------------------------------------------------------------------------

      // Convert MATLAB data array to MATLAB data typedarray
//      matlab::data::TypedArray<double> output0 = result[0];
//      matlab::data::TypedArray<double> output1 = result[1];
//      matlab::data::TypedArray<double> output2 = result[2];
//      matlab::data::TypedArray<double> output3 = result[3];
//      matlab::data::TypedArray<double> output4 = result[4];
//      matlab::data::TypedArray<double> output5 = result[5];
//      matlab::data::TypedArray<double> output6 = result[6];
//      matlab::data::TypedArray<double> output7 = result[7];
//      matlab::data::TypedArray<double> output8 = result[8];
//      matlab::data::TypedArray<double> output9 = result[9];
//      matlab::data::TypedArray<double> output10 = result[10];
//      matlab::data::TypedArray<double> output11 = result[11];
//      matlab::data::TypedArray<double> output12 = result[12];

      // Convert MATLAB data typedarray to std::vector<double>
//      std::vector<double> relative_t(output0.begin(), output0.end());
//      std::vector<double> relative_disp(output1.begin(), output1.end());
//      std::vector<double> speed(output2.begin(), output2.end());
//      std::vector<double> space(output3.begin(), output3.end());
//      std::vector<double> Fp(output4.begin(), output4.end());
//      std::vector<double> Fr(output5.begin(), output5.end());
//      std::vector<double> Ff(output6.begin(), output6.end());
//      std::vector<double> relax_a(output7.begin(), output7.end());
//      std::vector<double> relax_v(output8.begin(), output8.end());
//      std::vector<double> battery_power(output9.begin(), output9.end());
//      std::vector<double> speed_limit_dist(output10.begin(), output10.end());
//      std::vector<double> speed_limit_values(output11.begin(), output11.end());
//      std::vector<double> vd(output12.begin(), output12.end());
//      double battery_energy = result[13][0];

//      auto end_rh = std::chrono::high_resolution_clock::now();
//      double simulation_time = (end_rh - start_rh).count() / 1000000000.0;

      /*
      //---------------------------------------------------------------------------------------------
      // Print vd for debug
      for (int i = 0; i < N; i++) {
          std::cout << vd[i] << std::endl;
      }
      */

      //---------------------------------------------------------------------------------------------
      // Compute absolute time absolute_t and absolute traveled space absolute_disp
      double absolute_t[N] = { 0 };
      double absolute_disp[N] = { 0 };

//      for (int i = 0; i < N; i++) {
//          absolute_t[i] = relative_t[i];
//          absolute_disp[i] = relative_disp[i];
//      }

      //---------------------------------------------------------------------------------------------
      // Compute the absolute space of speed_limit_dist
//      for (int i = 0; i < speed_limit_dist.size(); i++) {
//          speed_limit_dist[i] = speed_limit_dist[i] + x0;
//      }

      //---------------------------------------------------------------------------------------------
      // Print speed limit in "cycle_number_output_speed_limit.txt"
//      print_speed_limit(cycle_number, speed_limit_dist, speed_limit_values, output_path);

      //---------------------------------------------------------------------------------------------
      // Print processing time in file "processing_time.txt"
//      print_processing_time(simulation_time, output_path);

      //---------------------------------------------------------------------------------------------
      // Print general results in file "initial_time_output_data.txt"
//      print_output_data(cycle_number, N, absolute_t, Fp, Fr, Ff, absolute_disp, speed, battery_power, output_path);

      //---------------------------------------------------------------------------------------------
      // Convert output vectors from dynamic vectors to static vectors
      
      double greenassistant_time[N]; 
      double greenassistant_space[N];
      double greenassistant_velocity[N];
   
//      for (int i = 0; i < N; i++) {
//          greenassistant_time[i] = relative_t[i];
//          greenassistant_space[i] = relative_disp[i];
//          greenassistant_velocity[i] = speed[i];
//      }
     
      // Data for the output
      double t0 = 0;
      double cost_manoeuvre = 0.0; //battery_energy; // Battery energy consumption [kW h]

      // Update relative cycle number
      k = k + 1;

      // ---------------------- WRITE YOUR CODE HERE ---------------------- 

      
      // Initialize a FlatBuffer builder
      flatbuffers::FlatBufferBuilder builder;
      // build your message
      auto green_struct_out = GreenStructOut(ecu_up_time, t0, greenassistant_time, greenassistant_space, greenassistant_velocity, cost_manoeuvre);
      auto packet = CreateIGreen(builder, data->cycle_number(), NULL, &green_struct_out);
      builder.Finish(packet);

      // Publish data, first the topic and then the message
      int err2 = zmq_send(subscriber, builder.GetBufferPointer(), builder.GetSize(), 0);
      if (err2 == -1) { std::cout << "Error!!!" << std::strerror(errno) << std::endl; }
      std::cout << "sent -> F:" << data->cycle_number() << std::endl;
      zmq_msg_close(&received_data);

      // End time
      auto end = std::chrono::high_resolution_clock::now();
      std::cout << "Total time:" << (end - start).count() / 1000000000.0 << std::endl;
   }

   // Close the socket
   zmq_close(subscriber);

   // Destroy the context
   zmq_ctx_destroy(context);
   //*/
   return 0;
}
