# ZeroMQ with FlatBuffers example

This folder contains an example of how to use ZeroMQ with FlatBuffers via C++.

## Usage

Install the third-party dependencies:

```bash
scripts/third_party.sh
```

Build the project:

```bash
mkdir build
cd build
cmake ..
make
```

Run the publisher:

```bash
build/publisher
```

Run the subscriber:

```bash
build/subscriber
```

## Notes

The FlatBuffers schema is already compiled, you can compile it again with:

```bash
third_party/flatbuffers/bin/flatc --cpp --gen-object-api -o interfaces/ green_function.fbs
```

## Useful links

- [ZeroMQ](https://zeromq.org/)
- [FlatBuffers](https://google.github.io/flatbuffers/)
