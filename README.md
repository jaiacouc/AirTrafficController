# Air Traffic Controller
> Air traffic control program that manages planes registration, taxing, and takeoff

## General Information
- This project utilizes a local server thats created in C that allows 
a multithreading to handle multiple clients that will connect and preform 
various actions
- The server acts as a airtraffic controller in the console
- The client acts as planes and as you open multiple consoles each acts as a plane

## Plane Commands

* `REG flightid` \
  Registers the plane with air traffic control

* `REQTAXI`\
   Request the plane be taxied onto the runway which inserts the plane into the queue

* `REQPOS`\
  Requests for the position of the plane in the queue
  
* `REQAHEAD`\
  Requests the plane ahead of the current plane

* `INAIR`\
  Notifies aircontrol that the plane is in the air

* `BYE`\
  Allows a plane to disconnect from the server

* `TAKEOFF`\
  Allows plane in position 1 to take off

  
