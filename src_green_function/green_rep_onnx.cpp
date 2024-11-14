#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <thread>

#include "onnxruntime_cxx_api.h"
#include "green_function_generated.h"
#include "zmq.h"

#define REQ_REP 0

using namespace GreenFunction;
int running = 1;

// Main function
int main(int argc, const char* argv[]) {

#if REQ_REP
   // Create a ZMQ context
   void *context = zmq_ctx_new();
   // Create the subscriber socket
   void *subscriber = zmq_socket(context, ZMQ_REP);
   // Set LINGER to 0 to avoid blocking when closing the socket
   int linger = 0;
   zmq_setsockopt(subscriber, ZMQ_LINGER, &linger, sizeof(linger));
   // Connect to the endpoint
   std::string zmq_address;
   if(argc >= 3){
      zmq_address = argv[2];
   }else{
      zmq_address = "tcp://localhost:5555";
   }
   std::cout << "Subscriber address: " << zmq_address << std::endl;
   int rc = zmq_connect(subscriber, zmq_address.c_str());
   assert(rc == 0);
   // Wait for 1 s to avoid the slow joiner problem
   std::this_thread::sleep_for(std::chrono::seconds(1));
#else
   // Create a ZMQ context
   void *context = zmq_ctx_new();
   void *subscriber = zmq_socket(context, ZMQ_SUB);
   zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "", 0);
   int trueValue = 1;
   zmq_setsockopt(subscriber, ZMQ_CONFLATE, &trueValue, sizeof(int));
   // Create the publisher to send the data to the broker
   std::string zmq_address_sub;
   if(argc >= 3){
      zmq_address_sub = argv[2];
   }else{
      zmq_address_sub = "tcp://10.196.37.63:5560";
   }
   std::cout << "Subscriber address: " << zmq_address_sub << std::endl;
   int rc = zmq_connect(subscriber, zmq_address_sub.c_str()); // add the address to the CO-DRIVER!
   assert(rc == 0);

   // Wait for 1 s to avoid the slow joiner problem
   std::this_thread::sleep_for(std::chrono::seconds(1));

   void *publisher = zmq_socket(context, ZMQ_PUB);
   // Set LINGER to 0 to avoid blocking when closing the socket
   int linger = 0;
   zmq_setsockopt(publisher, ZMQ_LINGER, &linger, sizeof(linger));
   std::string zmq_address_pub;
   if(argc >= 4){
      zmq_address_pub = argv[3];
   }else{
      zmq_address_pub = "tcp://10.196.37.13:6000";
   }
   std::cout << "Publisher address: " << zmq_address_pub << std::endl;
   //rc = zmq_connect(publisher, zmq_address_pub.c_str()); // add the address OF THE BROKER! "tcp://10.196.16.114:5555" tcp://localhost:6001
   rc = zmq_bind(publisher, zmq_address_pub.c_str());
   std::this_thread::sleep_for(std::chrono::seconds(1));
   assert(rc == 0);
   std::string Topic = "Output_Green_Function";
