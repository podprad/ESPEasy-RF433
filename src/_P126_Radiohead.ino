#include "_Plugin_Helper.h"
#ifdef USES_P126

#include <RH_ASK.h>
#include "SPI.h"

// #######################################################################################################
// #################################### Plugin 033: Dummy ################################################
// #######################################################################################################

#define PLUGIN_126
#define PLUGIN_ID_126 126
#define PLUGIN_NAME_126 "Custom - Radiohead RX"
#define PLUGIN_VALUENAME1_126 "Value1"

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
    Device[deviceCount].VType = Sensor_VType::SENSOR_TYPE_STRING;
    Device[deviceCount].Ports = 0;
    Device[deviceCount].PullUpOption = false;
    Device[deviceCount].InverseLogicOption = false;
    Device[deviceCount].FormulaOption = false;
    Device[deviceCount].ValueCount = 1;
    Device[deviceCount].SendDataOption = true;
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

  case PLUGIN_GET_DEVICEVALUENAMES:
  {
    // FIXME TD-er: Copy names as done in P026_Sysinfo.ino.
    strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_126));
    break;
  }

  case PLUGIN_INIT:
  {
    int rxPin = Settings.TaskDevicePin1[event->TaskIndex];
    int txPin = Settings.TaskDevicePin2[event->TaskIndex];

    if (rxPin < 0 || txPin < 0){
      addLogMove(LOG_LEVEL_INFO, F("Radiohead: no pins selected"));
      success = false;
      break;
    }

    driverRef = new RH_ASK(2000, rxPin, txPin);

    if (!driverRef->init()){
      addLogMove(LOG_LEVEL_INFO, F("Radiohead: init failed"));
      delete driverRef;
      driverRef = NULL;
      success = false;
      break;      
    }

    addLogMove(LOG_LEVEL_INFO, F("Radiohead: init ok"));

    success = true;
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
      addLogMove(LOG_LEVEL_INFO, (char*)buffer);
      Scheduler.schedule_task_device_timer(event->TaskIndex, millis() + 10);
    }

    break;
  }  

  case PLUGIN_READ:
  {
    addLogMove(LOG_LEVEL_INFO, F("Radiohead: PLUGIN_READ"));

    event->String2.clear();
    event->String2.concat((char*)buffer, (int)bufferLength);

    addLogMove(LOG_LEVEL_INFO, event->)

    break;
  }
  
  }
  return success;
}

#endif // USES_P033
