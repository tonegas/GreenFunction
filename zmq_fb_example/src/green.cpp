/*

Receive FlatBuffer data over ZMQ

*/

#include "green_function_generated.h"
#include "zmq.h"
#include <assert.h>
#include <chrono>
#include <iostream>
#include <signal.h>
#include <string>
#include <thread>
#include <vector>

using namespace GreenFunction;

int main()
{
  // Pub socket
  void *context = zmq_ctx_new();
  void *subscriber = zmq_socket(context, ZMQ_SUB);

  // Set CONFLATE to 1
  // int conflate = 1;
  // zmq_setsockopt(subscriber, ZMQ_CONFLATE, &conflate, sizeof(conflate));

  // Connect to port
  int rc = zmq_connect(subscriber, "tcp://127.0.0.1:5555");
  assert(rc == 0);

  // Subscriber to a topic
  zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "CoDriver", 8);

  // Generate empty CoDriver data
  uint64_t id = 0;
  std::vector<double> codriver_data = {};

  // Receive data
  std::cout << "Receiving data..." << std::endl;
  while (true)
  {
    // Create zmq message
    zmq_msg_t msg;
    zmq_msg_init(&msg);

    // Create a buffer for the topic
    zmq_msg_t topic;
    zmq_msg_init(&topic);

    // Receive data
    rc = zmq_msg_recv(&topic, subscriber, 0);
    rc = zmq_msg_recv(&msg, subscriber, 0);

    // Wait for 1 s
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    if (rc == -1)
      continue;

    // Fill in the flatbuffer
    auto codriver = flatbuffers::GetRoot<ICoDriver>(zmq_msg_data(&msg));

    // Get data
    id = codriver->id();
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

    // Free zmq message
    zmq_msg_close(&msg);
  }

  // Close ZMQ
  zmq_close(subscriber);
  zmq_ctx_destroy(context);

  return 0;
}
