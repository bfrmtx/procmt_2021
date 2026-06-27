# Binary Data Files

* names end with `.ats` 
* consist of a 1024-byte header
* followed by intel byte order of 32 bit integers
* amount of samples is specified in the header, and can be calculated by (file size - 1024) / 4
* contains achXmlHeader as reference to the measdoc or measdocxml file

ref raw: procmt/atsheader/atsheader_80_def.h
For newer utilities we skip the implementation of the "slice header".


## measdoc or measdocxml

* names end with `.xml` 

Typical name 098_2019-11-04_07-55-00_2018-11-07_04-00-00_R000_128H.xml: <br>

* serial of system 
* start date and time of recording
* stop date and time of recording
* run number (three digits)
* sample frequency (H or s for Hz or seconds)
* entries separated by underscores, no spaces, no colons, no dashes, no dots

entries of importance

```xml
<recording>
    <start_date>2019-11-04</start_date>
    <start_time>07:55:00</start_time>
    <stop_date>2019-11-07</stop_date>
    <stop_time>04:00:00</stop_time>
```
generated from atsheader, uiStartDateTime. stop time is is calculated with uiSamples and rSampleFreq.

The section `<ATSWriter>` is extracted from the atsheader.

```xml
<output>
  <ATSWriter>
    <configuration>
      <survey_header_name/>
      <meas_type>MT</meas_type>
      <powerline_freq1>0</powerline_freq1>
      <powerline_freq2>0</powerline_freq2>
      <channel id="0">
        <start_date>2019-11-04</start_date>
        <start_time>07:55:00</start_time>
        <sample_freq>1.28000000E+02</sample_freq>
        <num_samples>31372800</num_samples>
        <ats_data_file>098_V01_C00_R000_TEx_BL_128H.ats</ats_data_file>
        <channel_type>Ex</channel_type>
        <sensor_sernum>0</sensor_sernum>
        <sensor_type>EFP06</sensor_type>
        <ts_lsb>4.49689000E-04</ts_lsb>
        <sensor_cal_file>SENSOR.CAL</sensor_cal_file>
        <pos_x1>-4.50000000E+01</pos_x1>
        <pos_y1>0.00000000E+00</pos_y1>
        <pos_z1>0.00000000E+00</pos_z1>
        <pos_x2>4.50000000E+01</pos_x2>
        <pos_y2>0.00000000E+00</pos_y2>
        <pos_z2>0.00000000E+00</pos_z2>
      </channel>
```

What is extra in the measdocxml file is the calibration for the sensors, shortened:

```xml
<calibration_sensors>
    <channel id="1">
      <calibration/>
    </channel>
```

## Operation on Data / XML

Mostly we do filtering (decimation), un-split (concatenation) or split. <br>

If we concatenate (unsplit) we mostly take care of the start and stop time/date, and the number of samples: both in atsheader and XML. Calibration data is copied only. <br>
Same if we split, we take care of the start and stop time/date, and the number of samples: both in atsheader and XML. Calibration data is copied only. <br>

For a few tools we can modify the atsheader by carefully by overwriting:

* uiStartDateTime at 00Ch, uint32_t
* uiSamples at 004h, uint32_t
* achXmlHeader[64];  at 0C0h 

in the xml we change the start and stop date/time, and the number of samples. Calibration data is copied only. Basically we copy a master xml and change the entries.<br>

And the "evil" if we change the xml file name bacause of start/stop date/time, we also have to change the ats_data_file entry in the xml file. <br>

## Date and Time

Is always in UTC, and is formatted as `YYYY-MM-DD` for date and `HH:MM:SS` time or `HH-MM-SS` for time in file names. <br>