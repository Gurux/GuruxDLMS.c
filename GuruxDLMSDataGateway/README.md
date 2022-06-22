See An [Gurux](http://www.gurux.org/ "Gurux") for an overview.

Join the Gurux Community or follow [@Gurux](https://twitter.com/guruxorg "@Gurux") for project updates.

GuruxDLMS.c library is a high-performance ANSI C component that helps you to read you DLMS/COSEM compatible electricity, gas or water meters. 
We have try to make component so easy to use that you do not need understand protocol at all.

For more info check out [Gurux.DLMS](http://www.gurux.fi/index.php?q=Gurux.DLMS "Gurux.DLMS").

We are updating documentation on Gurux web page. 

Read should read [DLMS/COSEM FAQ](https://www.gurux.org/index.php?q=DLMSCOSEMFAQ) first to get started. Read Instructions for making your own [meter reading application](https://www.gurux.org/index.php?q=DLMSIntro) or build own 
DLMS/COSEM [meter/simulator/proxy](https://www.gurux.fi/Gurux.DLMS.Server).

If you have problems you can ask your questions in Gurux [Forum](https://www.gurux.org/forum).

If you find an issue, please report it here:
http://www.gurux.fi/fi/project/issues/gurux.dlms.c

Purpose
=========================== 
Gurux DLMS gateway (GW) is an example that allows HES to communiate with GW using WRAPPER frames. 
GW communicates with the meter using serial port connection and HDLC framing.
This example is a good starting point when HDLC frames are needed to ignored using Over The Air communication channels.

Before start
=========================== 
Before start you need to copy files from development src and include folder to dlms folder.

Command line parameters
=========================== 

-S parameter defines used serial port.
-p parameter defines listened TCP/IP port.
-t parameter defines used trace level (Error, Warning, Info, Verbose).