# xvcd-ch32v307
 用于ch32v307的XVC服务端  
 源码修改基于(https://github.com/AIOT-CAT/xvcd-ch347) (https://github.com/openwch/usb-jtag-spi)  
 ch32v307 usb2jtag固件源自(https://github.com/openwch/usb-jtag-spi)

# 使用教程
 1. 编译  
 使用gcc编译:gcc io_ch32v307.c xvcd_win.c -lws2_32 -o xvcd_ch32v307  
 使用msvc编译:cl /Zi /source-charset:utf-8 .\io_ch32v307.c .\xvcd_win.c /o xvcd_ch32v307 /link ws2_32.lib  
 2. 默认不使用SPI进行data shift,启动时添加--force-spi参数可以启用SPI。测试使用SPI的烧录速率大概是不使用SPI的两倍  
 3. 测试使用 (https://github.com/openwch/usb-jtag-spi) 中提供的openocd程序烧录2MB的固件仅需要1s不到，但是使用Vivado自带的烧录功能需要54s左右，有经验的朋友可以尝试修改一下。
