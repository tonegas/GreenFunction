//
// Created by Gastone Pietro on 2024-02-16
//
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <thread>
#include "codriver_interfaces_data_structs.h"
#include "green_function_generated.h"
#include "zmq_recv_queue.hpp"
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
   void *publisher = zmq_socket(context, ZMQ_REQ);

   // Set LINGER to 0 to avoid blocking when closing the socket
   int linger = 0;
   zmq_setsockopt(publisher, ZMQ_LINGER, &linger, sizeof(linger));
   zmq_setsockopt(publisher, ZMQ_SUBSCRIBE, "GreenStructOut", 14);

   // Bind to port
   int rc = zmq_bind(publisher, "tcp://127.0.0.1:5555");
   assert(rc == 0);

   // Wait for 1 s to avoid the slow joiner problem
   std::this_thread::sleep_for(std::chrono::seconds(1));

   // Create the queue
   ZMQRecvQueue subscriber_queue(publisher, 1);
   // Start the queue
   subscriber_queue.start();
   // Create an empty ZMQMessage
   std::shared_ptr<ZMQMessage> msg;

   // Handle SIGINT
   signal(SIGINT, [](int) { running = 0; });

   // Define a topic
   std::string topic_receiver = "";
   std::string topic_sender = "GreenStructIn";

   // Received Data
   // Packet Identifier. Must be related to the packet GreenStructOut
   uint64_t  cycle_number;
   // Means system up-time second [s]
   double ecu_up_time;
   // ECU up time when the primitive starts (based on ECUs given by Scenario Messages)
   double t0;
   // Vector of times for the velocity profile
   std::vector<double> velocity_profile_times;
   // Vector of velocity profile
   std::vector<double> velocity_profile_values;
   // Cost of the manoeuvre
   double cost_manoeuvre;

   // parse scenario file
   while (running && fread(scenario_msg.data_buffer, sizeof(scenario_msg.data_buffer), 1, fp_scenario) > 0) {
      std::cout << "sent -> cycle_number:" << scenario_msg.data_struct.CycleNumber << std::endl;
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

      // Publish data, first the topic_sender and then the message
      int err1 = zmq_send(publisher, topic_sender.c_str(), topic_sender.size(), ZMQ_SNDMORE);
      int err2 = zmq_send(publisher, builder.GetBufferPointer(), builder.GetSize(), 0);
      if(err1 == -1 or err2 == -1){
         std::cout << "Error!!!" << std::strerror(errno) << std::endl;
      }else{
         std::cout << "Invio!"<< std::endl;
      }

      // End time
      auto end = std::chrono::high_resolution_clock::now();

      // Wait for 10 ms
      //std::this_thread::sleep_for(std::chrono::milliseconds(10) - (end - start));
      /* IVIO TRAMITE ZMQ */

      /* RICEZIONE VIA ZMQ */
      bool ok = false;
      std::cout << "Receiving data..." << std::endl;
      int received = 0;
      while (received == 0) {
         // Start time
         auto start = std::chrono::high_resolution_clock::now();

         // TEST //
         int more = 0;
         size_t more_size = sizeof (more);
         zmq_msg_t received_topic;
         int rc = zmq_msg_init (&received_topic);
         assert (rc == 0);
         rc = zmq_msg_recv (&received_topic, publisher, 0);
         assert (rc != -1);
         rc = zmq_getsockopt (publisher, ZMQ_RCVMORE, &more, &more_size);
         assert (rc == 0); assert(more == 1);
         std::cout << "Topic!" << std::endl;

         zmq_msg_t received_data;
         rc = zmq_msg_init (&received_data);
         assert (rc == 0);
         rc = zmq_msg_recv (&received_data, publisher, 0);
         assert (rc != -1);
         rc = zmq_getsockopt (publisher, ZMQ_RCVMORE, &more, &more_size);
         assert (rc == 0); assert(more == 0);
         std::cout << "Data" << std::endl;

         // Print topic
         topic_receiver = std::string(
               static_cast<char *>(zmq_msg_data(msg->at(0))), zmq_msg_size(msg->at(0))
         );
         std::cout << "Topic: " << topic_receiver << std::endl;

         if (topic_receiver.compare("GreenStructOut") == 0) {
            // Fill in the flatbuffer
            auto data = flatbuffers::GetRoot<IGreen>(zmq_msg_data(msg->at(1)));

            cycle_number = data->cycle_number();
            std::cout << "receive <- cycle_number: " << cycle_number << std::endl;
            ecu_up_time = data->out()->ecu_up_time();
            t0 = data->out()->t0();
            auto velocity_profile_times_aux = data->out()->velocity_profile_times();
            auto velocity_profile_values_aux = data->out()->velocity_profile_values();
            velocity_profile_times = std::vector<double>(velocity_profile_times_aux->begin(),
                                                         velocity_profile_times_aux->end());
            velocity_profile_values = std::vector<double>(velocity_profile_values_aux->begin(),
                                                          velocity_profile_values_aux->end());
            cost_manoeuvre = data->out()->cost_manoeuvre();
         }
         received = 1;
      }

      /* COMPILAZIONE INTERFACCE */
    }

    return 0;
}