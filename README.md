# ORCHID

<img src="https://github.com/jmatta1/ORCHID/raw/master/doc/resources/orchid.jpg" width="320" height="200" />

### ORCHID - ORnl Conditions at HfIr Daq
ORCHID is a concurrent data acquisition system built to take data from the Detector Array for measurement of Neutrons and Gammas (DANG) at HFIR.

#### Building ORCHID
Installation of ORCHID is relatively simple.
 - Step 1: Ensure the boost libraries (minimum version 1.54) are installed. ORCHID uses the following components of Boost
    - Boost.Thread
    - Boost.Log
    - Boost.Filesystem
    - Boost.DateTime
    - Boost.Spirit
 - Step 2: Ensure the ncurses library is present
 - Step 3: Ensure the snmp library, and snmp utilities (like snmpget, snmpset, and snmpwalk) are installed, the MPOD system requires these
 - Step 4: Ensure the Caen Comm library is present, this is the library that is required for communication with the Vx1730 digitizer.
 - Step 5: Download the ORCHID repository
 - Step 6: Build ORCHID
    - In the main directory of ORCHID you can simply run `make release`, or `make debug`. This will automatically create the appropriate build directory, run cmake within that directory, build ORCHID within that directory, and copy the executable into the base directory.

#### Starting ORCHID
To run ORCHID, you first need to edit the overall configuration file, MPOD module configuration file, MPOD channel configuration file, digitizer module configuration file, and digitizer channel configuration file to match your needs. These files have comments within them to explain the options and their use.
After editting these files, simply copy the ORCHID executable and configuration files into a directory of your choice and execute `./orchid overall_config_file_name`

#### Using ORCHID
ORCHID is fairly simple to use via its command line interface. After ORCHID starts, it outputs the parameters it read in and pauses. This gives the user a chance to very that those parameters are correct before ORCHID procedes.

After the user presses enter ORCHID procedes to allocate resources and then display the 'Initialize HV Screen'. In this screen there are two options, `turnon` and `quit`. The `quit` option simply causes orchid to close. The `turnon` option has behaviour that varies with configuration. If the option "PerformPowerOn" is set to "True" in the general configuration file, then orchid will send a "Main power on" signal to the MPOD crate, set the modules and channels to the configuration given in the two MPOD config files and wait for all the channels to finish ramping their output, then it will make ORCHID transition to the 'Idle Screen'. If "PerformPowerOn" is set to "False" the `turnon` command simply takes the user to the 'Idle Screen.'

At the 'Idle Screen' there are a number of possible commands.
 - The `start` command starts data acquisition and slow controls event writing. It then transitions to the 'Data Taking Screen.' On program startup the `start` command will not work until the `changerun` command has been used to set both the run title and run number. On subsequent returns to this screen after the first, `start` will not work until *at least* the run number has been changed, using either then `runnumber` or `next` commands.
 - The `changerun` command allows the user to change both the run title and run number.
 - The `runnumber` command allows the user to change the run number to an arbitrary value.
 - The `next` command allows the user to increment the run number from its current value.
 - The `turnoff` command varies with the option "PerformPowerOff" as well. If "PerformPowerOff" is set to "False", it will simply take the user back to the 'Initialize HV Screen'. If it is set to "True" ORCHID will ramp down the MPOD HV and set the MPOD to off, then go to the 'Initialize HV Screen'
 - The `quit` command functions exactly as expected, however its exact behavior will vary somewhat with the "PerformPowerOff" option. If it is set to "False" ORCHID will immediately exit. If "PerformPowerOff" is set to "True" then, before exitting, the quit command will cause ORCHID to ramp the MPOD channels to 0V then sends the "Main power off" signal prior to exitting.

At the 'Data Taking Screen' there are two available commands.
 - The `stop` command will cease data acquisition and return the user to the 'Idle Screen'
 - The `quit` command will either cease data acquisition and close ORCHID, or it will cease data acquisition, ramp down the MPOD HV, shutdown the MPOD, and then close ORCHID. These occur for "PerformPowerOff" being set to "False" or "True" respectively.

#### Programming ToDo List
 - TODO: Clean up the format and structure of the ui thread
 - TODO: Convert MPOD reader / controller to use Boost::Asio
 - TODO: Perform overall code cleanup and burn the cruft out of the code

### Notes on thread creation ordering.
Threads are created in the following order:
 - Primary, initial execution, thread, created at program start, goes to sleep after UI thread starts.
 - Asyncrhonous file writer/worker thread
 - Slow Controls thread;
 - File output thread
 - Acquisition thread(s)
 - Processing thread(s)
 - UI Thread
