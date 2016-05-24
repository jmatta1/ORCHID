# ORCHID
<img src="https://github.com/jmatta1/ORCHID/raw/master/doc/resources/orchid.jpg" width="320" height="200" />
###ORCHID - ORnl baCkground at HfIr Daq

This is a Data Acquisiton and Control program for handling the CAEN digitizer via optical link and handling the MPOD HV system.

 - TODO: Implement csv handling (generic via template meta programming?)
 - TODO: Implement file buffer file
 - TODO: Implement compressing buffer
 - TODO: Implement lock free ring buffer https://www.kernel.org/doc/Documentation/trace/ring-buffer-design.txt
 - TODO: Implement Digitizer Reader and Controller
 - TODO: Implement MPOD Reader and Controller
 - TODO: Implement asynchronus UI
 - TODO: Implement various threads

In essense there are several threads that will perform data handling, one thread is constantly waiting for the optical link to send data, taking that data and dumping it to a lock free ring buffer. A pool of threads is taking data from the ring buffer, extracting only what is needed, and dumping that to another lock free ring buffer. Then a thread is constantly taking data from that ring buffer and dumping it to a compressing buffer. When that buffer exceeds 1MB it is sent to be written asynchonously to disk while another compressing buffer starts to be filled. Off to the side there is a thread that polls the MPOD on a regular schedule, perhaps 1Hz. These voltages are included in the relevant events. Further there is a display thread that handles updating the display and occaisionally writing stuff to the screen (1Hz?). If possible this thread will also handle asynchronous user input, if it is possible it will also be the thread to send commands to the digitizer and MPOD. If it is not possible then the main thread instead of being the display/async input/control thread, will be the input/control thread.
