Screen definitions for 
* variables displayed at one specific screen and 
* their respective memory address as configured in `../see memory_laout.h`.

Instanciate 
* a cached state,
* the variable list, and 
* their respective addresses.  

Power supply unit screen example: 
```
namespace dgus_origin {
namespace psu_control {

extern const uint16_t screen_variables[] PROGMEM;
extern CachedState cached_state;
struct Addresses {
  static constexpr uint16_t VP_PSU_CONTROL = 0x2004;
};

}
}
```