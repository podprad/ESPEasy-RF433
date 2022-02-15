#include "_Plugin_Helper.h"

#ifdef USES_P126

#include "src/Globals/EventQueue.h"
#include <RH_ASK.h>
#include "SPI.h"

// #######################################################################################################
// #################################### Plugin 126: RF 433 Events ########################################
// #######################################################################################################

#define PLUGIN_126
#define PLUGIN_ID_126 126
#define PLUGIN_NAME_126 "Custom - RF 433 Events"

RH_ASK* driverRef = NULL;
uint8_t buffer[128];
uint8_t bufferLength = sizeof(buffer);


boolean Plugin_126(uint8_t function, struct EventStruct *event, String &string)
{
  boolean success = false;

  switch (function)
  {
  case PLUGIN_DEVICE_ADD:
  {
    Device[++deviceCount].Number = PLUGIN_ID_126;
    Device[deviceCount].Type = DEVICE_TYPE_DUAL;
    Device[deviceCount].VType = Sensor_VType::SENSOR_TYPE_NONE;
    Device[deviceCount].Ports = 0;
    Device[deviceCount].PullUpOption = false;
    Device[deviceCount].InverseLogicOption = false;
    Device[deviceCount].FormulaOption = false;
    Device[deviceCount].ValueCount = 0;
    Device[deviceCount].SendDataOption = false;
    Device[deviceCount].TimerOption = false;
    Device[deviceCount].TimerOptional = true;
    Device[deviceCount].GlobalSyncOption = true;
    Device[deviceCount].OutputDataType = Output_Data_type_t::All;
    break;
  }

  case PLUGIN_GET_DEVICENAME:
  {
    string = F(PLUGIN_NAME_126);
    break;
  }

  case PLUGIN_INIT:
  {
    int rxPin = Settings.TaskDevicePin1[event->TaskIndex];
    int txPin = Settings.TaskDevicePin2[event->TaskIndex];

    if (rxPin < 0 || txPin < 0){
      addLogMove(LOG_LEVEL_INFO, F("RF 433 Events: no pins selected"));
      success = false;
      break;
    }

    driverRef = new RH_ASK(2000, rxPin, txPin);

    if (!driverRef->init()){
      addLogMove(LOG_LEVEL_INFO, F("RF 433 Events: init failed"));
      delete driverRef;
      driverRef = NULL;
      success = false;
      break;      
    }

    addLogMove(LOG_LEVEL_INFO, F("RF 433 Events: init ok"));

    success = true;
    break;
  }

  case PLUGIN_EXIT:
  {
    if (driverRef != NULL){
      delete driverRef;
      driverRef = NULL;
    }
    
    break;
  }

  case PLUGIN_FIFTY_PER_SECOND:
  {
    if (!driverRef)
    {
      success = false;
      break;
    }

    memset(&buffer, 0, sizeof(buffer));
    
    if (driverRef->recv(buffer, &bufferLength)){
      String eventString;
      eventString.concat((char*)buffer, bufferLength);
      eventString.trim();
      eventString.toLowerCase();

      addLogMove(LOG_LEVEL_INFO, String(F("RF 433 Events: received ")) + eventString);
      
      String command = parseString(eventString, 1);
      if (command == F("event")){
        String newEvent = parseStringToEnd(eventString, 2);
        addLogMove(LOG_LEVEL_INFO, String(F("RF 433 Events: Trigerring event ")) + newEvent);
        eventQueue.addMove(std::move(newEvent));
      }
    }

    break;
  }  
  
  }
  return success;
}

#endif // USES_P033
