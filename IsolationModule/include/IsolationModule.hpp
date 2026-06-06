

#ifndef INC_ISOLATION_MODULE_H_
#define INC_ISOLATION_MODULE_H_

#include <cstdint>
#include "PowerMuxModule.hpp"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}

namespace IsolationModule
{
    typedef bool (*SendDataFunc)(const uint32_t id, const uint64_t data);

    class IsolationController
    {
    public:
        IsolationController(SendDataFunc SendFunction);        
        ~IsolationController();
        bool checkIsolation(uint8_t ConnID);

    private:
        SendDataFunc sendFunc;
    };

} // namespace IsolationModule

extern IsolationModule::IsolationController *iso;

#endif // __cplusplus

#endif /* INC_ISOLATION_MODULE_H_ */