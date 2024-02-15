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

int main()
{
  // Create a ZMQ context
  void *context = zmq_ctx_new();

  // Create the subscriber socket
  void *subscriber = zmq_socket(context, ZMQ_SUB);

  // Connect to the endpoint
  int rc = zmq_connect(subscriber, "tcp://127.0.0.1:5555");
  assert(rc == 0);

  // Subscribe to the topic
  rc = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "CoDriver", 8);
  assert(rc == 0);

  // Wait for 1 s to avoid the slow joiner problem
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Create the queue
  ZMQRecvQueue subscriber_queue(subscriber, 1);

  // Start the queue
  subscriber_queue.start();

  // Create an empty ZMQMessage
  std::shared_ptr<ZMQMessage> msg = std::make_shared<ZMQMessage>();

  // Generate empty data containers
  std::string topic                 = "";
  uint64_t id                       = 0;
  std::vector<double> codriver_data = {};

  // Receive data
  bool ok = false;
  std::cout << "Receiving data..." << std::endl;
  while (true)
  {
    // Start time
    auto start = std::chrono::high_resolution_clock::now();

    // Receive data
    ok = subscriber_queue.msg_recv(msg);
    if (!ok)
    {
      std::this_thread::sleep_for(std::chrono::microseconds(100));
      continue;
    }

    // Print topic
    topic = std::string(
        static_cast<char *>(zmq_msg_data(msg->at(0))), zmq_msg_size(msg->at(0))
    );
    std::cout << "Topic: " << topic << std::endl;

    // Fill in the flatbuffer
    auto codriver = flatbuffers::GetRoot<ICoDriver>(zmq_msg_data(msg->at(1)));

    // Get data
    id                  = codriver->id();
    auto codriver_array = codriver->data()->double_array();
    codriver_data =
        std::vector<double>(codriver_array->begin(), codriver_array->end());

    // Print data
    std::cout << "id: " << id << std::endl;
    std::cout << "data: ";
    for (auto &d : codriver_data)
    {
      std::cout << d << " ";
    }
    std::cout << std::endl;

    // End time
    auto end = std::chrono::high_resolution_clock::now();

    // Wait for 1 s
    std::this_thread::sleep_for(std::chrono::seconds(1) - (end - start));
  }

  return 0;
}
