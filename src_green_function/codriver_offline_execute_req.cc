//
// Created by Gastone Pietro on 2024-02-16
//
#include "codriver_interfaces_data_structs.h"
#include "green_function_generated.h"
#include "zmq.h"
#include "zmq_recv_queue.hpp"
#include <chrono>
#include <iostream>
#include <stdio.h>
#include <thread>

using namespace GreenFunction;

int running = 1;

#define REQ_REP 0

int main(int argc, const char *argv[]) {
   FILE *fp_scenario;
   if (argc == 2) {
      fp_scenario = fopen(argv[1], "rb");
      if (fp_scenario == NULL) {
         std::cout << "WRONG file" << std::endl;
         return -1;
      }
   }

   // Structu of the scenario
   scenario_msg_t scenario_msg;

#if REQ_REP
   // Create ZMQ context
   void *context = zmq_ctx_new();
   // Create the publisher socket
   void *publisher = zmq_socket(context, ZMQ_REQ);
   // Set LINGER to 0 to avoid blocking when closing the socket
   int linger = 0;
   zmq_setsockopt(publisher, ZMQ_LINGER, &linger, sizeof(linger));
   // Bind to port
   std::string zmq_address_pub;
   if(argc >= 3){
      zmq_address_pub = argv[2];
   }else{
      zmq_address_pub = "tcp://localhost:5555";
   }
   std::cout << "Publisher address: " << zmq_address_pub << "..." << std::endl;
   int rc = zmq_bind(publisher, zmq_address_pub.c_str());
   assert(rc == 0);
#else
   // Create ZMQ context
   void *context = zmq_ctx_new();
   // Create the publisher socket
   void *publisher = zmq_socket(context, ZMQ_PUB);
   // Set LINGER to 0 to avoid blocking when closing the socket
   int linger = 0;
   zmq_setsockopt(publisher, ZMQ_LINGER, &linger, sizeof(linger));
   // Bind to port
   std::string zmq_address_pub;
   if(argc >= 3){
      zmq_address_pub = argv[2];
   }else{
      zmq_address_pub = "tcp://localhost:5560";
   }
   std::cout << "Publisher address: " << zmq_address_pub << "..." << std::endl;
   int rc = zmq_bind(publisher, zmq_address_pub.c_str());
   assert(rc == 0);
   // Create the subscriber socket to recieve from the broker - Green function output
   void *subscriber = zmq_socket(context, ZMQ_SUB);
   zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "Output_Green_Function", 0);
   std::string zmq_address_sub;
   if(argc >= 4){
      zmq_address_sub = argv[3];
   }else{
      zmq_address_sub = "tcp://localhost:6000";
   }
   std::cout << "Subscriber address: " << zmq_address_sub << "..." << std::endl;
   rc = zmq_connect(subscriber, zmq_address_sub.c_str());
   assert(rc == 0);
