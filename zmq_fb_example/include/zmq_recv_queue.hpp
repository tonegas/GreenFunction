/*

clang-format off
███████╗███╗   ███╗ ██████╗     ██████╗ ███████╗ ██████╗██╗   ██╗     ██████╗ ██╗   ██╗███████╗██╗   ██╗███████╗
╚══███╔╝████╗ ████║██╔═══██╗    ██╔══██╗██╔════╝██╔════╝██║   ██║    ██╔═══██╗██║   ██║██╔════╝██║   ██║██╔════╝
  ███╔╝ ██╔████╔██║██║   ██║    ██████╔╝█████╗  ██║     ██║   ██║    ██║   ██║██║   ██║█████╗  ██║   ██║█████╗
 ███╔╝  ██║╚██╔╝██║██║▄▄ ██║    ██╔══██╗██╔══╝  ██║     ╚██╗ ██╔╝    ██║▄▄ ██║██║   ██║██╔══╝  ██║   ██║██╔══╝
███████╗██║ ╚═╝ ██║╚██████╔╝    ██║  ██║███████╗╚██████╗ ╚████╔╝     ╚██████╔╝╚██████╔╝███████╗╚██████╔╝███████╗
╚══════╝╚═╝     ╚═╝ ╚══▀▀═╝     ╚═╝  ╚═╝╚══════╝ ╚═════╝  ╚═══╝       ╚══▀▀═╝  ╚═════╝ ╚══════╝ ╚═════╝ ╚══════╝
clang-format on

Note that this class is designed to be **BLAZINGLY FAST** with some readability in
mind. If you feel like you can make it **BLAZINGLY FASTER**, feel free to
discuss it with us!

*/

#pragma once

#include "zmq.h"
#include "zmq_message.hpp"
#include <chrono>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

/**
 * @brief A simple class to store incoming ZMQ messages in a queue
 * asynchronously.
 *
 * This header-only class provides a simple interface to have a variable-size
 * FIFO queue of incoming ZMQ messages. The class is designed to be as similar
 * as possible to the default libzmq API, but with the added functionality of a
 * queue. As such, it will **NOT** do most of the ZMQ work for you, it will
 * simply store the incoming messages in a queue asynchronously.
 *
 * The class is designed to be used in a multi-threaded environment, where the
 * user can start the receive thread and then call the `msg_recv` method to
 * retrieve the oldest message in the queue. The user can also stop the receive
 * thread at any time.
 */
class ZMQRecvQueue
{
private:
  // ZMQ socket
  void *socket;

  // Message queue
  std::queue<std::shared_ptr<ZMQMessage>> msg_queue;
  size_t queue_size;

  // Receive thread
  std::thread *recv_thread;

  // Mutex for queue access
  std::mutex mtx_queue;

  // Mutex for thread management
  std::mutex mtx_manage_thread;

  // Running flag
  bool stop_thread = false;

public:
  /**
   * @brief Construct a new ZMQRecvQueue object
   *
   */
  ZMQRecvQueue() = default;

  /**
   * @brief Construct a new ZMQRecvQueue object
   *
   * **YOU** are responsible for creating and binding the ZMQ socket. This class
   * will only store the incoming messages in a queue. Note that once you pass
   * the socket to this class, you should not use it in your main thread
   * anymore, as this class will be using it in a separate thread. Remember that
   * ZMQ sockets are **NOT** thread-safe.
   *
   * @param socket The ZMQ socket to receive messages from
   * @param queue_size The maximum size of the queue
   */
  ZMQRecvQueue(void *socket, const size_t queue_size = 1)
  {
    // Set the socket
    this->socket = socket;

    // Set queue size
    this->queue_size = queue_size;
  }

  /**
   * @brief Destroy the ZMQRecvQueue object
   *
   * This will stop the receive thread if it is running.
   */
  ~ZMQRecvQueue()
  {
    // Stop the thread only if it is running
    if (!this->stop_thread)
    {
      this->stop();
    }
  }

  /**
   * @brief Start the receive thread
   *
   * This will start the receive thread, which will receive messages from the
   * ZMQ socket and store them in the queue.
   */
  void start()
  {
    // Start the receive thread
    this->recv_thread = new std::thread(&ZMQRecvQueue::receive_loop, this);
  }

  /**
   * @brief Stop the receive thread
   *
   * This will stop the receive thread and wait for it to finish.
   */
  void stop()
  {
    // Tell the thread to stop
    { // Critical section
      std::lock_guard<std::mutex> lock(this->mtx_manage_thread);
      this->stop_thread = true;
    }

    // Wait for the thread to finish
    this->recv_thread->join();

    // Free the thread
    delete this->recv_thread;
  }

  /**
   * @brief Receive a message from the queue
   *
   * This will receive the oldest message from the queue and store it in the
   * provided ZMQMessage.
   *
   * @param msg The ZMQMessage to store the message in
   * @return true if a message was received, false otherwise
   */
  bool msg_recv(std::shared_ptr<ZMQMessage> &msg)
  {
    // Lock
    std::lock_guard<std::mutex> lock(this->mtx_queue);

    // Check if the queue is empty
    if (this->msg_queue.empty())
    {
      return false;
    }

    // Give the oldest message to the user
    msg = this->msg_queue.front();

    // Pop the oldest message
    msg_queue.pop();

    return true;
  }

private:
  // Infinite loop to receive messages
  void receive_loop()
  {
    while (true)
    {
      // Check if the thread should stop
      { // Critical section
        std::lock_guard<std::mutex> lock(this->mtx_manage_thread);

        // Check if the thread should stop
        if (this->stop_thread)
        {
          break;
        }
      }

      // Receive data
      this->receive();
    }
  }

  // Receive a message from the socket with all its parts
  void receive()
  {
    // Create empty ZMQ message
    zmq_msg_t part;

    // Create an empty ZMQMessage
    std::shared_ptr<ZMQMessage> msg = std::make_shared<ZMQMessage>();

    // Prepare the more flag and classic ZMQ return variable
    int more         = 0;
    size_t more_size = sizeof(more);
    int rc;

    // Receive all message parts
    while (true)
    {
      // Initialise the msg
      zmq_msg_init(&part);

      // Block until a message is available to be received from socket
      rc = zmq_msg_recv(&part, this->socket, ZMQ_DONTWAIT);
      if (-1 == rc)
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        return;
      }

      // Add the part to the message
      msg->add_part(&part);

      // Close the message
      zmq_msg_close(&part);

      // Check if there are still parts to receive
      zmq_getsockopt(this->socket, ZMQ_RCVMORE, &more, &more_size);

      // Check if there are still parts to receive
      if (!more)
      {
        break;
      }
    }

    // Cycle the queue
    this->cycle_queue(msg);
  }

  // Cycle the queue pushing the new message at the end and freeing the oldest
  // message at the beginning if the queue is full
  void cycle_queue(const std::shared_ptr<ZMQMessage> &msg)
  {
    // Lock
    std::lock_guard<std::mutex> lock(this->mtx_queue);

    // Check if the queue is full
    if (this->msg_queue.size() >= this->queue_size)
    {
      this->msg_queue.pop();
    }

    // Push the new message
    this->msg_queue.push(msg);
  }
};
