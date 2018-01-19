#include "qextusbbase.h"

const GUID GUID_CLASS_I82930_BULK = {0x8715c3bf, 0x4949, 0x4260,
                                    {0x90, 0xf1, 0xa2, 0xaa, 0x1d, 0xeb, 0xfb, 0x90}};


/*!
\fn QextUsbBase::QextUsbBase()
Default constructor.
*/
QextUsbBase::QextUsbBase()
 : QIODevice()
{
    setGUID(GUID_CLASS_I82930_BULK);
    construct();
}

/*!
\fn QextSerialBase::QextSerialBase(const QString & name)
Construct a port and assign it to the device specified by the name parameter.
*/
QextUsbBase::QextUsbBase(GUID pGuid)
 : QIODevice()
{
    setGUID(pGuid);
    construct();
}

/*!
\fn QextSerialBase::~QextSerialBase()
Standard destructor.
*/
QextUsbBase::~QextUsbBase()
{
	delete mutex;
}

/*!
\fn void QextSerialBase::construct()
Common constructor function for setting up default port settings.
(115200 Baud, 8N1, Hardware flow control where supported, otherwise no flow control, and 0 ms timeout).
*/
void QextUsbBase::construct()
{
	mutex = new QMutex( QMutex::Recursive );
	setOpenMode(QIODevice::NotOpen);
}

void QextUsbBase::setQueryMode(QueryMode mechanism)
{
	_queryMode = mechanism;
}

/*!
\fn void QextSerialBase::setPortName(const QString & name)
Sets the name of the device associated with the object, e.g. "COM1", or "/dev/ttyS0".
*/

void QextUsbBase::setGUID(GUID pGuid)
{
    m_Guid = pGuid;
}

GUID QextUsbBase::getGUID()
{
    return m_Guid;
}

/*!
\fn bool QextSerialBase::isSequential() const
Returns true if device is sequential, otherwise returns false. Serial port is sequential device
so this function always returns true. Check QIODevice::isSequential() documentation for more 
information.
*/
bool QextUsbBase::isSequential() const
{
	return true;
}

/*!
\fn bool QextSerialBase::atEnd() const
This function will return true if the input buffer is empty (or on error), and false otherwise.
Call QextSerialBase::lastError() for error information.
*/
bool QextUsbBase::atEnd() const
{
    if (size()) {
        return true;
    }
    return false;
}

/*!
\fn qint64 QextSerialBase::readLine(char * data, qint64 maxSize)
This function will read a line of buffered input from the port, stopping when either maxSize bytes
have been read, the port has no more data available, or a newline is encountered.
The value returned is the length of the string that was read.
*/
qint64 QextUsbBase::readLine(char * data, qint64 maxSize)
{
    qint64 numBytes = bytesAvailable();
    char* pData = data;

	if (maxSize < 2)	//maxSize must be larger than 1
		return -1;

    /*read a byte at a time for MIN(bytesAvail, maxSize - 1) iterations, or until a newline*/
    while (pData<(data+numBytes) && --maxSize) {
        readData(pData, 1);
        if (*pData++ == '\n') {
            break;
        }
    }
    *pData='\0';

    /*return size of data read*/
    return (pData-data);
}

/*!
\fn ulong QextSerialBase::lastError() const
Returns the code for the last error encountered by the port, or E_NO_ERROR if the last port
operation was successful.  Possible error codes are:

\verbatim
Error                           Explanation
---------------------------     -------------------------------------------------------------
E_NO_ERROR                      No Error has occured
E_INVALID_FD                    Invalid file descriptor (port was not opened correctly)
E_NO_MEMORY                     Unable to allocate memory tables (POSIX)
E_CAUGHT_NON_BLOCKED_SIGNAL     Caught a non-blocked signal (POSIX)
E_PORT_TIMEOUT                  Operation timed out (POSIX)
E_INVALID_DEVICE                The file opened by the port is not a character device (POSIX)
E_BREAK_CONDITION               The port detected a break condition
E_FRAMING_ERROR                 The port detected a framing error
                                (usually caused by incorrect baud rate settings)
E_IO_ERROR                      There was an I/O error while communicating with the port
E_BUFFER_OVERRUN                Character buffer overrun
E_RECEIVE_OVERFLOW              Receive buffer overflow
E_RECEIVE_PARITY_ERROR          The port detected a parity error in the received data
E_TRANSMIT_OVERFLOW             Transmit buffer overflow
E_READ_FAILED                   General read operation failure
E_WRITE_FAILED                  General write operation failure
\endverbatim
*/
ulong QextUsbBase::lastError() const
{
    return lastErr;
}

