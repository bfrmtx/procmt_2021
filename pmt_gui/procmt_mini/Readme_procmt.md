# ProcMT[^1] 
## procmt_mini

procmt_mini is a small GUI driving the procmt kernel.
You are reading the readme file for procmt_mini with Qt6 which has beed downloaded from the [procmt repository](https://github.com/bfrmtx/procmt_2021) or from the metronix cloud [Procmt_experimental](https://cloud.geo-metronix.de/s/wiqepHdGHHpKS5M)



### Installation

Simply unzip the downloaded file.

On Windows you need to set the PATH variable to the bin directory *and* the lib directory. Example:

`c:\Program Files\metronix\procmt\bin`

`c:\Program Files\metronix\procmt\lib`

On Linux / MacOS rpath is used to find the libraries.

When processing is finished, by default the "ediplotter" is called in order to display the results.
The "old" ediplotter is NOT part of the procmt package. (It does not work with Qt6). You can download it from the metronix cloud [procmt_mini](https://cloud.geo-metronix.de/s/GJaZaPprX3YmsK9). Unzip it and set the PATH variable to the directory. Example:

`c:\Program Files\metronix\procmt_old`

### EDI

```{warning} 
ProcMT does not rotate data at all. Additionally an orthogonal setup is assumed always.
```

```{warning} 
Almost all EDI file readers assume **NORTH** as measurement direction. 
```

The correct way is to supply all coordinates / distances in the EDI file.

```
>EMEAS ID=1000.0001 CHTYPE=EX X=-3.000000e+01 Y=0.000000e+00 Z=0.000000e+00 X2=3.000000e+01  Y2=0.000000e+00 Z2=0.000000e+00 
>EMEAS ID=1001.0001 CHTYPE=EY X=0.000000e+00 Y=-3.000000e+01 Z=0.000000e+00 X2=0.000000e+00 Y2=3.000000e+01  Z2=0.000000e+00 
>HMEAS ID=1002.0001 CHTYPE=HX X=0.000000e+00 Y=0.000000e+00 Z=0.000000e+00 X2=0.000000e+00 Y2=0.000000e+00 Z2=0.000000e+00 
>HMEAS ID=1003.0001 CHTYPE=HY X=0.000000e+00 Y=0.000000e+00 Z=0.000000e+00 X2=0.000000e+00 Y2=0.000000e+00 Z2=0.000000e+00 
>HMEAS ID=1004.0001 CHTYPE=HZ X=0.000000e+00 Y=0.000000e+00 Z=0.000000e+00 X2=0.000000e+00 Y2=0.000000e+00 Z2=0.000000e+00 
```

In the example above distances for the coils are NOT given.<br>
In this case the EDI reader shall assume that HX has the same direction as EX and HY has the same direction as EY.<br>
HZ is positive down.

For ZROT and TROT the EDI format is ambiguous. The ROT=NONE option should cover the case that the data is processed in the direction of the physical setup (ProcMT).<br>
And the option ROT=ZROT the case that the tensor is rotated in a certain direction - for example Swift's angle. <br>
ROT=ZROT however can also simply mean that the data is rotated in the direction of the physical setup. That is the case using ProcMT and the direction of the physical setup is repeated for all frequencies.<br>
If NO values are given for H, the Z direction is used (default).

```{note} 
Experienced professionals use N-S E-W setup only. "Specialists" wait for crashes.
```

More documentation can be found in our [manuals](https://manuals.geo-metronix.de/).

### 2025

Digital filtering has been adjusted to the new atss standard.The filter delay is shifted by one point.

So for a 71 length FIR filter at 512 Hz I shift 477 points <br>
(old version was 476 points) <br>
Simply filter your *source* runs again if you want to be compatible in future. 

[^1]: You can open markdown files with VSCODE & preview extension in order to get a better view.