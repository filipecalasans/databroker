# Data Broker

It is a data broker that allows exchanging data between distributed modules. 
Modules are distributed entities able to exchange data and keep state synchnization between them. 
This broker is target to applications where data must be shared among modules 
and there must exist state consistence between them. 
Applications such as distributed simulation, simulators, multi-agent systems and multiplayer game may apply for use this broker.

# How does it work ?

Modules provide and consume data periodically using the data connection interface.
The broker builds data routes using the module configuration file, which describes the data needed by each module. 
The same principle is used in the Control communication channel, used to formward command packet between modules. 
Commands are a special kind of data, wich are not produced periodically and may be used to module synchronization, 
module configuration or to delivery low rate data.

# Configuration 

## Modules Configuration

   ```
{
   "id": "radio",
   "name":"Radio Communication",
   "description": "This module receives and send data throught RF to the robots",
   "ip": "127.0.0.1",
   "port_data":6002,
   "port_control":6003,
   "socket_type":"udp",

   "data_published": [ 
      {
         "id": "vx0",
         "name": "Estimated position X",
         "description": "Measured Velocity X from robot 0",
         "unit": "m/s"
      },
      {
         "id": "vy0",
         "name": "measured velocity Y",
         "description": "Measeured velocity Y from Robot 0",
         "unit": "m/s"
      },
      {
         "id": "ball_sensor",
         "name": "Ball Pocession Sensor ",
         "description": "Indicates if the robot got the ball procession",
         "unit": "True of False"
      }
   ],

   "data_consumed": [ 
      {
         "source": "ai",
         "id": "vx0" 
      },
      {
         "source": "ai",
         "id": "vy0" 
      }
   ],

   "commands_consumed": [ 
      {
         "source": "ai",
         "command": "kick"
      }
   ]	
}

   ```

## Broker Configuration




# Dependencies
 - [Qt >= 5.4](https://www.qt.io/)
 - [Google Protobuffer](https://developers.google.com/protocol-buffers/)
