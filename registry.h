#pragma once

#include <type_traits>

// C++ class for read/store application settings into Windows Registry (can be
// accessed via regedit application). Supports storing values in integral and
// string format.
//
// NOTE: If you want to store decimal data (float or double), convert it to std::string first!
class Registry
{
private:
	HKEY hKey;
	long status;
	bool isRead;

public:
	// Class for processing load/store data into Windows Registry
	class Proxy
	{
	public:
		Proxy(Registry& reg, const char* key) 
			: reg(reg), key(key)
		{}
		
		// Read an integral value from registry with the specified key, thanks to SFINAE
		template <typename T, typename = typename std::enable_if_t<std::is_integral_v<T>>>
		operator T() const
		{
			if (!reg.isRead) {
				return 0;
			}
			T ret;
			DWORD type = REG_DWORD;
			if (std::is_same_v<T, uint64_t> || std::is_same_v<T, int64_t>) {
				type = REG_QWORD;
			}
			DWORD size = sizeof(T);
			LONG result = RegQueryValueEx(reg.hKey, key, nullptr, &type, reinterpret_cast<LPBYTE>(&ret), &size);
			if (result != ERROR_SUCCESS) {
				return 0;
			}
			return ret;
		}

		// Read a char array from registry with the specified key, and convert it to std::string
		operator std::string() const
		{
			if (!reg.isRead) {
				return "";
			}
			char ret[260];
			DWORD size = sizeof(ret);
			DWORD type = REG_SZ;
			LONG res = RegQueryValueEx(reg.hKey, key, nullptr, &type, reinterpret_cast<LPBYTE>(ret), &size);
			if (res != ERROR_SUCCESS) {
				return "";
			}
			return std::string(ret);
		}

		// Store an integral value to registry with the specified key. Will be stored as REG_QWORD if T
		// is a 64-bit integer, REG_DWORD otherwise.
		template <typename T, typename = typename std::enable_if_t<std::is_integral_v<T>>>
		void operator=(T& value)
		{
			if (reg.isRead) {
				return;
			}
			DWORD type = REG_DWORD;
			if (std::is_same_v<T, uint64_t> || std::is_same_v<T, int64_t>) {
				type = REG_QWORD;
			}
			RegSetValueEx(reg.hKey, key, 0, type, reinterpret_cast<LPBYTE>(&value), sizeof(T));
		}

		// Store a std::string value to registry with the specified key. Will be stored as REG_SZ
		void operator=(std::string& value)
		{
			if (reg.isRead) {
				return;
			}
			RegSetValueEx(reg.hKey, key, 0, REG_SZ, reinterpret_cast<LPBYTE>(const_cast<char*>(value.data())), value.size());
		}

	private:
		Registry& reg;
		const char* key;
	};

	Registry(HKEY key, LPCSTR subKey, bool read)
	{
		isRead = read;

		// On write mode, create a new subkey if its doesn't exist in Windows Registry
		if (isRead) {
			status = RegOpenKeyEx(key, subKey, 0, KEY_READ, &hKey);
		} else {
			status = RegOpenKeyEx(key, subKey, 0, KEY_WRITE, &hKey);
			if (status != ERROR_SUCCESS) {
				status = RegCreateKeyEx(key, subKey, 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr);
			}
		}
	}

	~Registry()
	{
		if (hKey != nullptr) {
			RegCloseKey(hKey);
		}
	}

	explicit operator bool() const { return status == ERROR_SUCCESS; }

	// Overloaded operator for load/store values with the designed key, like in std::map
	Proxy operator[](const char* key) {	return Proxy(*this, key); }	
};
