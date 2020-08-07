This folder contains screen bindings for variables and the corresponding memory address (on display). 
Screen files specifies
* list(s) of variables to be pushed to one specific screen while it is active and 
* their respective memory address as configured in `../see memory_laout.h`.

Instanciate 
* a cached state (if needed by the handler),
* the variable list, and 
* their respective addresses.  

Power supply unit screen example: 
```
// header file

namespace dgus_origin {
namespace psu_control {

// screen variables
extern const uint16_t screen_variables[];
// cached state
extern CachedState cached_state;

} // namespace psu_control
} // namespace dgus_origin

// cpp file

namespace dgus_origin {
namespace psu_control {

CachedState cached_state{0};
const uint16_t screen_variables[] PROGMEM{to_address(dgus::memory_layout::PowerSupplyUnit::Control), 0x0000};

} // namespace psu_control
} // namespace dgus_origin

```