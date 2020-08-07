This folder shall contain display handler that 
* are either used only in dgus_origin, or
* differently flavoured than the base implementation in `../../DGUSDisplay.h`.

Define 
* a cached state (if any needed), and the 
* necessary handler.
 
Power supply unit example: 
```
// header file 

namespace dgus_origin {
namespace psu_control {

union CachedState { ... }; // instanciation follows in the screen implementation
void handle_psu_xxx(DGUS_VP_Variable &var, void *val_ptr);
void handle_psu_yyy(DGUS_VP_Variable &var, void *val_ptr);

} // namespace psu_control 
} // namespace dgus_origin
```

The cached state not only encapsulates the handler's data, 
but also explicitely states the structure the handler expect.
It also and comes in handy if more than one `uint16_t` data field must be stored for a meaningful operation. 
For example color LED handling which needs six bytes for proper handling.
 