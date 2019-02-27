## gr-detect_cc

### Nsf-Detect: Library of Gnuradio Companion (GRC) blocks, C++  and Python code for Radio Astronomy

This repository contains all modules in the gr-nsf github node plus new C++ code for efficient detection
of radio transient events.

This repository contains both Python and C++ version of the event capture software.
The block _detect_ is the C++ version.   The block _event_ is the python version.   Both
versions are compiled and installed using cmake.

### Observer Interface: NsfIntegrate60.grc

![Observer Interface](/images/NsfIntegrate.png)

### Documentation

http://www.opensourceradiotelescopes.org/wk

The executables are in the _examples_ directory.

The GRC files are:

1. vectordemo.grc - Simple test block comparing the simulated data after averaging and medianing (no data source needed).

1. Integrate_test.grc - More complicated test function using all blocks in the NsfIntegrate Designs except
the *Osmosdr* block.

1. NsfIntegrate60.grc - Block configured to use an AIRSPY mini with 6.0 MHz bandwidth for Radio Astronomy Observations

1. NsfIntegrate20.grc - Block configured to use an RTL SDR dongle with 2.0 MHz bandwidth for Radio Astronomy Observations

1. eventdetect.grc - Event detection demonstration graph.

1. eventwrite.grc - Event detection with writing of events and logging a summary.

The '*.ast' files contain example spectral line observations. 

The '*.hot' files contains hot-load calibration observations.  The NsfIntegrate blocks can overwrite these files.

Configuration files are used to record some input parameters and allow restarting tests and survey observations.

1.  Watch60.conf is a configuration file for the NsfIntegrate60.grc AIRSPY 6.0 MHz observing block
2.  Watch20.conf is a configuration file for the NsfIntegrate20.grc RTLSDR 2.0 MHz observing block

The Watch.not is a *Note File* describing the astronomical setup.  This file also contains a spectrum observation,
as the goal of the data header is to allow a complete re-observation, based on the previous header values.

### To test these blocks, without installing into the standard GRC path, use these commands:

`cd examples`

`export PYTHONPATH=../python:$PYTHONPATH`

and update your ~/.gnuradio/config.conf file with this path:

`[grc]`

`local_blocks_path = ./:../grc`

Finally run the GRC files:

`gnuradio-companion vectordemo.grc`

or 

`gnuradio-companion NsfIntegrate60.grc`

To start observing.  This graph assumes you have an AIRSPY mini.  If you have a different device, you'll have
to edit the graph with the appropriate configuration.

### Optomizing operation
The hardkernel.com Odriod XU4 octa-core processor can capture all 6 MHz of data from an AIRSPY-mini, if the 2 GHz processors are selected.  After recreating the design on your local computer using GRC, then exit and run python from the command line.

The linux 'taskset' command can be used to select the 2 GHz processors (number 4,5,6 and 7) and one slower processor:

`taskset -c 7,6,5,4,3 python NsfIntegrate60.py`

You can use the _top_ command to see which processes are using the CPU on your computer.

Move all the other processes to other cores with the taskset commands like:

`taskset -pc 0 process-id`

Where process-id is one of the higher cpu usage processes on your device.

### Documentation

http://www.opensourceradiotelescopes.org/wk

![Observer Interface Raw Signals](/images/NsfIntegrateRaw.png)

Glen Langston -- glen.i.langston@gmail.com -- 2019 February 27
