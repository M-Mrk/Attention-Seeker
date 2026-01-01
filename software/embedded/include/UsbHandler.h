#ifndef USB_HANDLER_H
#define USB_HANDLER_H

class UsbHandler {
public:
    void setup();
    void startListening();
    void sendKey(char key);
};

extern UsbHandler usbHandler;

#endif