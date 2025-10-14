#include<iostream>
#include"String.h"

String::String() 
	: str(new char[1]{'\0'}), size(0) {}

String::String(const char* s) {
	int len = 0;
	while (s[len] != '\0') ++len;
	size = len;
	str = new char[len + 1];
	for (int i = 0; i <= len; ++i) str[i] = s[i];
}

String::~String() {
	delete[] str;
	str = nullptr;
}

String::String(const String& other)
	: str(new char[other.size + 1]), size(other.size)
{
	for (int i = 0; i <= size; ++i) str[i] = other.str[i];
}

String::String(String&& other) noexcept
	: str(other.str), size(other.size)
{
	other.size = 0;
	other.str = nullptr;
}

String& String::operator=(const String& other)
{
	if (this == &other) return *this;
	delete[] str;
	str = new char[other.size + 1]; size = other.size;
	for (int i = 0; i <= size; ++i) str[i] = other.str[i];
	return *this;
}

String& String::operator=(String&& other) noexcept
{
	delete[] str;
	str = other.str; size = other.size;
	other.size = 0;
	other.str = nullptr;
	return *this;
}

String String::operator+(const String& rhs) const
{
	size_t len = size + rhs.size;
	char* new_str = new char[len + 1];
	for (int i = 0; i < size; ++i) new_str[i] = str[i]; // i < size pq no quiero q copie '\0'
	for (int j = 0; j <= rhs.size; ++j) new_str[size + j] = rhs.str[j];

	/*String ret(new_str);
	delete[] new_str;
	return ret; se puede evitar este delete con el constructor privado*/
	return String(new_str, len);
}

bool String::operator==(const String& rhs) const
{
	if (size != rhs.size) return false;
	for (int i = 0; i < size; ++i) if (str[i] != rhs.str[i]) return false;
	return true;
}

size_t String::length() const
{
	return size;
}

void String::clear()
{
	*this = String();
}