#pragma once
#include <iostream>

class String
{
private:
	char* str;
	size_t size;

	String(char* s, size_t sz) : str(s), size(sz) {} // util para operator+()
public:
	String(); // Empty constructor
	String(const char* s); // Constructor
	~String();
	String(const String& other); // Copy constructor
	String(String&& other) noexcept; // Move constructor

	String& operator=(const String& other); // Copy assignment
	String& operator=(String&& other) noexcept; // Move assignment
	String operator+(const String& rhs) const;
	bool operator==(const String& rhs) const;

	size_t length() const; // string length
	void clear(); // set to empty string

	/*
	**Extra bonus: add needed code to string so that this call is efficient. Hint: remember &&**

	String GetMeAString() { return String("another string"); }
	void main() { String a = GetMeAString(); }
	Works, because it calls efficiently the Move constructor.
	*/
};