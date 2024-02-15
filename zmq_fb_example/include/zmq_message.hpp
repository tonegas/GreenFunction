/*

clang-format off
███████╗███╗   ███╗ ██████╗     ███╗   ███╗███████╗███████╗███████╗ █████╗  ██████╗ ███████╗
╚══███╔╝████╗ ████║██╔═══██╗    ████╗ ████║██╔════╝██╔════╝██╔════╝██╔══██╗██╔════╝ ██╔════╝
  ███╔╝ ██╔████╔██║██║   ██║    ██╔████╔██║█████╗  ███████╗███████╗███████║██║  ███╗█████╗
 ███╔╝  ██║╚██╔╝██║██║▄▄ ██║    ██║╚██╔╝██║██╔══╝  ╚════██║╚════██║██╔══██║██║   ██║██╔══╝
███████╗██║ ╚═╝ ██║╚██████╔╝    ██║ ╚═╝ ██║███████╗███████║███████║██║  ██║╚██████╔╝███████╗
╚══════╝╚═╝     ╚═╝ ╚══▀▀═╝     ╚═╝     ╚═╝╚══════╝╚══════╝╚══════╝╚═╝  ╚═╝ ╚═════╝ ╚══════╝
clang-format on

Note that this class is designed to be **BLAZINGLY FAST** with some readability in
mind. If you feel like you can make it **BLAZINGLY FASTER**, feel free to
discuss it with us!

*/

#pragma once

#include "zmq.h"
#include <iostream>
#include <vector>

/**
 * @brief A simple class to store incoming ZMQ message parts.
 *
 * This header-only class provides a simple interface to have a variable-size
 * vector of incoming ZMQ message parts.
 */
class ZMQMessage
{
private:
  // Vector of ZMQ message parts
  std::vector<zmq_msg_t *> parts;

public:
  /**
   * @brief Construct a new ZMQMessage object
   *
   * @param num_parts The number of parts to reserve space for
   */
  ZMQMessage(size_t num_parts = 10)
  {
    // Reserve space for the parts
    this->parts.reserve(num_parts);
  }

  /**
   * @brief Destroy the ZMQMessage object
   *
   */
  ~ZMQMessage()
  {
    // Close all the parts and free the memory
    for (auto part : this->parts)
    {
      zmq_msg_close(part);
      delete part;
    }
  }

  /**
   * @brief Get the number of parts in the message
   *
   * @return The number of parts in the message
   */
  size_t size() const { return this->parts.size(); }

  /**
   * @brief Check if the message is empty
   *
   * @return true if the message is empty, false otherwise
   */
  bool empty() const { return this->parts.empty(); }

  /**
   * @brief Access a part of the message without checking for bounds
   *
   * @param index The index of the part to access
   * @return The ZMQ message part at the given index
   */
  zmq_msg_t *operator[](size_t index) { return this->parts[index]; }

  /**
   * @brief Access a part of the message checking for bounds
   *
   * @param index The index of the part to access
   * @return The ZMQ message part at the given index
   */
  zmq_msg_t *at(size_t index) { return this->parts.at(index); }

  /**
   * @brief Add a part to the message
   *
   * @param part The ZMQ message part to add
   */
  void add_part(zmq_msg_t *part)
  {
    zmq_msg_t *new_part = new zmq_msg_t;
    zmq_msg_init(new_part);
    zmq_msg_move(new_part, part);
    this->parts.push_back(new_part);
  }
};
