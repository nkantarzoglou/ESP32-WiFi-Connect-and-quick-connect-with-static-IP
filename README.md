A simple application built using Arduino IDE.

The aim of this is to allow the user to connect the ESP-32 to the local Wi-Fi network.  
But once a successful connection occurs, we store the connection data(local_IP, gateway, subnet, primaryDNS, secondaryDNS).  
This allows the device to reconnect later much faster.  
Also, there is functionality implemented to restore default values in case the quick Wi-Fi connection goes in error.

Particularly useful for battery-powered applications.
