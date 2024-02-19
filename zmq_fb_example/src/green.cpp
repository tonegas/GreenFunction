/*

Receive FlatBuffer data over ZMQ, this emulates a slow subscriber to showcase
how to solve the slow subscriber problem.

*/

#include "green_function_generated.h"
#include "zmq.h"
#include "zmq_recv_queue.hpp"
#include <assert.h>
#include <chrono>
#include <iostream>
#include <mutex>
#include <queue>
#include <signal.h>
#include <string>
#include <thread>
#include <vector>

using namespace GreenFunction;

int running = 1;

int main()
{
  // Create a ZMQ context
  void *context = zmq_ctx_new();

  // Create the subscriber socket
  void *subscriber = zmq_socket(context, ZMQ_REP);
  // Set LINGER to 0 to avoid blocking when closing the socket
  int linger = 0;
  zmq_setsockopt(subscriber, ZMQ_LINGER, &linger, sizeof(linger));
  zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "GreenStructIn", 13);

  // Connect to the endpoint
  int rc = zmq_connect(subscriber, "tcp://127.0.0.1:5555");
  assert(rc == 0);

  // Wait for 1 s to avoid the slow joiner problem
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Handle SIGINT
  signal(SIGINT, [](int) { running = 0; });

  // Generate empty input
  std::string topic_receiver = "";
  std::string topic_sender   = "GreenStructOut";

  // Received Date
  // Packet Identifier. Must be related to the packet GreenStructIn
  uint64_t cycle_number;
  // Means system up-time second [s]
  double ecu_up_time;
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

  // Receive data
  bool ok = false;
  std::cout << "Receiving data..." << std::endl;
  while (running)
  {
    // Start time
    auto start = std::chrono::high_resolution_clock::now();

    // Read message topic
    int more         = 0;
    size_t more_size = sizeof(more);
    zmq_msg_t received_topic;
    int rc = zmq_msg_init(&received_topic);
    assert(rc == 0);
    rc = zmq_msg_recv(&received_topic, subscriber, 0);
    assert(rc != -1);
    rc = zmq_getsockopt(subscriber, ZMQ_RCVMORE, &more, &more_size);
    assert(rc == 0);
    assert(more == 1);

    // Read message data
    zmq_msg_t received_data;
    rc = zmq_msg_init(&received_data);
    assert(rc == 0);
    rc = zmq_msg_recv(&received_data, subscriber, 0);
    assert(rc != -1);
    rc = zmq_getsockopt(subscriber, ZMQ_RCVMORE, &more, &more_size);
    assert(rc == 0);
    assert(more == 0);

    // Print topic_receiver
    topic_receiver = std::string(
        static_cast<char *>(zmq_msg_data(&received_topic)),
        zmq_msg_size(&received_topic)
    );
    std::cout << "Topic: " << topic_receiver << std::endl;

    if (topic_receiver.compare("GreenStructIn") == 0)
    {
      // Fill in the flatbuffer
      auto data = flatbuffers::GetRoot<IGreen>(zmq_msg_data(&received_data));

      // Get data
      cycle_number                     = data->cycle_number();
      ecu_up_time                      = data->in()->ecu_up_time();
      adasis_curvature_nrs             = data->in()->adasis_curvature_nrs();
      auto adasis_curvature_dist_aux   = data->in()->adasis_curvature_dist();
      auto adasis_curvature_values_aux = data->in()->adasis_curvature_values();
      adasis_curvature_dist            = std::vector<double>(
          adasis_curvature_dist_aux->begin(), adasis_curvature_dist_aux->end()
      );
      adasis_curvature_values = std::vector<double>(
          adasis_curvature_values_aux->begin(),
          adasis_curvature_values_aux->end()
      );
      lane_width                       = data->in()->lane_width();
      adasis_speed_limits_nrs          = data->in()->adasis_speed_limits_nrs();
      auto adasis_speed_limit_dist_aux = data->in()->adasis_speed_limit_dist();
      auto adasis_speed_limit_values_aux =
          data->in()->adasis_curvature_values();
      adasis_speed_limit_dist = std::vector<double>(
          adasis_speed_limit_dist_aux->begin(),
          adasis_speed_limit_dist_aux->end()
      );
      adasis_speed_limit_values = std::vector<double>(
          adasis_speed_limit_values_aux->begin(),
          adasis_speed_limit_values_aux->end()
      );

      std::cout << "received <- cycle_number: " << cycle_number << std::endl;

      /* WRITE YOUR CODE HERE */
      std::this_thread::sleep_for(std::chrono::seconds(3));

      // Initialize a FlatBuffer builder
      flatbuffers::FlatBufferBuilder builder;
      // build your message
      double t0 = adasis_curvature_dist[0];
      flatbuffers::span<const double, 20> velocity_profile_times(
          adasis_speed_limit_dist.data(), 20
      );
      flatbuffers::span<const double, 20> velocity_profile_values(
          adasis_speed_limit_values.data(), 20
      );
      double cost_manoeuvre = adasis_speed_limits_nrs;

      auto green_struct_out = GreenStructOut(
          ecu_up_time, t0, velocity_profile_times, velocity_profile_values,
          cost_manoeuvre
      );
      auto packet =
          CreateIGreen(builder, data->cycle_number(), NULL, &green_struct_out);
      builder.Finish(packet);

      // Publish data, first the topic and then the message
      int err1 = zmq_send(
          subscriber, topic_sender.c_str(), topic_sender.size(), ZMQ_SNDMORE
      );
      int err2 = zmq_send(
          subscriber, builder.GetBufferPointer(), builder.GetSize(), 0
      );
      if (err1 == -1 or err2 == -1)
      {
        std::cout << "Error!!!" << std::strerror(errno) << std::endl;
      }
      std::cout << "sent -> cycle_number:" << data->cycle_number() << std::endl;
      zmq_msg_close(&received_topic);
      zmq_msg_close(&received_data);
    }

    // End time
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Total time:" << (end - start).count() / 1000000000.0
              << std::endl;
  }

  // Close the socket
  zmq_close(subscriber);

  // Destroy the context
  zmq_ctx_destroy(context);

  return 0;
}
