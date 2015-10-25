# Overview
`iris` is asynchronous logging library designed to log messages with minimum overhead to the performance. The cost of logging a message is merely copying the arguments into a lockfree queue.

# Design
Under the hood, `iris` uses a background thread (logging thread) to offload logging from other threads.
1. For every other threads, they keep a lockfree queue to buffer the messages.
2. The logging thread periodically scans through all these queues to collect messages, formats them and writes them into log file.  

A few highlights of the design of `iris` to achieve low latency:
1. Buffering messages with thread local lockfree queue.
2. Memory is managed by a thread local ringbuffer taking the advantage that logging is FIFO. Because the fact that logging is FIFO. This scales well in multithreaded environment. 
3. Minimum context switches.

# Usage
The supported severity levels are:
```c++
enum severity_level {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL
};
```
By default, `iris` logs messages to `stdout` and filters out severity level less than `INFO`.

```c++
#include <iris/level_logger.h>

// this creates a logging thread, logs messages to stdout
iris::level_logger g_log;

int main (int argc, char const *argv[]) {
    //configure thread level parameters, these should be done before any logging
    // queue size
    g_log.set_thread_queue_size(1024);
    // ring buffer size
    g_log.set_thread_ringbuf_size(10240);

    g_log.info("Greetings from %s, bye %d\n", 'iris', 0);

    //this tells logging thread to persist the data into file and waits
    g_log.sync_and_close();

    return 0;
}
```
Using a `file_writer` to logs all messages into a file:
```c++
#include <iris/level_logger.h>
#include <iris/file_writer.h>

iris::file_writer writer("./log.txt");
// this creates a logging thread
iris::level_logger g_log(&writer, iris::TRACE);

int main (int argc, char const *argv[]) {
    //configure thread level parameters, these should be done before any logging
    // queue size
    g_log.set_thread_queue_size(1024);
    // ring buffer size
    g_log.set_thread_ringbuf_size(20480);
    
    g_log.info("Greetings from %s, bye %d\n", 'iris', 0);
    
    //this tells logging thread to persist the data into file and waits
    g_log.sync_and_close();

    return 0;
}
```

# Building & Installation

To build the library, simply clone the projet, go inside the `iris` direcotry and  run following commands.
```shell
make
make test
make install
```
To integrate `iris` into your program, link with `-liris` options.