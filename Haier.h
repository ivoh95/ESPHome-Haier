#ifndef HAIER_ESP_HAIER_H
#define HAIER_ESP_HAIER_H

#include "esphome.h"
#include <string>

using namespace esphome;
using namespace esphome::climate;

//message formatting
#define HEADER						255 // first two bytes are 255
//another header here
#define MESSAGE_LENGTH_OFFSET		2// our message length
//3 we have a 64 next except in the init message, then five 0s
//4 0 here
//5 0 here
//6 0 here
//7 0 here
//8 0 here
#define MESSAGE_TYPE_OFFSET			9 //tells us what type of message it is
//10 not sure 
//11 not sure
// control command bits and bytes start here, dead bits immediatly revert, unknown bits stay on until the remote is used which turns them off, these are the bits we mess with
#define SET_POINT_OFFSET 			12// 0x00 is 16 degrees C, offset of 16c for the set point. 
#define VERTICAL_SWING_OFFSET		13
	#define VERTICAL_SWING_MSK 				0x0F
	#define VERTICAL_SWING_MAX_UP			0x02
	#define VERTICAL_SWING_UP				0x04
	#define VERTICAL_SWING_CENTER			0x06
	#define VERTICAL_SWING_DOWN				0x08
	#define VERTICAL_SWING_HEALTH_UP		0x01
	#define VERTICAL_SWING_HEALTH_DOWN		0x03
	#define VERTICAL_SWING_AUTO 			0x0C
#define MODE_FAN_OFFSET 			14
	#define MODE_MSK			0xF0
	#define MODE_AUTO       	0x00
	#define MODE_DRY			0x40
	#define MODE_COOL			0x20
	#define MODE_HEAT			0x80
	#define MODE_FAN			0xC0
    #define FAN_MSK				0x0F
	#define FAN_LOW	    		0x03
	#define FAN_MID		  		0x02
	#define FAN_HIGH	     	0x01
	#define FAN_AUTO	   		0x05
//15 not tested, not sure
#define STATUS_DATA1_OFFSET 		16 
	#define AWAY_BIT				(0)//away mode for 10c
	#define DISPLAY_BIT				(1)//if the display is on or off
	#define DEAD_BIT				(2)
	#define DEAD_BIT				(3)
	#define DEAD_BIT				(4)
	#define F_BIT					(5)//switches the display to f instead of c
	#define DEAD_BIT				(6)
	#define SELF_CLEAN56_BIT		(7)//starts a cleaning cycle clean 56
#define STATUS_DATA2_OFFSET			17 // Purify/Quiet mode/OnOff/...
	#define POWER_BIT				(0)	
	#define PURIFY_DATA2_BIT		(1)
	#define UNKNOWN_BIT      		(2) //remote turns it off
	#define FAST_BIT				(3)
	#define QUIET_BIT				(4)	
	#define SLEEP_BIT				(5)
	#define LOCK_BIT				(6) //disables remote
	#define DEAD_BIT				(7) 
#define UNKOWN_BYTE					18 
	#define UNKNOWN_BIT				(0) 
	#define UNKNOWN_BIT				(1)
	#define UNKNOWN_BIT				(2)
	#define UNKNOWN_BIT				(3)
	#define UNKNOWN_BIT				(4)
	#define UNKNOWN_BIT				(5)
	#define DEAD_BIT				(6)
	#define DEAD_BIT				(7)
#define HORIZONTAL_SWING_OFFSET		19
	#define HORIZONTAL_SWING_MSK			0x0F
	#define HORIZONTAL_SWING_CENTER 		0x00
	#define HORIZONTAL_SWING_MAX_LEFT 		0x03
	#define HORIZONTAL_SWING_LEFT 			0x04
	#define HORIZONTAL_SWING_MAX_RIGHT 		0x06
	#define HORIZONTAL_SWING_RIGHT 			0x05
	#define HORIZONTAL_SWING_AUTO 			0x07