#endif

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
   // Number of filled element inside AdasisSpeedLimitDist and AdasisSpeedLimitValues
   int32_t adasis_speed_limits_nrs;
   // Vector of distance for the speed limits if is negative means that the
   // vehicle in inside a speed limit [m]
   std::vector<double> adasis_speed_limit_dist;
   // Vector of speedlimit in km [km/h]
   std::vector<double> adasis_speed_limit_values;

   // Load ONNX model
   std::cout << "Load ONNX model.... " ;
   Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test");
   Ort::SessionOptions session_options;
   //Ort::Session session(env, "../onnx_models/saved_model.onnx", session_options);
   std::string onnx_model;
   if (argc >= 2) {
      onnx_model = argv[1];
   } else {
      onnx_model = "../onnx_models/net.onnx";
   }
   std::cout << onnx_model;
   Ort::Session session(env, onnx_model.c_str(), session_options);
   std::cout << " ....ONNX model loaded." << std::endl;

   // Start to Receiving data
   std::cout << "Start to Receiving data..." << std::endl;
   int k = 1; // Relative cycle number

   while (running) {
      // Start time
      auto start = std::chrono::high_resolution_clock::now();

      // Read message data
      zmq_msg_t received_data;
      rc = zmq_msg_init(&received_data);
      assert(rc == 0);
      //flags = 0 blocking and flags = 1 non-blocking NOT TESTED
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
      zmq_msg_close(&received_data);

      std::cout << "received <- cycle_number: " << cycle_number << std::endl;
      // Fix the vector removing the last elements if it is greater than the number of elements
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
      // Adding the element in the origin
      if (adasis_curvature_dist[0] > 0) {
         adasis_curvature_dist.insert(adasis_curvature_dist.begin(), -1.0);
         adasis_curvature_values.insert(adasis_curvature_values.begin(), adasis_curvature_values[0]);
      }
      if (adasis_speed_limit_dist[0] > 0) {
         adasis_speed_limit_dist.insert(adasis_speed_limit_dist.begin(), -1.0);
         adasis_speed_limit_values.insert(adasis_speed_limit_values.begin(), 130);
      }
      // Get maximum distance
      double max_dist = std::max(adasis_curvature_dist[adasis_curvature_dist.size() - 1], adasis_speed_limit_dist[adasis_speed_limit_dist.size() - 1]);
      double sample_dist = 10.0; // [meter]

      std::cout<< "Adasis Speed Limit Dist: ";
      for (int i = 0; i < adasis_speed_limit_dist.size(); i++) {
         std::cout <<"("<<i<<")"<< adasis_speed_limit_dist[i] << " ";
      }
      std::cout<< std::endl;
      std::cout<< "Adasis Speed Limit Values: ";
      for (int i = 0; i < adasis_speed_limit_values.size(); i++) {
         std::cout <<"("<<i<<")"<< adasis_speed_limit_values[i] << " ";
      }
      std::cout<< std::endl;
      std::cout<< "Adasis Curvature Dist: ";
      for (int i = 0; i < adasis_curvature_dist.size(); i++) {
         std::cout <<"("<<i<<")"<< adasis_curvature_dist[i] << " ";
      }
      std::cout<< std::endl;
      std::cout<< "Adasis Curvature Values: ";
      for (int i = 0; i < adasis_curvature_values.size(); i++) {
         std::cout <<"("<<i<<")"<< adasis_curvature_values[i] << " ";
      }
      std::cout<< std::endl;
      std::cout<< "Max Dist: "<< max_dist << std::endl;

      // Compute the legal speed limit
      std::vector<float> legal_speed_limit;
      std::vector<float> vmax_turn;
      float a0 = 4.2;
      float v0 = 14.84;
      int curvature_idx = 0;
      int speed_limit_idx = 0;
      // find the first idx with positive number inside the adasis_curvature_dist and adasis_speed_limit_dist
      while (curvature_idx < adasis_curvature_dist.size() && adasis_curvature_dist[curvature_idx] < 0) {
         curvature_idx++;
      }
      curvature_idx--; //Get the last index with negative number
      while(speed_limit_idx < adasis_speed_limit_dist.size() && adasis_speed_limit_dist[speed_limit_idx] < 0) {
         speed_limit_idx++;
      }
      speed_limit_idx--; //Get the last index with negative number
      // Interpolation of the data for the netwrok
      for (float dist = 0; dist < max_dist; dist += sample_dist) {
         float adasis_curvature_value;
         while (curvature_idx + 1 < adasis_curvature_dist.size() && dist >= adasis_curvature_dist[curvature_idx+1]) {
            curvature_idx++;
         }
         if (curvature_idx + 1 < adasis_curvature_dist.size()) {
            // Linear interpolation
            adasis_curvature_value = adasis_curvature_values[curvature_idx] + (adasis_curvature_values[curvature_idx + 1] - adasis_curvature_values[curvature_idx]) * (dist - adasis_curvature_dist[curvature_idx]) / (adasis_curvature_dist[curvature_idx + 1] - adasis_curvature_dist[curvature_idx]);
         }else{
            adasis_curvature_value = adasis_curvature_values[curvature_idx];
         }
         float adasis_speed_limit_value;
         while (speed_limit_idx + 1 < adasis_speed_limit_dist.size() && dist >= adasis_speed_limit_dist[speed_limit_idx+1]) {
            speed_limit_idx++;
         }
         adasis_speed_limit_value = adasis_speed_limit_values[speed_limit_idx]/3.6;
         float aux = pow(a0, 2) / pow(adasis_curvature_value, 2) / pow(v0, 4) + 0.25;
         float vmax_turn_value = v0 * (pow(pow(aux, 0.5) - 0.5, 0.25));
         std::cout <<"Dist: "<< dist << " Curvature: " << adasis_curvature_value << " Speed Limit: " << adasis_speed_limit_value << " Vmax Turn: " << vmax_turn_value << std::endl;
         vmax_turn.push_back(vmax_turn_value);
         legal_speed_limit.push_back(std::min(adasis_speed_limit_value, vmax_turn_value));
      }
      std::cout<< "Max Turn: ";
      for (int i = 0; i < vmax_turn.size(); i++) {
         std::cout << vmax_turn[i] << " ";
      }
      std::cout<< std::endl;
      std::cout<< "Legal Speed Limit: ";
      for (int i = 0; i < legal_speed_limit.size(); i++) {
         std::cout << legal_speed_limit[i] << " ";
      }
      std::cout<< std::endl;
      std::cout<<"V0: "<< ego_velocity<< std::endl;

      // ONNX Mauro network
      std::cout << "Start ONNX..." << std::endl;
      std::vector<const char*> input_names = {"Vo","VL"};
      // Get ego velocity
      std::vector<float> Vo(1, ego_velocity);
      std::vector<int64_t> Vo_shape = {1, 1};

      // Prepare input and output
      std::vector<float> VL = legal_speed_limit;
      std::vector<int64_t> VL_shape = {1, (int)VL.size(), 1};
      std::vector<const char*> output_names = {"Vg"};
      std::vector<float> Vg(VL.size(), 0.0f);
      std::vector<int64_t> Vg_shape = {1, (int)VL.size(), 1};

      // Create the tensor
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

      // Run the model
      Ort::RunOptions run_options;
      session.Run(run_options, input_names.data(), inputTensors.data(), 2, output_names.data(), outputTensors.data(), 1);

      std::cout << "VL: ";
      for (int i = 0; i < Vg.size(); i++) {
         std::cout << VL[i] << " ";
      }
      std::cout << "\nVg: ";
      for(int i = 0; i < Vg.size(); i++) {
         std::cout << Vg[i] << " ";
      }
      std::cout << "\n...End ONNX" << std::endl;

      // Prepare the output data
      std::cout<<"Prepare the output data..."<<std::endl;

      const int N = 20;
      double t0 = 0;
      double cost_manoeuvre = 0.0; //battery_energy; // Battery energy consumption [kW h]
      double greenassistant_time[N];
      double greenassistant_space[N];
      double greenassistant_velocity[N];

      float dist = 0;
      for(int i = 0; i < N; i++, dist += 10){
         greenassistant_time[i] = 0;
         greenassistant_space[i] = dist;
         if (i < Vg.size()) {
            greenassistant_velocity[i] = Vg[i];
         } else {
            greenassistant_velocity[i] = Vg[Vg.size() - 1];
         }
      }


      // Initialize a FlatBuffer builder
      flatbuffers::FlatBufferBuilder builder;
      // build your message
      auto green_struct_out = GreenStructOut(ecu_up_time, t0, greenassistant_time, greenassistant_space, greenassistant_velocity, cost_manoeuvre);
      auto packet = CreateIGreen(builder, data->cycle_number(), NULL, &green_struct_out);
      builder.Finish(packet);

#if REQ_REP
      // Publish data, first the topic and then the message
      int err2 = zmq_send(subscriber, builder.GetBufferPointer(), builder.GetSize(), 0);
      if (err2 == -1) { std::cout << "Error!!!" << std::strerror(errno) << std::endl; }
      std::cout << "sent -> F:" << data->cycle_number() << std::endl;
#else
      int err2 = zmq_send(publisher, Topic.c_str(), Topic.size(), ZMQ_SNDMORE); // send m
      if (err2 == -1) { std::cout << "Error!!!" << std::strerror(errno) << std::endl; }
      err2 = zmq_send(publisher, builder.GetBufferPointer(), builder.GetSize(), 0);
      if (err2 == -1) { std::cout << "Error!!!" << std::strerror(errno) << std::endl; }
      std::cout << "sent -> F:" << data->cycle_number() << std::endl;
#endif
      // End time
      auto end = std::chrono::high_resolution_clock::now();
      std::cout << "Total time:" << (end - start).count() / 1000000000.0 << std::endl;
   }
#if REQ_REP
   zmq_close(subscriber);
   zmq_ctx_destroy(context);
#else
   zmq_close(subscriber);
   zmq_close(publisher);
   zmq_ctx_destroy(context);
#endif

}