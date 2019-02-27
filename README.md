## gr-detect_cc

### Nsf-Detect: Library of Gnuradio Companion (GRC) blocks, C++  and Python code for Radio Astronomy

This repository contains both Python and C++ versions of the event capture software.
The blocks with _detect_ names are the C++ versions.   The blocks with _event_ are the python versions.  Both
versions are prepared and installed using _cmake_.

### Observer Interface: NsfDetect10rtlsdr.grc

![Observer Inteface](/images/NsfDetect10rtlsdr.png)

### Executables

The executables are in the _examples_ directory.

The GRC files are:

1. NsfDetect10rtlsdr.grc - Detect events (C++ version) at 1 MHz sample rate (both I+Q) using an RTL SDR dongle.

1. NsfDetect60airspy.grc - Detect events (C++ version) at 6 MHz sample rate (both I+Q) using an AIRSPY Mini dongle.

1. eventdemo.grc - Simple graph block testing python version of event detection.

1. detect_log.grc - Simple graph block testing the C++ version of event detection.

1. eventwrite.grc - Event detection (python) with writing of events and logging a summary.

The data go into the _events_ directory one directory up from the current directory.

The '*.eve' files contain example event observations. 

The '*.log' files contain logs of events detected.

Configuration files are used to record some input parameters and allow restarting tests and survey observations.

1.  Detect.conf is a configuration file for the NsfIntegrate60.grc AIRSPY 6.0 MHz observing block

### Optomizing operation
The hardkernel.com Odriod XU4 octa-core processor can capture all 6 MHz of data from an AIRSPY-mini, if the 2 GHz processors are selected.  After recreating the design on your local computer using GRC, then exit and run python from the command line.

The linux 'taskset' command can be used to select the 2 GHz processors (number 4,5,6 and 7) and one slower processor:

`taskset -c 7,6,5,4,3 python NsfDetect60airspy.py`

You can use the _top_ command to see which processes are using the CPU on your computer.

Move all the other processes to other cores with the taskset commands like:

`taskset -pc 0 process-id`

Where process-id is one of the higher cpu usage processes on your device.

### Test without a Software Defined Radio Device
The event detection code (both C++ and Python) can be tested without hardware device.
Below is the detect_log.grc design that tests the C++ code.

![Documentation](/images/detect_log.png)


### Documentation

http://www.opensourceradiotelescopes.org/wk



Glen Langston -- glen.i.langston@gmail.com -- 2019 February 27
