INCLUDEPATH += ../BST_IDE/usb/

SOURCES +=  ../BST_IDE/usb/usbport.cpp \
            ../BST_IDE/usb/win_qextusbport.cpp \
            ../BST_IDE/usb/qextusbbase.cpp

HEADERS +=  ../BST_IDE/usb/usbport.h \
            ../BST_IDE/usb/win_qextusbport.h \
            ../BST_IDE/usb/qextusbbase.h \
            ../BST_IDE/usb/usbdi.h \
            ../BST_IDE/usb/devioctl.h \
            ../BST_IDE/usb/usbioctl.h \
            ../BST_IDE/usb/usbiodef.h \
            ../BST_IDE/usb/usb.h \
            ../BST_IDE/usb/usb200.h \
            ../BST_IDE/usb/usb100.h \
            ../BST_IDE/usb/usbuser.h

LIBS        += ../BST_IDE/usb/lib/odbc32.lib\
                ../BST_IDE/usb/lib/odbccp32.lib\
                ../BST_IDE/usb/lib/winmm.lib\
                ../BST_IDE/usb/lib/usbd.lib\
                ../BST_IDE/usb/lib/setupapi.lib\
                ../BST_IDE/usb/lib/lmusbdll.lib\
                ../BST_IDE/usb/lib/wsock32.lib
