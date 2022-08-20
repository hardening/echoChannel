# Server-side echo channel

This is an implementation of the RDP ECHO channel. It has some neat options to test dynamic and static channels, sending
a given amount of packet and then close the channel.

## Build

This is meant to be built with mingw with the following commands:

``` console
# mkdir -p output
# meson setup --cross x86_64-w64-mingw32.txt output
# meson compile -C output
```

## Running

The echo channel has various options:

* `-static` opens the echo channel in static mode instead of the default dynamic mode
* `--packets=nb` allows to write `nb` packets and then exit (mainly to test channel closing on the client side)
