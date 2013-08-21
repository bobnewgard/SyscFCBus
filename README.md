## Introduction

This project provides the Bus struct, it's helpers, and the Bus\_src
class.

### SyscFCBus Use Cases

#### Supported Use Cases

* Passing communications data among components instantiated in a SystemC unit test bench

#### Unsupported Use Cases

* TBD

### C++ Templates

All of the struct and class definitions in SyscFCBus are templated with a single
parameter T\_be that specifies the bus width in bytes.  The value of the parameter
is the log base 2 of the number of bytes in the bus, so a value of zero specifies
2^1 or one byte and 6 specifies 2^6 or 64 bytes.

### Bus struct

The Bus struct is designed for use in a datapath, modelling communications
frame or cell data.  It supports datapath widths of 1, 2, 4, 8, 16,
32 or 64 bytes and has the fields

        usr[31:0]
        err
        val
        sof
        eof
        dat[((M*8)-1):0]
        mod[(N-1):0]

Where M is the number of bytes in the datapath and N is log base 2(M).

The Bus struct is designed for use in driving verilog RTL models
pre-processed by [Verilator](http://www.veripool.org/wiki/verilator).
Verilator imposes constraints on the data types verilog models export.
These constraints are reflected in the types declared for the fields in
the Bus struct.

### Bus\_split class

Splits out individual signals from a Bus for use with verilog module I/O.

### Bus\_src class

The Bus\_src class is designed for use as a datapath frame source.  Frames
are requested from a [SyscDrv](https://github.com/bobnewgard/SyscDrv)
instance.  Once every clock, frame data is written to the Bus struct.
The Bus struct is sent out of the class on an sc\_core::sc\_out<Bus<>>
channel.

### Unit Tests

The SyscFCBus verification suite is comprised of 28 tests
that are combinations of bus sizes
(1, 2, 4, 8, 16, 32 or 64 bytes)
and frame request delays (0, 1, 2 or 3 clocks).

Of the 28 tests, 27 are replicated from the test for a 1-byte bus
with zero clock request delay.

To generate the 27 tests with frame byte counts verified over the
range 64 to 68:

        ./tbgen gen

To generate the 27 tests with frame byte counts verified over the
range 64 to 1500:

        (export TBGEN_FRAME_COUNT=1437 ; ./tbgen gen)

The resulting directory structure is 7 test benches tb\_0 through tb\_6,
each testbench containing 4 tests test\_0 through test\_3.

To run each of the tests individually (where X specifies the test
bench tb\_X and Y specifies the test test\_Y):

        ./tbrun X Y sim

To view waveforms for these using the GTKWave waveform viewer:

        ./tbrun X Y wav

To run all 28 tests, saving the log:

        ./tbval |& tee log

## Validated Environments

The unit tests have been run successfully in the following environments

| Linux                | libc  | gcc   | SystemC | make | bash   | Python | Scapy |
|----------------------|-------|-------|---------|------|--------|--------|-------|
| Debian 3.2.0-4-amd64 | 2.13  | 4.7.2 | 2.3.0   | 3.81 | 4.2.37 | 2.7.3  | 2.2.0 |

## Other Dependencies

* [SyscMake](https://github.com/bobnewgard/SyscMake)
* [SyscMsg](https://github.com/bobnewgard/SyscMsg)
* [SyscJson](https://github.com/bobnewgard/SyscJson)
* [SyscDrv](https://github.com/bobnewgard/SyscDrv)
* [SyscClk](https://github.com/bobnewgard/SyscClk)

## Installation

1. Make sure you have installed the components shown in the
   "Validated Environments" section
    * Install SystemC from source, since it is unlikely to be
      packaged
1. Clone repos listed in "Other Dependencies"
    * Clone such that SyscFCBus and all other Sysc\* repos are
      in the same directory
1. Modify the path to the SystemC installation, SYSC\_DIR, in SyscMake/vars.mk
1. execute "make" for hints about available targets

## Issues

Issues are tracked at [github.com/bobnewgard/SyscFCBus/issues](https://github.com/bobnewgard/SyscFCBus/issues)

## Pull Requests

Pull requests are found at [github.com/bobnewgard/SyscFCBus/pulls](https://github.com/bobnewgard/SyscFCBus/pulls)

## License

### License For Code

The code in this project is licensed under the GPLv3

### License for This Project Summary

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0
Unported License. To view a copy of this license, visit
http://creativecommons.org/licenses/by-sa/3.0/.
