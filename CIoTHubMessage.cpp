#include <stdexcept>

#include "CIoTHubMessage.h"

using namespace std;

CIoTHubMessage::CIoTHubMessage()
{
    throw runtime_error("Default constructor for CIoTHubMessage should not be called");
}

CIoTHubMessage::CIoTHubMessage(const std::string &message) : CIoTHubMessage(message.c_str())
{

}

CIoTHubMessage::CIoTHubMessage(const char *message)
{
    _isOwned = true;
    _handle = IoTHubMessage_CreateFromString(message);

    if (_handle == NULL)
        throw runtime_error("Failed to create CIoTHubMessage instance");
}

CIoTHubMessage::CIoTHubMessage(const uint8_t *message, size_t length)
{
    _isOwned = true;
    _handle = IoTHubMessage_CreateFromByteArray(message, length);

    if (_handle == NULL)
        throw runtime_error("Failed to create CIoTHubMessage instance");
}

CIoTHubMessage::CIoTHubMessage(IOTHUB_MESSAGE_HANDLE handle)
{
    _isOwned = false;
    _handle = handle;
}

CIoTHubMessage::CIoTHubMessage(const CIoTHubMessage &other)
{
    _handle = IoTHubMessage_Clone(other.GetHandle());

    if (_handle == NULL)
        throw runtime_error("Failed to create CIoTHubMessage instance");
}

CIoTHubMessage::~CIoTHubMessage()
{
    if (_isOwned)
        IoTHubMessage_Destroy(GetHandle());
}

const char *CIoTHubMessage::GetCString() const
{
    if (GetContentType() == IOTHUBMESSAGE_STRING)
    {
        return IoTHubMessage_GetString(GetHandle());
    }
    else 
    {
        return "";
    }
}

const string CIoTHubMessage::GetString() const
{
    return string(IoTHubMessage_GetString(GetHandle()));
}

IOTHUB_MESSAGE_RESULT CIoTHubMessage::GetByteArray(const uint8_t **buffer, size_t *size) const
{
    return IoTHubMessage_GetByteArray(GetHandle(), buffer, size);
}

IOTHUBMESSAGE_CONTENT_TYPE CIoTHubMessage::GetContentType() const
{
    return IoTHubMessage_GetContentType(GetHandle());
}

IOTHUB_MESSAGE_RESULT CIoTHubMessage::SetContentTypeSystemProperty(const char *contentType)
{
    return IoTHubMessage_SetContentTypeSystemProperty(GetHandle(), contentType);
}

const char *CIoTHubMessage::GetContentTypeSystemProperty() const
{
    return IoTHubMessage_GetContentTypeSystemProperty(GetHandle());
}

CMapUtil *CIoTHubMessage::GetProperties()
{
    return new CMapUtil(IoTHubMessage_Properties(GetHandle()));
}

IOTHUB_MESSAGE_RESULT CIoTHubMessage::SetProperty(const char *key, const char *value)
{
    return IoTHubMessage_SetProperty(GetHandle(), key, value);
}

const char *CIoTHubMessage::GetProperty(const char *key) const
{
    return IoTHubMessage_GetProperty(GetHandle(), key);
}

IOTHUB_MESSAGE_RESULT CIoTHubMessage::SetMessageId(const char *messageId)
{
    return IoTHubMessage_SetMessageId(GetHandle(), messageId);
}

const char *CIoTHubMessage::GetMessageId() const
{
    return IoTHubMessage_GetMessageId(GetHandle());
}

IOTHUB_MESSAGE_RESULT CIoTHubMessage::SetCorrelationId(const char *correlationId)
{
    return IoTHubMessage_SetCorrelationId(GetHandle(), correlationId);
}

const char *CIoTHubMessage::GetCorrelationId() const
{
    return IoTHubMessage_GetCorrelationId(GetHandle());
}