/*****************************************
Routine Description:
    Given the HardwareDeviceInfo, representing a handle to the plug and
    play information, and deviceInfoData, representing a specific usb device,
    open that device and fill in all the relevant information in the given
    USB_DEVICE_DESCRIPTOR structure.
Arguments:
    HardwareDeviceInfo:  handle to info obtained from Pnp mgr via SetupDiGetClassDevs()
    DeviceInfoData:      ptr to info obtained via SetupDiEnumInterfaceDevice()
Return Value:
    return HANDLE if the open and initialization was successfull,
    else INVLAID_HANDLE_VALUE.
*****************************************/
HANDLE QextUsbBase::OpenOneDevice (IN HDEVINFO                    HardwareDeviceInfo,
                                   IN PSP_INTERFACE_DEVICE_DATA   DeviceInfoData,
                                   IN char *devName )
{
    PSP_INTERFACE_DEVICE_DETAIL_DATA     functionClassDeviceData = NULL;
    ULONG                                predictedLength = 0;
    ULONG                                requiredLength = 0;
    HANDLE                               hOut = INVALID_HANDLE_VALUE;
    // allocate a function class device data structure to receive the
    // goods about this particular device.
    SetupDiGetInterfaceDeviceDetail(HardwareDeviceInfo,
                                    DeviceInfoData,
                                    NULL, // probing so no output buffer yet
                                    0, // probing so output buffer length of zero
                                    &requiredLength,
                                    NULL); // not interested in the specific dev-node
    predictedLength = requiredLength;
    functionClassDeviceData = (PSP_INTERFACE_DEVICE_DETAIL_DATA)malloc (predictedLength);
    functionClassDeviceData->cbSize = sizeof (SP_INTERFACE_DEVICE_DETAIL_DATA);
    // Retrieve the information from Plug and Play.
    if (! SetupDiGetInterfaceDeviceDetail (HardwareDeviceInfo,
                                           DeviceInfoData,
                                           functionClassDeviceData,
                                           predictedLength,
                                           &requiredLength,
                                           NULL))
    {
        free( functionClassDeviceData );
        return INVALID_HANDLE_VALUE;
    }
    char tempDevicePath[80];
    int i;
    for(i=0;i<=80;i++)
    {
        tempDevicePath[i]=functionClassDeviceData->DevicePath[i];
    }
    strcpy( devName,tempDevicePath) ;
    hOut = CreateFile(functionClassDeviceData->DevicePath,
                      GENERIC_READ | GENERIC_WRITE,
                      FILE_SHARE_READ | FILE_SHARE_WRITE,
                      NULL, // no SECURITY_ATTRIBUTES structure
                      OPEN_EXISTING, // No special create flags
                      0, // No special attributes
                      NULL); // No template file
    free( functionClassDeviceData );
    return hOut;
}