//20 not tested, not sure
#define STATUS_DATA3_OFFSET 		21 //cleaning and purify related this is the last byte we control in the control command since they are of size 20
	#define PURIFY_DATA3_BIT0		(0) //turns on purify light/ different purify mode???? turning on the other health mode turns this one off
	#define DEAD_BIT				(1)
	#define PURIFY_DATA3_BIT1		(2) //turns on with purify? comes on with the bit from data 2
	#define PURIFY_DATA3_BIT2		(3)//turns on with purify? comes on with the bit from data 2
	#define SELF_CLEAN_BIT			(4) //self clean, not 56. 
	#define DEAD_BIT				(5) 
	#define DEAD_BIT				(6)
	#define DEAD_BIT				(7) 
//from here down is status only, our control message ended at 21 
#define TEMPERATURE_OFFSET   		22 //multiply by 2, 0.5c steps
//23 only seen as 0
//24, seems to be a measurement of some kind, hangs out around 60-70, gets lower after running for a while, temperature?
#define COMPRESSSOR_BYTE					27//seeems to be 1 in off, 3 in heat? changeover valve?

//message types seen and used
#define SEND_TYPE_POLL				115//next message is poll, enables command and poll messages to be replied to
#define RESPONSE_TYPE_POLL          116 //message was received

#define SEND_POLL					1 //send a poll
#define RESPONSE_POLL				2 //response of poll
#define ERROR_POLL					3//the ac sends this when it didnt like our last command message

#define SEND_TYPE_WIFI				252 //next message is wifi, enables wifi messages to be replied to, for the signal strength indicator, disables poll messages
#define RESPONSE_TYPE_WIFI          253 //confirmed
#define SEND_WIFI                   247 // current signal strength, no reply to this

#define SEND_COMMAND               	96//send a control command, no reply to this
#define INIT_COMMAND                97//this enables coms? magic message

#define MAX_MESSAGE_SIZE				64 //64 should be enough to cover largest messages we send and receive, for now. 
#define CRC_OFFSET(message)			(message[2])
#define HEADER_LENGTH				3
#define CONTROL_COMMAND_OFFSET  	11 //where our changable data starts in the control message
#define POLY 0xa001 //used for crc
#define NO_MSK						0xFF


// temperatures supported by AC system
#define MIN_SET_TEMPERATURE 16
#define MAX_SET_TEMPERATURE 30

byte getChecksum(const byte * message) {
		byte frame_length = message[MESSAGE_LENGTH_OFFSET];
        byte crc = 0;
        
        for (int i = MESSAGE_LENGTH_OFFSET; i <= (frame_length+1); i++){
            crc += message[i];
		}
        return crc;
    }

 unsigned crc16(unsigned crc, unsigned char *buf, size_t len)
    { 
        while (len--) {
            crc ^= *buf++;
            crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
            crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
            crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
            crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
            crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
            crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
            crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
            crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
        }
     	  return crc;
    }
    String getHex(byte * message, byte size) {
        String raw;

        for (int i=0; i < size; i++){
			raw += " " + String(message[i]);
        }
        raw.toUpperCase();
        return raw;
    }

void sendData(byte * message) {
		byte size = message[MESSAGE_LENGTH_OFFSET]; 
		size += 5; 
        byte crc_offset = size-3;
        byte crc = getChecksum(message);
        word crc_16 = crc16(0, &(message[2]), crc_offset-2);
    
        message[crc_offset] = crc;
        message[crc_offset+1] = (crc_16>>8)&0xFF;
        message[crc_offset+2] = crc_16&0xFF;

        Serial.write(message, size);
        auto raw = getHex(message, size);
        ESP_LOGD("Haier", "Message sent: %s  - CRC: %d - CRC16: %X ", raw.c_str(), crc, crc_16);

    }
	

class Haier : public Climate, public PollingComponent {

private:

    byte status[MAX_MESSAGE_SIZE];//where we hold our status message
	byte initialization_1[15] = {HEADER,HEADER,10,0,0,0,0,0,0,INIT_COMMAND,0,7}; //enables coms? magic message, needs some timing pause after this?
	byte type_poll[12] = {HEADER,HEADER,8,64,0,0,0,0,0,SEND_TYPE_POLL}; //sets message type, we only use the poll type for now
	byte type_wifi[12] = {HEADER,HEADER,8,64,0,0,0,0,0,SEND_TYPE_WIFI};
	byte wifi_status[17] = {HEADER,HEADER,12,64,0,0,0,0,0,SEND_WIFI,0,0,0,50}; //last byte is signal strength, second to last goes to 1 when trying to connect, not used for now
	byte poll[15] = {HEADER,HEADER,10,64,0,0,0,0,0,SEND_POLL,77,1};//ask for the status

	byte climate_mode_fan_speed = FAN_AUTO;
	byte fan_mode_fan_speed = FAN_MID;

	// Some vars for debuging purposes	
	byte previous_status[MAX_MESSAGE_SIZE];
	bool first_status_received = false;
	// Functions

	bool GetControlBit(byte byte_number, byte bit_number)
	{
		return bitRead(id(control_command)[byte_number], bit_number);;
	}

	void SetControlBit(byte byte_number, byte bit_number, bool bit_state){
		bitWrite(id(control_command)[byte_number], bit_number, bit_state);
	}
	
	byte GetControlByte(byte byte_number, byte MSK)
	{
		return (id(control_command)[byte_number] & MSK); 
	}

	void SetControlByte(byte byte_number, byte MSK, byte setting){
		id(control_command)[byte_number] &= ~MSK;
		id(control_command)[byte_number] |= setting;
	}

	void CompareStatusByte()
	{		
		for (int i=0;i<(status[MESSAGE_LENGTH_OFFSET]);i++)
		{
			if(status[i] != previous_status[i]){
				ESP_LOGD("Debug", "Changed Byte %d: 0x%X --> 0x%X ", i, previous_status[i],status[i]);
				for (int j=0; j<8;j++)
				{
					if(bitRead(status[i],j) != bitRead(previous_status[i],j))
						ESP_LOGD("Debug", "Changed BIT: %d", j);
				}
			}
			previous_status[i] = status[i];
		}
	}


public:

    Haier() : PollingComponent(5 * 1000) {
    }

    
    void setup() override {
        
        Serial.begin(9600);
		Serial.setTimeout(100); //give up after 100ms if we are trying to read a message and it doesnt arrive in the loop. 
		delay(2000); //wait for the ac to boot, esp boots faster then the AC
		sendData(initialization_1);//start the comms?
		sendData(type_poll);//enable polling and commands?
		delay(2000);//wait for the ac, if we dont wait it wont be ready, and it wont init. 
	
    }

