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

1. eventdemo.grc - Simple graph block testing the python version of event detection (no hardware needed).

1. detect_log.grc - Simple graph block for  testing the C++ version of event detection (no hardware needed).

1. eventwrite.grc - Event detection (python) with writing of events and logging a summary.  This graph requires a software defined radio.

The data go into the _events_ directory, one directory up from the current directory.

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

## Five Day Observing Test.
![Five Day Survey](/images/PeakEvent19March6.png)

We completed the GnuRadio C++ code and flow graph and ran observations for 5 days, from March 5 to 9, 2019.
The tests used a 32 diameter Horn with an effective system temperature of 120K.   The tests used an AIRSPY mini
software defined radio.

During this time, we identified events with signficance greater tha 6-sigma compared to the RMS noise in the
data stream.    For each event, we recorded 16384 I,Q samples, at 6 MHz sample rate, centered on the event.
These 16384 samples corresponds to a total integration time of 0.00273 seconds, with 0.00137 seconds before the
event and 0.00137 seconds after the event.

In the plot above, we zoom into the Peak event for March 6, 2019.   This time series shows a rapid rise, followed by a slower decay.

We wrote a program, "E", to plot individual events (available from github via a clone of http://github.com/glangsto/analyze),
and a program, "FFT", which Fourier Transforms the time series of samples and also counts events in the files.
The Event Count rate as a function of time of day shows some interesting features.  The time series is plotted below.

###

![Documentation](/images/EventsDetected6Sigma19March5-9.png)

There is clearly a time dependence on count rate.    It seems that more events are detected during the day light hours.
The Local Standard Time on the East Coast is UTC-5.  So UTC=0 corresponds to 19:00 EST or 7 PM.
The count rate increases signficantly at UTC=11, corrsponding to 6:00 AM EST.

The source of these flash events is detected is uncertain.  They may be due to the Sun or some terrestrial sources, like airplanes flying through the beam of the telescope.

The rarer, nightly, events may be due to cosmic-rays colliding with the Earth's atmosphere, creating radio flashes.



### Documentation

http://www.opensourceradiotelescopes.org/wk



Glen Langston -- glen.i.langston@gmail.com -- 2019 February 27
