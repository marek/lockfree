#
# lockfree demo
# Marek Kudlacz
#

This is accompanying code to a intro to lockfree presentation.
This show cases a practical usage scenario in optimizing a logging system.

### test cases
The goal is to minimize the time spent logging in the working/main threads.
The benchmarks only time how long it takes to issue the log statement, not how long the
background writer takes to flush everything to disk.

Keep in mind as the internal queues get used up, their performance will decrease. The cache and queue sizes must be tuned 
to your particular use case. They rarely will be maxed out like this.

- **direct**: directly logging to a file
- **multipledirect**: multiple threads directly logging to a file. Synchronized with a lock
- **worker**: using a background thread to write logging statements from the main thread
- **multipleworker**: using a background thread to write logging statements from multiple threads. Synchronized with a lock
- **cblockfree**: similar to multipleworker. Using a CircularBuffer to pass data between logging threads and background writer. No locks.
- **dualcbfree**: two CircularBuffers. One for unused std::strings to speed up allocation. one for queueing log statements
- **mpsclockfree**: CircularBuffer for unused std::strings. Multi Producer, Single Consumer queue for passing log statements
- **spsclockfree**: Single Producer, Single Consumer queue holding free strings and as log queue. Two for each working thread. Shared amongst background writer.
- **tdcblockfree**: Same as previous, but trying the same with two CircularBuffers per thread.