/********************************************************************
Routine Description:
   Do the required PnP things in order to find
   the next available proper device in the system at this time.
Arguments:
    pGuid:      ptr to GUID registered by the driver itself
    outNameBuf: the generated name for this device
Return Value:
    return HANDLE if the open and initialization was successful,
    else INVLAID_HANDLE_VALUE.
********************************************************************/
HANDLE QextUsbBase::OpenUsbDevice( LPGUID  pGuid, char *outNameBuf)
{
   ULONG NumberDevices;
   HANDLE hOut = INVALID_HANDLE_VALUE;
   HDEVINFO                 hardwareDeviceInfo;
   SP_INTERFACE_DEVICE_DATA deviceInfoData;
   ULONG                    i;
   BOOLEAN                  done;
   PUSB_DEVICE_DESCRIPTOR   usbDeviceInst;
   PUSB_DEVICE_DESCRIPTOR   *UsbDevices = &usbDeviceInst;

   *UsbDevices = NULL;
   NumberDevices = 0;
   // Open a handle to the plug and play dev node.
   // SetupDiGetClassDevs() returns a device information set that contains info on all
   // installed devices of a specified class.
   hardwareDeviceInfo = SetupDiGetClassDevs(pGuid,
                                            NULL, // Define no enumerator (global)
                                            NULL, // Define no
                                            (DIGCF_PRESENT | // Only Devices present
                                            DIGCF_INTERFACEDEVICE)); // Function class devices.
   if(hardwareDeviceInfo   ==   INVALID_HANDLE_VALUE)
   {
       //   Insert   error   handling   here.
       return INVALID_HANDLE_VALUE;
   }
   // Take a wild guess at the number of devices we have;
   // Be prepared to realloc and retry if there are more than we guessed
   NumberDevices = 4;
   done = FALSE;
   //Enumerate   through   all   devices   in   Set.
   deviceInfoData.cbSize = sizeof (SP_INTERFACE_DEVICE_DATA);
   i=0;
   while (!done)
   {
        NumberDevices *= 2;
        if (*UsbDevices)
        {
            *UsbDevices = (USB_DEVICE_DESCRIPTOR *)realloc (*UsbDevices, (NumberDevices * sizeof (USB_DEVICE_DESCRIPTOR)));
        }
        else
        {
            *UsbDevices = (USB_DEVICE_DESCRIPTOR *)calloc (NumberDevices, sizeof (USB_DEVICE_DESCRIPTOR));
        }
        if (NULL == *UsbDevices)
        {
            // SetupDiDestroyDeviceInfoList destroys a device information set
            // and frees all associated memory.
            SetupDiDestroyDeviceInfoList (hardwareDeviceInfo);
            return INVALID_HANDLE_VALUE;
        }
        usbDeviceInst = *UsbDevices + i;
        for (; i < NumberDevices; i++)
        {
             // SetupDiEnumDeviceInterfaces() returns information about device interfaces
             // exposed by one or more devices. Each call returns information about one interface;
             // the routine can be called repeatedly to get information about several interfaces
             // exposed by one or more devices.
            if (SetupDiEnumDeviceInterfaces (hardwareDeviceInfo,
                                             0, // We don't care about specific PDOs
                                             pGuid,
                                             i,
                                             &deviceInfoData))
            {
                hOut = OpenOneDevice (hardwareDeviceInfo, &deviceInfoData, outNameBuf);
                if ( hOut != INVALID_HANDLE_VALUE )
                {
                    done = TRUE;
                    break;
                }
            }
            else
            {
                if (ERROR_NO_MORE_ITEMS == GetLastError())
                {
                    done = TRUE;
                    break;
                }
            }
        }
   }
   NumberDevices = i;
   // SetupDiDestroyDeviceInfoList() destroys a device information set
   // and frees all associated memory.
   SetupDiDestroyDeviceInfoList (hardwareDeviceInfo);
   free ( *UsbDevices );
   return hOut;
}

/*****************************************
Routine Description:
    Given a ptr to a driver-registered GUID, give us a string with the device name
    that can be used in a CreateFile() call.
    Actually briefly opens and closes the device and sets outBuf if successfull;
    returns FALSE if not
Arguments:
    pGuid:      ptr to GUID registered by the driver itself
    outNameBuf: the generated zero-terminated name for this device
Return Value:
    TRUE on success else FALSE
*****************************************/
BOOL QextUsbBase::GetUsbDeviceFileName( LPGUID  pGuid, char *outNameBuf)
{
    HANDLE hDev = OpenUsbDevice( pGuid, outNameBuf );
    if ( hDev != INVALID_HANDLE_VALUE )
    {
        CloseHandle( hDev );
        return TRUE;
    }
    return FALSE;
}

/*****************************************
Routine Description:
    Called by main() to open an instance of our device after obtaining its name
Arguments:
    None
Return Value:
    Device handle on success else NULL
*****************************************/
HANDLE QextUsbBase::OpenDevFile( char *filename)
{
    HANDLE h;
    if ( !GetUsbDeviceFileName( (LPGUID)(&m_Guid), m_CompleteDeviceName) )
    {
        return  INVALID_HANDLE_VALUE;
    }
    strcat (m_CompleteDeviceName,"\\");
    strcat (m_CompleteDeviceName,filename);
    h = CreateFileA(m_CompleteDeviceName,
                    GENERIC_WRITE | GENERIC_READ,
                    FILE_SHARE_WRITE | FILE_SHARE_READ,
                    NULL,
                    OPEN_EXISTING,
                    0,
                    NULL);
    return h;  //INVALID_HANDLE_VALUE
}


