Display handler that 
* are either used only in dgus_origin, or
* differently flavoured than the implementation in `../../DGUSDisplay.h`.

Define 
* a cached state (if any needed), and the 
* necessary handler for the file context.
 
Power supply unit example: 
```
namespace dgus_origin {
namespace psu_control {

union CachedState {
  static constexpr uint8_t UNSET = 0;
  static constexpr uint8_t ENABLE = 1;
  static constexpr uint8_t DISABLE = 2;
  struct {
    // flags set by display
    uint8_t enable_disable : 2;
    uint8_t _unused : 6;
    // flags set internally
    uint8_t enabled_disabled : 2;
    uint8_t _unused2 : 6;
  } __attribute__((packed));
  uint16_t data;
};

void handle_psu_xxx(DGUS_VP_Variable &var, void *val_ptr);
void handle_psu_yyy(DGUS_VP_Variable &var, void *val_ptr);

}
}
```

The cached state not only encapsulates the handler's data, 
it also explicitely states the structure it expects and comes in handy if more than one `uint16_t` data field must be stored for a meaningful operation. 
For example color LED handling which needs six bytes for handling.
 