#endif

   // Wait for 1 s to avoid the slow joiner problem
   std::this_thread::sleep_for(std::chrono::seconds(1));

   // Handle SIGINT
   signal(SIGINT, [](int) { running = 0; });

   // Received Data
   // Packet Identifier. Must be related to the packet GreenStructOut
   uint64_t cycle_number;
   // Means system up-time second [s]
   double ecu_up_time;
   // ECU up time when the primitive starts (based on ECUs given by Scenario
   // Messages)
   double t0;
   // Vector of times for the velocity profile
   std::vector<double> greenassistant_time;
   // Vector of space profile
   std::vector<double> greenassistant_space;
   // Vector of velocity profile
   std::vector<double> greenassistant_velocity;
   // Cost of the manoeuvre
   double cost_manoeuvre;

   // parse scenario file
   while (running && fread(scenario_msg.data_buffer, sizeof(scenario_msg.data_buffer), 1, fp_scenario) > 0) {
      auto start = std::chrono::high_resolution_clock::now();
      std::cout << "cycle_number:" << scenario_msg.data_struct.CycleNumber << std::endl;
      if (scenario_msg.data_struct.CycleNumber == 1 || scenario_msg.data_struct.CycleNumber % 50 == 0) {
         if (scenario_msg.data_struct.CycleNumber > 1) {
            /* RICEZIONE VIA ZMQ */
            std::cout << "Receiving data..." << std::endl;
            int received = 0;
#if REQ_REP == 0
            int counter_message = 0;
#endif
            while (received == 0) {
#if REQ_REP == 0
               if(counter_message == 0) {
                  char buffer[256];
                  int size;
                  size = zmq_recv(subscriber, buffer, 255, 0);
                  assert(size >= 0);
                  std::cout << "Data topic " << buffer << std::endl;
                  counter_message += 1;
                  // Check if more message parts are to be received
                  int more;
                  size_t more_size = sizeof(more);
                  zmq_getsockopt(subscriber, ZMQ_RCVMORE, &more, &more_size);
                  counter_message += 1;
               } else {
#endif
                  // Read message data
                  zmq_msg_t received_data;
                  zmq_msg_init(&received_data);
#if REQ_REP
                  rc = zmq_msg_recv(&received_data, publisher, 0);
#else
                  rc = zmq_msg_recv(&received_data, subscriber, 0);
#endif
                  assert(rc != -1);

                  // Fill in the flatbuffer
                  auto data = flatbuffers::GetRoot<IGreen>(zmq_msg_data(&received_data));

                  cycle_number = data->cycle_number();
                  std::cout << "received <- cycle_number: " << cycle_number << std::endl;
                  ecu_up_time = data->out()->ecu_up_time();
                  t0 = data->out()->t0();
                  auto greenassistant_time_aux = data->out()->greenassistant_time();
                  auto greenassistant_velocity_aux = data->out()->greenassistant_velocity();
                  auto greenassistant_space_aux = data->out()->greenassistant_space();
                  greenassistant_time = std::vector<double>(greenassistant_time_aux->begin(), greenassistant_time_aux->end());
                  greenassistant_velocity = std::vector<double>(greenassistant_velocity_aux->begin(), greenassistant_velocity_aux->end());
                  greenassistant_space = std::vector<double>(greenassistant_space_aux->begin(), greenassistant_space_aux->end());
                  cost_manoeuvre = data->out()->cost_manoeuvre();
                  zmq_msg_close(&received_data);
                  received = 1;
#if REQ_REP == 0
               }
#endif
            }
         }

         // Initialize a FlatBuffer builder
         flatbuffers::FlatBufferBuilder builder;

         auto green_struct = GreenStructIn(scenario_msg.data_struct.ECUupTime, scenario_msg.data_struct.VLgtFild,
                                           scenario_msg.data_struct.AdasisCurvatureNrs[0],
                                           scenario_msg.data_struct.AdasisCurvatureDist, scenario_msg.data_struct.AdasisCurvatureValues,
                                           scenario_msg.data_struct.LaneWidth, scenario_msg.data_struct.AdasisSpeedLimitsNrs[0],
                                           scenario_msg.data_struct.AdasisSpeedLimitDist, scenario_msg.data_struct.AdasisSpeedLimitValues);

         auto packet = CreateIGreen(builder, scenario_msg.data_struct.CycleNumber, &green_struct);
         builder.Finish(packet);

         // Send data
         int err2 = zmq_send(publisher, builder.GetBufferPointer(), builder.GetSize(), 0);
         if (err2 == -1) { std::cout << "Error!!!" << std::strerror(errno) << std::endl; }
         std::cout << "sent -> cycle_number:" << scenario_msg.data_struct.CycleNumber << std::endl;
      }
      // Compute codriver
      std::this_thread::sleep_for(std::chrono::milliseconds(50));

      // End time
      auto end = std::chrono::high_resolution_clock::now();
      std::cout << "Total time:" << (end - start).count() / 1e9 << std::endl;
   }
#if REQ_REP == 0
   zmq_close(subscriber);
#endif
   zmq_close(publisher);
   zmq_ctx_destroy(context);

   return 0;
}