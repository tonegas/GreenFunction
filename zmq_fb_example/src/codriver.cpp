/*

Send FlatBuffer data over ZMQ

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
  void *publisher = zmq_socket(context, ZMQ_PUB);

  // Set LINGER to 0
  int linger = 0;
  zmq_setsockopt(publisher, ZMQ_LINGER, &linger, sizeof(linger));

  // Bind to port
  int rc = zmq_bind(publisher, "tcp://127.0.0.1:5555");
  assert(rc == 0);

  // Define a topic
  std::string topic = "CoDriver";

  // Generate CoDriver data
  uint64_t id = 0;
  std::vector<double> codriver_data = {1.0, 2.0, 3.0};

  // Send data
  std::cout << "Sending data..." << std::endl;
  for (int i = 0; i < 100; i++)
  {
    // Initialize a FlatBuffer builder
    flatbuffers::FlatBufferBuilder builder;

    // Fill in the flatbuffer
    auto codriver_struct = CoDriverStruct(flatbuffers::span<double, 3>(
        codriver_data.data(), codriver_data.size()));
    auto packet = CreateICoDriver(builder, id, &codriver_struct);
    builder.Finish(packet);

    // Publish data
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

    // Wait for 0.5 s
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Modify data
    id += 1;
    codriver_data[0] += 1.0;
    codriver_data[1] += 1.0;
    codriver_data[2] += 1.0;
  }

  // Close ZMQ
  zmq_close(publisher);
  zmq_ctx_destroy(context);

  return 0;
}