/*****************************************
Routine Description:
    Called to get ascii string of USB descriptor
Arguments:
    PUSB_ENDPOINT_DESCRIPTOR->bDescriptorType or
    PUSB_DEVICE_DESCRIPTOR->bDescriptorType or
    PUSB_INTERFACE_DESCRIPTOR->bDescriptorType or
    PUSB_STRING_DESCRIPTOR->bDescriptorType or
    PUSB_POWER_DESCRIPTOR->bDescriptorType or
    PUSB_CONFIGURATION_DESCRIPTOR->bDescriptorType
Return Value:
    ptr to string
*****************************************/
char *QextUsbBase::usbDescriptorTypeString(UCHAR bDescriptorType )
{
    switch(bDescriptorType)
    {
        case USB_DEVICE_DESCRIPTOR_TYPE:
            return "USB_DEVICE_DESCRIPTOR_TYPE";
        case USB_CONFIGURATION_DESCRIPTOR_TYPE:
            return "USB_CONFIGURATION_DESCRIPTOR_TYPE";
        case USB_STRING_DESCRIPTOR_TYPE:
            return "USB_STRING_DESCRIPTOR_TYPE";
        case USB_INTERFACE_DESCRIPTOR_TYPE:
            return "USB_INTERFACE_DESCRIPTOR_TYPE";
        case USB_ENDPOINT_DESCRIPTOR_TYPE:
            return "USB_ENDPOINT_DESCRIPTOR_TYPE";
    #ifdef USB_POWER_DESCRIPTOR_TYPE // this is the older definintion which is actually obsolete
        // workaround for temporary bug in 98ddk, older USB100.h file
        case USB_POWER_DESCRIPTOR_TYPE:
            return "USB_POWER_DESCRIPTOR_TYPE";
    #endif
    #ifdef USB_RESERVED_DESCRIPTOR_TYPE  // this is the current version of USB100.h as in NT5DDK
        case USB_RESERVED_DESCRIPTOR_TYPE:
            return "USB_RESERVED_DESCRIPTOR_TYPE";
        case USB_CONFIG_POWER_DESCRIPTOR_TYPE:
            return "USB_CONFIG_POWER_DESCRIPTOR_TYPE";
        case USB_INTERFACE_POWER_DESCRIPTOR_TYPE:
            return "USB_INTERFACE_POWER_DESCRIPTOR_TYPE";
    #endif // for current nt5ddk version of USB100.h
        default:
            return "??? UNKNOWN!!";
    }
}

/*****************************************
Routine Description:
    Called to get ascii string of endpt descriptor type
Arguments:
    PUSB_ENDPOINT_DESCRIPTOR->bmAttributes
Return Value:
    ptr to string
*****************************************/
char *QextUsbBase::usbEndPointTypeString(UCHAR bmAttributes)
{
    UINT typ = bmAttributes & USB_ENDPOINT_TYPE_MASK;
    switch( typ)
    {
        case USB_ENDPOINT_TYPE_INTERRUPT:
            return "USB_ENDPOINT_TYPE_INTERRUPT";
        case USB_ENDPOINT_TYPE_BULK:
            return "USB_ENDPOINT_TYPE_BULK";
        case USB_ENDPOINT_TYPE_ISOCHRONOUS:
            return "USB_ENDPOINT_TYPE_ISOCHRONOUS";
        case USB_ENDPOINT_TYPE_CONTROL:
            return "USB_ENDPOINT_TYPE_CONTROL";
        default:
            return "??? UNKNOWN!!";
    }
}

/*****************************************
Routine Description:
    Called to get ascii string of USB_CONFIGURATION_DESCRIPTOR attributes
Arguments:
    PUSB_CONFIGURATION_DESCRIPTOR->bmAttributes
Return Value:
    ptr to string
*****************************************/
char *QextUsbBase::usbConfigAttributesString(UCHAR bmAttributes)
{
    UINT typ = bmAttributes & USB_CONFIG_POWERED_MASK;
    switch( typ)
    {
        case USB_CONFIG_BUS_POWERED:
            return "USB_CONFIG_BUS_POWERED";
        case USB_CONFIG_SELF_POWERED:
            return "USB_CONFIG_SELF_POWERED";
        case USB_CONFIG_REMOTE_WAKEUP:
            return "USB_CONFIG_REMOTE_WAKEUP";
        default:
            return "??? UNKNOWN!!";
    }
}


