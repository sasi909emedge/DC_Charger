/* testable.h: Implementation of a testable component.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#ifndef INC_TIME_MODULE_H_
#define INC_TIME_MODULE_H_

#include <cstdint>
// #include <cstdbool>
#include <vector>
#include <array>
#include <string>
#include <stdexcept>
#include <time.h>
#include <sys/time.h>
#include "esp_log.h"
#include "esp_system.h"
#include "NVSModule.hpp"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}

namespace TimeModule
{
   namespace Constants
   {

   } // namespace Constants

   class SystemTime
   {
   public:
      SystemTime();
      ~SystemTime();

      esp_err_t setTime(time_t time);
      esp_err_t writeCurrentTimeToFlash(void);
      esp_err_t getTime(time_t *time);
      esp_err_t getTimeFromFlash(time_t *time);
      esp_err_t getTimeInOcppFormat(char *timeString);
      esp_err_t setTimeFromOcppFormat(const char *timeString);

      uint64_t GetNumofSecondsFromOcppTimeString(char *timestring);
      uint64_t Get_NumofSecondsFromCurrentTime(void);

      bool isTimeExpired(char *timestring);

      NVSModule::NVSStorage *storage;

   private:
      time_t currentTime;
   };
} // namespace NVSModule

extern TimeModule::SystemTime *SystemTime;
#endif // __cplusplus
#endif /* INC_TIME_MODULE_H_ */