This repo. contains a variety of Windows kernel-mode drivers developed to demonstrate various rootkit basic functions. 
The idea is that these can then be combined to provide a wide array of desired functionality.

- **HelloWorls**
  - Simple windows kernel-mode driver for just printing hellow world to debug view. Meant to showcase windows driver boiler plate
- **IOCTL**
  - Tech demo for demonstrating user-mode to kernal communications using IOCTL calls
- **KAPC_Injection**
  - KAPC_Injection is a Windows kernel-mode driver that injects a dll into all or a subset of applicaitons running on a system. The injection is done via kernel APC injection.
- **WindowsFilteringPlatform**
  - Driver showcasing how Windows Filtering Platform (WFP) can be used to intercept communications, both to listen and to block
 
-------
The projects in this repository is meant for inspiration and as a starting off point for future projects. It also aims to serve as psudu documentaiton for future projects.