/*****************************************
Routine Description:
    Called to do formatted ascii dump to console of a USB config descriptor
Arguments:
    ptr to USB configuration descriptor
Return Value:
    none
*****************************************/
void QextUsbBase::print_USB_CONFIGURATION_DESCRIPTOR(PUSB_CONFIGURATION_DESCRIPTOR cd)
{
    IDE_DEBUG(QString("===== USB DEVICE STATUS =====\nUSB_CONFIGURATION_DESCRIPTOR"));
    IDE_DEBUG(QString("bLength = 0x%x, decimal %d").arg(cd->bLength, cd->bLength));
    IDE_DEBUG(QString("bDescriptorType = 0x%x ( %s )").arg(cd->bDescriptorType).arg(usbDescriptorTypeString( cd->bDescriptorType )));
    IDE_DEBUG(QString("wTotalLength = 0x%x, decimal %d").arg(cd->wTotalLength, cd->wTotalLength));
    IDE_DEBUG(QString("bNumInterfaces = 0x%x, decimal %d").arg(cd->bNumInterfaces, cd->bNumInterfaces));
    IDE_DEBUG(QString("bConfigurationValue = 0x%x, decimal %d").arg(cd->bConfigurationValue, cd->bConfigurationValue));
    IDE_DEBUG(QString("iConfiguration = 0x%x, decimal %d").arg(cd->iConfiguration, cd->iConfiguration));
    IDE_DEBUG(QString("bmAttributes = 0x%x ( %s )").arg(cd->bmAttributes).arg(usbConfigAttributesString( cd->bmAttributes )));
    IDE_DEBUG(QString("MaxPower = 0x%x, decimal %d").arg(cd->MaxPower, cd->MaxPower));
    IDE_DEBUG(QString("-----------------------------\n"));
}
/*****************************************
Routine Description:
    Called to do formatted ascii dump to console of a USB interface descriptor
Arguments:
    ptr to USB interface descriptor
Return Value:
    none
*****************************************/
void QextUsbBase::print_USB_INTERFACE_DESCRIPTOR(PUSB_INTERFACE_DESCRIPTOR id, UINT ix)
{
    IDE_DEBUG(QString("USB_INTERFACE_DESCRIPTOR #%d").arg(ix));
    IDE_DEBUG(QString("bLength = 0x%x").arg(id->bLength));
    IDE_DEBUG(QString("bDescriptorType = 0x%x ( %s )").arg(id->bDescriptorType).arg(usbDescriptorTypeString( id->bDescriptorType )));
    IDE_DEBUG(QString("bInterfaceNumber = 0x%x").arg(id->bInterfaceNumber));
    IDE_DEBUG(QString("bAlternateSetting = 0x%x").arg(id->bAlternateSetting));
    IDE_DEBUG(QString("bNumEndpoints = 0x%x").arg(id->bNumEndpoints));
    IDE_DEBUG(QString("bInterfaceClass = 0x%x").arg(id->bInterfaceClass));
    IDE_DEBUG(QString("bInterfaceSubClass = 0x%x").arg(id->bInterfaceSubClass));
    IDE_DEBUG(QString("bInterfaceProtocol = 0x%x").arg(id->bInterfaceProtocol));
    IDE_DEBUG(QString("bInterface = 0x%x").arg(id->iInterface));
    IDE_DEBUG(QString("-----------------------------\n"));
}
/*****************************************
Routine Description:
    Called to do formatted ascii dump to console of a USB endpoint descriptor
Arguments:
    ptr to USB endpoint descriptor,
    index of this endpt in interface desc
Return Value:
    none
*****************************************/
void QextUsbBase::print_USB_ENDPOINT_DESCRIPTOR(PUSB_ENDPOINT_DESCRIPTOR ed, int i)
{
    IDE_DEBUG(QString("USB_ENDPOINT_DESCRIPTOR for Pipe%02d").arg(i));
    IDE_DEBUG(QString("bLength = 0x%x").arg(ed->bLength));
    IDE_DEBUG(QString("bDescriptorType = 0x%x ( %s )").arg(ed->bDescriptorType).arg(usbDescriptorTypeString( ed->bDescriptorType )));
    if (USB_ENDPOINT_DIRECTION_IN( ed->bEndpointAddress ))
        IDE_DEBUG(QString("bEndpointAddress= 0x%x ( INPUT )").arg(ed->bEndpointAddress));
    else
        IDE_DEBUG(QString("bEndpointAddress= 0x%x ( OUTPUT )").arg(ed->bEndpointAddress));
    IDE_DEBUG(QString("bmAttributes= 0x%x ( %s )").arg(ed->bmAttributes).arg(usbEndPointTypeString ( ed->bmAttributes )));
    IDE_DEBUG(QString("wMaxPacketSize= 0x%x, decimal %d").arg(ed->wMaxPacketSize, ed->wMaxPacketSize));
    IDE_DEBUG(QString("bInterval = 0x%x, decimal %d").arg(ed->bInterval, ed->bInterval));
    IDE_DEBUG(QString("-----------------------------\n"));
}


