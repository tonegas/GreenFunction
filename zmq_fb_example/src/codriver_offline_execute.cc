//
// Created by Gastone Pietro on 2024-02-16
//
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <thread>
#include "codriver_interfaces_data_structs.h"
#include "green_function_generated.h"
#include "zmq.h"

using namespace GreenFunction;

int running = 1;

int main(int argc, const char* argv[]) {
   FILE *fp_scenario;
   if(argc == 2) {
      fp_scenario = fopen(argv[1], "rb");
      if (fp_scenario == NULL) {
         std::cout << "WRONG file" << std::endl;
         return -1;
      }
   }

   // Structu of the scenario
   scenario_msg_t  scenario_msg;

   // Create ZMQ context
   void *context = zmq_ctx_new();

   // Create the publisher socket
   void *publisher = zmq_socket(context, ZMQ_PUB);

   // Set LINGER to 0 to avoid blocking when closing the socket
   int linger = 0;
   zmq_setsockopt(publisher, ZMQ_LINGER, &linger, sizeof(linger));

   // Bind to port
   int rc = zmq_bind(publisher, "tcp://127.0.0.1:5555");
   assert(rc == 0);

   // Handle SIGINT
   signal(SIGINT, [](int) { running = 0; });

   // Wait for 1 s to avoid the slow joiner problem
   std::this_thread::sleep_for(std::chrono::seconds(1));

   // Define a topic
   std::string topic = "GreenStructIn";

   // parse scenario file
   while (running && fread(scenario_msg.data_buffer, sizeof(scenario_msg.data_buffer), 1, fp_scenario) > 0) {
      std::cout << scenario_msg.data_struct.CycleNumber << std::endl;
      // Start time
      auto start = std::chrono::high_resolution_clock::now();

      // Initialize a FlatBuffer builder
      flatbuffers::FlatBufferBuilder builder;

      auto green_struct = GreenStructIn(
            scenario_msg.data_struct.ECUupTime,
            scenario_msg.data_struct.AdasisCurvatureNrs[0],
            scenario_msg.data_struct.AdasisCurvatureDist,
            scenario_msg.data_struct.AdasisCurvatureValues,
            scenario_msg.data_struct.LaneWidth,
            scenario_msg.data_struct.AdasisSpeedLimitsNrs[0],
            scenario_msg.data_struct.AdasisSpeedLimitDist,
            scenario_msg.data_struct.AdasisSpeedLimitValues
      );

      auto packet = CreateIGreen(builder, scenario_msg.data_struct.CycleNumber, &green_struct);
      builder.Finish(packet);

      // Publish data, first the topic and then the message
      zmq_send(publisher, topic.c_str(), topic.size(), ZMQ_SNDMORE);
      zmq_send(publisher, builder.GetBufferPointer(), builder.GetSize(), 0);

      // End time
      auto end = std::chrono::high_resolution_clock::now();

      // Wait for 10 ms
      std::this_thread::sleep_for(std::chrono::milliseconds(10) - (end - start));
        /* IVIO TRAMITE ZMQ */

        /* RICEZIONE VIA ZMQ */

        /* COMPILAZIONE INTERFACCE */
    }

    return 0;
}