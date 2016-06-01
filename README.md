# ORCHID
<img src="https://github.com/jmatta1/ORCHID/raw/master/doc/resources/orchid.jpg" width="320" height="200" />
###ORCHID - ORnl baCkground at HfIr Daq

This is a Data Acquisiton and Control program for handling the CAEN digitizer via optical link and handling the MPOD HV system.

####Programming ToDo List
 - TODO: Implement File Output With Asio
 - TODO: Implement Compressing buffer
 - TODO: Implement a lock free queue with waiting for data
 - TODO: Implement Digitizer Reader and Controller
 - TODO: Remove MPOD spoofing from SnmpUtilController once testing with actual MPOD
 - TODO: Implement the Digitizer Thread
 - TODO: Implement the File Writing Thread
 - TODO: Implement the Event Processing Thread(s)
 - TODO: Update UI Thread as more things are implemented
 - TODO: Clean up the format and structure of the ui thread when everything else is done
 - TODO: Perform code cleanup and burn the cruft out of the code

In essense there are several threads that will perform data handling:

One thread is constantly waiting for the optical link to send data, taking that data and dumping pointers to buffers of data into a lock free queue with a wait for data condition variable. 

A pool of threads is taking data from the queue, extracts only what is needed into events from the data buffers. Empty data buffers are put on a lock free queue sending them back to the digitizer thread, events are enqueued in another lock free queue with a wait for data condition variable to be sent to the file thread.

In the mean time the slow controls thread is reading the mpod and temperature data occaisionally (the exact timing set in the input file), placing that in a structure for display in the UI thread and also dumping that data into an event. It then enqueues the event in the lock free queue that seends voltage events to the file thread.

The file thread first checks to see if there is anything in the voltage event queue. If there is it reads that event, puts it into the output buffer (compressing or otherwise), sends the empty event into the voltage event return queue, and then proceeds as if there was nothing in the voltage event queue.  If there is nothing in the voltage event queue then the file thread checks the data event queue for events, if there is no event it waits on a condition variable that wakes the thread if there is a status change, a new voltage event, or a new data event. If there is an event in the data event queue it then places that event in the output buffer. If any output buffer write operation would cause the buffer to exceed a set size (4MB may be a good number but it is regulated by the input file) then the buffer is sent for an asynchronous write to disk and a new buffer is taken, initialized and set as the buffer that is written to. 
