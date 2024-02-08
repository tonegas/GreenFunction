/*

Send FlatBuffer data over ZMQ, this emulates a fast publisher to showcase how to
solve the slow subscriber problem.

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

  // Wait for 1 s to avoid the slow joiner problem
  std::this_thread::sleep_for(std::chrono::seconds(2));

  // Define a topic
  std::string topic = "CoDriver";

  // Generate CoDriver data
  uint64_t id                       = 0;
  std::vector<double> codriver_data = {1.0, 2.0, 3.0};

  // Send data
  std::cout << "Sending data..." << std::endl;
  for (int i = 0; i < 1e5; i++)
  {
    // Start time
    auto start = std::chrono::high_resolution_clock::now();

    // Initialize a FlatBuffer builder
    flatbuffers::FlatBufferBuilder builder;

    // Fill in the flatbuffer
    auto codriver_struct = CoDriverStruct(
        flatbuffers::span<double, 3>(codriver_data.data(), codriver_data.size())
    );
    auto packet = CreateICoDriver(builder, id, &codriver_struct);
    builder.Finish(packet);

    // Publish data, first the topic and then the message
    zmq_send(publisher, topic.c_str(), topic.size(), ZMQ_SNDMORE);
    zmq_send(publisher, builder.GetBufferPointer(), builder.GetSize(), 0);

    // Print data
    std::cout << "id: " << id << std::endl;
    std::cout << "data: ";
    for (auto &d : codriver_data)
    {
      std::cout << d << " ";
    }
    std::cout << std::endl;

    // Modify data
    id               += 1;
    codriver_data[0] += 1.0;
    codriver_data[1] += 1.0;
    codriver_data[2] += 1.0;

    // End time
    auto end = std::chrono::high_resolution_clock::now();

    // Wait for 10 ms
    std::this_thread::sleep_for(std::chrono::milliseconds(10) - (end - start));
  }

  // Close ZMQ
  zmq_close(publisher);
  zmq_ctx_destroy(context);

  return 0;
}
