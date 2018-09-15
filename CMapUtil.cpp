#include <stdexcept>

#include "CMapUtil.h"

using namespace std;

CMapUtil *CMapUtil::CreateMap()
{
    MAP_HANDLE work;

    if (NULL == (work = Map_Create(NULL)))
        throw runtime_error("Failed to create map");
    
    return new CMapUtil(work, true);
}

CMapUtil::CMapUtil(MAP_HANDLE handle, bool isOwned)
{
    _isOwned = isOwned;
    _handle = handle;
}

CMapUtil::CMapUtil(const CMapUtil &other)
{
    _isOwned = true;
    _handle = Map_Clone(other.GetHandle());

    if (_handle == NULL)
        throw runtime_error("Failed to clone map");
}

CMapUtil::~CMapUtil()
{
    if (_isOwned) 
        Map_Destroy(GetHandle());
}

MAP_RESULT CMapUtil::Add(const char *key, const char *value)
{
    return Map_Add(GetHandle(), key, value);
}

MAP_RESULT CMapUtil::AddOrUpdate(const char *key, const char *value)
{
    return Map_AddOrUpdate(GetHandle(), key, value);
}

bool CMapUtil::ContainsKey(const char *key) const
{
    bool found;
    MAP_RESULT result = Map_ContainsKey(GetHandle(), key, &found);

    if (result != MAP_OK)
        throw runtime_error("Failed to check for key");
    else
        return found;
}

bool CMapUtil::ContainsValue(const char *value) const
{
    bool found;
    MAP_RESULT result = Map_ContainsValue(GetHandle(), value, &found);

    if (result != MAP_OK)
        throw runtime_error("Failed to check for value");
    else
        return found;
}

const char *CMapUtil::GetValue(const char *key) const
{
    return Map_GetValueFromKey(GetHandle(), key);
}