    void loop() override  {
		byte data[MAX_MESSAGE_SIZE]={255,255,0}; //set the two header bytes and a null length

		if (Serial.available()>13){//we want at least 13 bytes, smallest message?
    	    if (((Serial.read() == HEADER) && (Serial.read() == HEADER))) { //keep looking for two header bytes

				while (Serial.peek() == HEADER) Serial.read();//get rid of any header bytes that may have gotten past due to serial timing

				byte message_length  = Serial.read();//next byte should be the message length
				data[MESSAGE_LENGTH_OFFSET] = message_length;//set it in the array
        	    Serial.readBytes(&data[HEADER_LENGTH], message_length);//read bytes after the header, read up till the crc, we dont verify the crc 16 so we dont read those bytes
				message_length += MESSAGE_LENGTH_OFFSET; //Include the bytes from before our length byte into our length

       			byte check = getChecksum(data); //calculate the checksum

				if (check != data[message_length]) {//check the message to verify its checksum matches
           			ESP_LOGW("Haier", "Invalid checksum (%d vs %d)", check, data[message_length]); //show checksum issue
					auto raw = getHex(data, message_length); //show the message
        			ESP_LOGW("Haier", "Invalid message L: %d: :%s ", message_length, raw.c_str());
           			return;
        		}
				else if (data[MESSAGE_TYPE_OFFSET] == RESPONSE_POLL) {
					auto raw = getHex(data, message_length); //show the message without checksum 
					ESP_LOGD("Haier", "Received Status Message: %s ", raw.c_str()); 
					memcpy(status, data, message_length);
					parseStatus();
				}
				else if (data[MESSAGE_TYPE_OFFSET] == RESPONSE_TYPE_WIFI) {
					ESP_LOGD("Haier", "Received Reponse to WiFi Type Message:"); 
					sendData(wifi_status); //not implemented yet, since we never send the message to get into wifi message type
				}
				else if (data[MESSAGE_TYPE_OFFSET] == RESPONSE_TYPE_POLL) {
					ESP_LOGD("Haier", "Received Reponse to Poll Type Message:"); 
					//not implemented yet, since we never send the message except once during init
				}
				else if (data[MESSAGE_TYPE_OFFSET] == ERROR_POLL) {
					ESP_LOGD("Haier", "Command message received, but it had invalid settings"); 
				}
				else{
					auto raw = getHex(data, message_length);
					ESP_LOGD("Haier", "Received Unknown Message: %s ", raw.c_str());
				}
			}
    	}
	}

    void update() override {
	
        ESP_LOGD("Haier", "POLLING");
		if (!first_status_received){//if we havent gotten our first status
		ESP_LOGD("Haier", "First Status Not Received");
		}
        sendData(poll);
    }

protected:
    ClimateTraits traits() override {
        auto traits = climate::ClimateTraits();
        traits.set_supported_modes(
        {
            climate::CLIMATE_MODE_OFF,
            climate::CLIMATE_MODE_COOL,
            climate::CLIMATE_MODE_HEAT,
            climate::CLIMATE_MODE_FAN_ONLY,
            climate::CLIMATE_MODE_DRY,
            climate::CLIMATE_MODE_AUTO
        });

        traits.set_supported_fan_modes(
        {
            climate::CLIMATE_FAN_AUTO,
            climate::CLIMATE_FAN_LOW,
            climate::CLIMATE_FAN_MEDIUM,
            climate::CLIMATE_FAN_HIGH,
        });

        traits.set_supported_swing_modes(
        {
            climate::CLIMATE_SWING_OFF,
            climate::CLIMATE_SWING_BOTH,
            climate::CLIMATE_SWING_VERTICAL,
            climate::CLIMATE_SWING_HORIZONTAL
        });
		traits.set_supported_presets(
		{
			climate::CLIMATE_PRESET_NONE,
			climate::CLIMATE_PRESET_ECO,
			climate::CLIMATE_PRESET_BOOST,
			climate::CLIMATE_PRESET_SLEEP,
			climate::CLIMATE_PRESET_AWAY
			
		});

        traits.set_visual_min_temperature(MIN_SET_TEMPERATURE);
        traits.set_visual_max_temperature(MAX_SET_TEMPERATURE);
        traits.set_visual_temperature_step(0.5f); //displays current temp in 0.5 degree steps, we cannot change it in 0.5 degree steps however
        traits.set_supports_current_temperature(true);
        return traits;
    }

public:

