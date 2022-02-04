Haier AC integration for Home Assistant with ESPHome

Based on the work of https://github.com/albetaCOM/esp-haier and other home assitant forum users

Replaces the "ESP32 for Haier" wifi dongle and hOn app with an ESPHome device.  Still in developement beta, only tested by me, but testing and feedback is welcome. Proper readme coming soon. This adds a climate entity, and switch entities for the other functions that we know how to control in Home Assistant. Not sure on model compatibility, but if it uses the hOn app and ESP32 for Haier dongle, it might work. 

Currently known Byte and Bit address definitions and information on the message format can be found in the haier.h file. We receive the current status from the ac and send it back with our changes to the settings. The status message contains more info then we can send in the control message, so some bytes (after 21) we cant control, only read. 

For development I have added a few "tester" entities, these allow you to view and toggle bits from HA. This can be used for finding out what unknown bits in the message do, there are sill a lot of them. Testing for this is welcome, different models may use the bits for different things based on features that my model didnt have for testing. ESPhome debug log is enabled to help with this, it will show you what bytes and bits change when they do. If you dont need them, feel free to remove them from the yaml. 


Health aka Purify, display on/off, Remote Lock, Self Clean, Steri Clean, display Units are all currently implemented as switches in HA.

Quiet, Boost, Away, and Sleep are implemented as Presets in HA. 



Things known to not be implemented yet are controling the wifi light on the ac unit, timers, potentially a few position settings, and fully understanding the message format. 
