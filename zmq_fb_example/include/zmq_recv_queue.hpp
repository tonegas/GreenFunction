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
#include <atomic>
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

  // Running flag
  std::atomic_bool thread_running{false};

public:
  /**
   * @brief Construct a new ZMQRecvQueue object
   *
   */
  ZMQRecvQueue() = default;

  /**
   * @brief Construct a new ZMQRecvQueue object
   *
   * @param queue_size The maximum size of the queue
   */
  ZMQRecvQueue(const size_t queue_size = 1)
  {
    // Set queue size
    this->queue_size = queue_size;
  }

  /**
   * @brief Destroy the ZMQRecvQueue object
   *
   * This will stop the receive thread if it is running, as well as free the
   * ZMQ socket if it is still owned by the ZMQRecvQueue.
   */
  ~ZMQRecvQueue()
  {
    // Stop the thread only if it is running
    if (this->thread_running)
    {
      void *socket;
      this->stop(&socket);
      zmq_close(socket);
    }
  }

  /**
   * @brief Start the receive thread
   *
   * This will start the receive thread, which will receive messages from the
   * ZMQ socket and store them in the queue.
   *
   * **YOU** are responsible for creating the ZMQ socket and passing it to this
   * method. This is done to allow the user to create any socket they want, with
   * any configuration they want.
   *
   * Since ZMQ sockets are not thread-safe, we will take ownership of the
   * socket and set your pointer to `nullptr`. Once you are done with the
   * ZMQRecvQueue, you can call the `stop` method to stop the receive thread and
   * receive the socket back. We will never close the socket, so **YOU** are
   * responsible for doing so.
   *
   * @param socket The ZMQ socket to receive messages from
   */
  void start(void **socket)
  {
    // Take ownership of the socket
    this->socket = *socket;
    *socket      = nullptr;

    // Check that the socket has a timeout set
    int timeout;
    size_t timeout_size = sizeof(timeout);
    zmq_getsockopt(this->socket, ZMQ_RCVTIMEO, &timeout, &timeout_size);
    if (timeout == -1)
    {
      throw std::runtime_error(
          "ZMQRecvQueue: The socket has no timeout set, this will "
          "cause the receive thread to block indefinitely. Please set "
          "a timeout different from -1 to avoid this."
      );
    }

    // Warn the user if the timeout is zero
    if (timeout == 0)
    {
      std::cerr << "ZMQRecvQueue: The socket has a timeout of 0, this will "
                   "cause the receive thread to consume 100%% of the CPU when "
                   "no messages are available. "
                   "Please set a timeout different from 0 to avoid this."
                << std::endl;
    }

    // Set the running flag
    this->thread_running = true;

    // Start the receive thread
    this->recv_thread = new std::thread(&ZMQRecvQueue::receive_loop, this);
  }

  /**
   * @brief Stop the receive thread
   *
   * This will stop the receive thread and wait for it to finish. It will then
   * return the ZMQ socket to the user.
   *
   * @param socket The ZMQ socket to return to the user
   */
  void stop(void **socket)
  {
    // Tell the thread to stop
    this->thread_running = false;

    // Wait for the thread to finish
    this->recv_thread->join();

    // Free the thread
    delete this->recv_thread;

    // Return the socket to the user
    *socket = this->socket;

    // Set the socket to nullptr
    this->socket = nullptr;
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
    while (this->thread_running)
    {
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

      // No Block until a message is available to be received from socket
      rc = zmq_msg_recv(&part, this->socket, ZMQ_DONTWAIT);
      if (-1 == rc)
      {
        return;
      }

      // Add the part to the message
      msg->add_part(&part);

      // Close the message
      zmq_msg_close(&part);

      // Check if there are still parts to receive
      zmq_getsockopt(this->socket, ZMQ_RCVMORE, &more, &more_size);
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