    void parseStatus() {

		//update all statuses in the control message from what we recieve from the status message
		for (int i=CONTROL_COMMAND_OFFSET;i<(status[MESSAGE_LENGTH_OFFSET]);i++)
		{
			id(control_command)[i] = status[i];
		}

		ESP_LOGD("Debug", "HVAC Mode = 0x%X", GetControlByte(MODE_FAN_OFFSET, MODE_MSK));
		ESP_LOGD("Debug", "Fan speed Status = 0x%X", GetControlByte(MODE_FAN_OFFSET, FAN_MSK));
		ESP_LOGD("Debug", "Horizontal Swing Status = 0x%X", GetControlByte(HORIZONTAL_SWING_OFFSET, HORIZONTAL_SWING_MSK));
		ESP_LOGD("Debug", "Vertical Swing Status = 0x%X", GetControlByte(VERTICAL_SWING_OFFSET, VERTICAL_SWING_MSK));
		ESP_LOGD("Debug", "Set Point Status = 0x%X", GetControlByte(SET_POINT_OFFSET, NO_MSK));

		if(first_status_received) //dont compare before we have gotten our first state
			CompareStatusByte();
		
		if (preset == CLIMATE_PRESET_AWAY) target_temperature = 10; //away mode sets to 10c
        else target_temperature = (GetControlByte(SET_POINT_OFFSET, NO_MSK)+16); //offset of 16

		current_temperature = (GetControlByte(TEMPERATURE_OFFSET, NO_MSK) / 2); //divide by 2

		//remember the fan speed we last had for climate vs fan
		if(GetControlByte(MODE_FAN_OFFSET, MODE_MSK) == MODE_FAN){
			fan_mode_fan_speed = GetControlByte(MODE_FAN_OFFSET, FAN_MSK);
		}
		else{
			climate_mode_fan_speed = GetControlByte(MODE_FAN_OFFSET, FAN_MSK);
		}

		switch (GetControlByte(MODE_FAN_OFFSET, FAN_MSK)) {
                    case FAN_AUTO:
                        fan_mode = CLIMATE_FAN_AUTO;
                        break;
                    case FAN_MID:
                        fan_mode = CLIMATE_FAN_MEDIUM;
                        break;
					case FAN_LOW:
						fan_mode = CLIMATE_FAN_LOW;
                        break;
                    case FAN_HIGH:
                        fan_mode = CLIMATE_FAN_HIGH;
                        break;
        }
		//climate mode
        if (GetControlBit(STATUS_DATA2_OFFSET, POWER_BIT) == false) {
            mode = CLIMATE_MODE_OFF;
		} 
		else {
			// Check current hvac mode
            switch (GetControlByte(MODE_FAN_OFFSET, MODE_MSK)) {
                case MODE_COOL:
                    mode = CLIMATE_MODE_COOL;
                    break;
                case MODE_HEAT:
                    mode = CLIMATE_MODE_HEAT;
                    break;
                case MODE_DRY:
				    mode = CLIMATE_MODE_DRY;
					break;
				case MODE_FAN:
                    mode = CLIMATE_MODE_FAN_ONLY;
                    break;
                case MODE_AUTO:
				     mode = CLIMATE_MODE_AUTO;
            }
		}		

		//extra modes/presets
		if (GetControlBit(STATUS_DATA2_OFFSET, QUIET_BIT)) {
               preset = CLIMATE_PRESET_ECO;
        }
		else if(GetControlBit(STATUS_DATA2_OFFSET, FAST_BIT)) {
			preset = CLIMATE_PRESET_BOOST;
		}
		else if (GetControlBit(STATUS_DATA2_OFFSET, SLEEP_BIT)) {
            preset = CLIMATE_PRESET_SLEEP;
        }
		else if(GetControlBit(STATUS_DATA1_OFFSET, AWAY_BIT)) {
			preset = CLIMATE_PRESET_AWAY;
		}
		else{
			preset = CLIMATE_PRESET_NONE;
		}			

		if((GetControlByte(HORIZONTAL_SWING_OFFSET, HORIZONTAL_SWING_MSK) == HORIZONTAL_SWING_AUTO) && (GetControlByte(VERTICAL_SWING_OFFSET, VERTICAL_SWING_MSK) == VERTICAL_SWING_AUTO) ){
			swing_mode = CLIMATE_SWING_BOTH;				
		}
		else if((GetControlByte(HORIZONTAL_SWING_OFFSET, HORIZONTAL_SWING_MSK)) == HORIZONTAL_SWING_AUTO){
			swing_mode = CLIMATE_SWING_HORIZONTAL;
		}
		else if((GetControlByte(VERTICAL_SWING_OFFSET, VERTICAL_SWING_MSK)) == VERTICAL_SWING_AUTO){
			swing_mode = CLIMATE_SWING_VERTICAL;
		}
		else{
			swing_mode = CLIMATE_SWING_OFF;
		}

        this->publish_state();
		first_status_received = true; //we have gotten our first status and published
    }


    void control(const ClimateCall &call) override {
			
		ESP_LOGD("Control", "Control call");

		if(first_status_received == false){
			ESP_LOGE("Control", "No action, first poll answer not received");
			return; //cancel the control, we cant do it without a poll answer.
		}

        if (call.get_mode().has_value()) {	
            switch (*call.get_mode()) {
                case CLIMATE_MODE_OFF:
					SetControlBit(STATUS_DATA2_OFFSET, POWER_BIT, false);
                    break;
					
                case CLIMATE_MODE_AUTO:
					SetControlBit(STATUS_DATA2_OFFSET, POWER_BIT, true);
					SetControlByte(MODE_FAN_OFFSET, MODE_MSK, MODE_AUTO);
					SetControlByte(MODE_FAN_OFFSET, FAN_MSK, climate_mode_fan_speed);
                    break;
					
                case CLIMATE_MODE_HEAT:	
					SetControlBit(STATUS_DATA2_OFFSET, POWER_BIT, true);
					SetControlByte(MODE_FAN_OFFSET, MODE_MSK, MODE_HEAT);
					SetControlByte(MODE_FAN_OFFSET, FAN_MSK, climate_mode_fan_speed);
                    break;
					
                case CLIMATE_MODE_DRY:		
					SetControlBit(STATUS_DATA2_OFFSET, POWER_BIT, true);
					SetControlByte(MODE_FAN_OFFSET, MODE_MSK, MODE_DRY);
					SetControlByte(MODE_FAN_OFFSET, FAN_MSK, climate_mode_fan_speed);					
                    break;
					
                case CLIMATE_MODE_FAN_ONLY:				
					SetControlBit(STATUS_DATA2_OFFSET, POWER_BIT, true);
					SetControlByte(MODE_FAN_OFFSET, MODE_MSK, MODE_FAN);
					SetControlByte(MODE_FAN_OFFSET, FAN_MSK, fan_mode_fan_speed); //pick a speed, auto doesnt work in fan only mode				
                    break;

                case CLIMATE_MODE_COOL:
					SetControlBit(STATUS_DATA2_OFFSET, POWER_BIT, true);
					SetControlByte(MODE_FAN_OFFSET, MODE_MSK, MODE_COOL);
					SetControlByte(MODE_FAN_OFFSET, FAN_MSK, climate_mode_fan_speed);
                    break;
            }
			
		}
		
		//Set fan speed, if we are in fan mode, reject auto in fan mode
        if (call.get_fan_mode().has_value()) {
            switch(call.get_fan_mode().value()) {
                case CLIMATE_FAN_LOW:
					SetControlByte(MODE_FAN_OFFSET, FAN_MSK, FAN_LOW);
                    break;
                case CLIMATE_FAN_MEDIUM:
					SetControlByte(MODE_FAN_OFFSET, FAN_MSK, FAN_MID);
                    break;
                case CLIMATE_FAN_HIGH:
					SetControlByte(MODE_FAN_OFFSET, FAN_MSK, FAN_HIGH);
                    break;
                case CLIMATE_FAN_AUTO:
					if (mode != CLIMATE_MODE_FAN_ONLY) //if we are not in fan only mode
						SetControlByte(MODE_FAN_OFFSET, FAN_MSK, FAN_AUTO);
                    break;
			}
		}

        //Set swing mode
        if (call.get_swing_mode().has_value()){
            switch(call.get_swing_mode().value()) {
                case CLIMATE_SWING_OFF:
					SetControlByte(HORIZONTAL_SWING_OFFSET, HORIZONTAL_SWING_MSK, HORIZONTAL_SWING_CENTER);
					SetControlByte(VERTICAL_SWING_OFFSET, VERTICAL_SWING_MSK, VERTICAL_SWING_CENTER);
                    break;
                case CLIMATE_SWING_VERTICAL:
					SetControlByte(HORIZONTAL_SWING_OFFSET, HORIZONTAL_SWING_MSK, HORIZONTAL_SWING_CENTER);
					SetControlByte(VERTICAL_SWING_OFFSET, VERTICAL_SWING_MSK, VERTICAL_SWING_AUTO);
                    break;
                case CLIMATE_SWING_HORIZONTAL:
					SetControlByte(HORIZONTAL_SWING_OFFSET, HORIZONTAL_SWING_MSK, HORIZONTAL_SWING_AUTO);
					SetControlByte(VERTICAL_SWING_OFFSET, VERTICAL_SWING_MSK, VERTICAL_SWING_CENTER);
                    break;
                case CLIMATE_SWING_BOTH:
					SetControlByte(HORIZONTAL_SWING_OFFSET, HORIZONTAL_SWING_MSK, HORIZONTAL_SWING_AUTO);
					SetControlByte(VERTICAL_SWING_OFFSET, VERTICAL_SWING_MSK, VERTICAL_SWING_AUTO);
                    break;
			}
        }
				
		if (call.get_target_temperature().has_value()) {
			SetControlByte(SET_POINT_OFFSET, NO_MSK,(*call.get_target_temperature() -16)); //set the tempature at our offset, subtract 16.
		}
		
		if (call.get_preset().has_value()) {
            switch(call.get_preset().value()) {
				case CLIMATE_PRESET_NONE:
					SetControlBit(STATUS_DATA1_OFFSET, AWAY_BIT, false);
					SetControlBit(STATUS_DATA2_OFFSET, QUIET_BIT, false);
					SetControlBit(STATUS_DATA2_OFFSET, FAST_BIT, false);
					SetControlBit(STATUS_DATA2_OFFSET, SLEEP_BIT, false);
                    break;
                case CLIMATE_PRESET_ECO:
					SetControlBit(STATUS_DATA1_OFFSET, AWAY_BIT, false);
					SetControlBit(STATUS_DATA2_OFFSET, QUIET_BIT, true);
					SetControlBit(STATUS_DATA2_OFFSET, FAST_BIT, false);
					SetControlBit(STATUS_DATA2_OFFSET, SLEEP_BIT, false);
                    break;
                case CLIMATE_PRESET_BOOST:
					SetControlBit(STATUS_DATA1_OFFSET, AWAY_BIT, false);
					SetControlBit(STATUS_DATA2_OFFSET, QUIET_BIT, false);
					SetControlBit(STATUS_DATA2_OFFSET, FAST_BIT, true);
					SetControlBit(STATUS_DATA2_OFFSET, SLEEP_BIT, false);
                    break;
				case CLIMATE_PRESET_AWAY:
					SetControlBit(STATUS_DATA1_OFFSET, AWAY_BIT, true);
					SetControlBit(STATUS_DATA2_OFFSET, QUIET_BIT, false);
					SetControlBit(STATUS_DATA2_OFFSET, FAST_BIT, false);
					SetControlBit(STATUS_DATA2_OFFSET, SLEEP_BIT, false);
                    break;
				case CLIMATE_PRESET_SLEEP:
					SetControlBit(STATUS_DATA1_OFFSET, AWAY_BIT, false);
					SetControlBit(STATUS_DATA2_OFFSET, QUIET_BIT, false);
					SetControlBit(STATUS_DATA2_OFFSET, FAST_BIT, false);
					SetControlBit(STATUS_DATA2_OFFSET, SLEEP_BIT, true);
                    break;
			}
		}
		sendData(id(control_command));  //send the data after all our changes, no need to send after each change
   }
};
#endif //HAIER_ESP_HAIER_H