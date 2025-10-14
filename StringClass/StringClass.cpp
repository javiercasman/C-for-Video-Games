#include <iostream>
#include "String.h"

String GetMeAString() {
    return String("another string");
}

int main() {
    using std::cout; using std::endl;

    cout << "--- Test String Class ---" << endl;

    // 1️⃣ Constructor por defecto
    String s1;
    cout << "s1 (default): \"" << s1.c_str() << "\", length: " << s1.length() << endl;

    // 2️⃣ Constructor con const char*
    String s2("Hello");
    cout << "s2: \"" << s2.c_str() << "\", length: " << s2.length() << endl;

    // 3️⃣ Copy constructor
    String s3(s2);
    cout << "s3 (copy of s2): \"" << s3.c_str() << "\", length: " << s3.length() << endl;

    // 4️⃣ Move constructor
    String s4(String("Temporary"));
    cout << "s4 (move from temporary): \"" << s4.c_str() << "\", length: " << s4.length() << endl;

    // 5️⃣ Copy assignment
    s1 = s2;
    cout << "s1 (after copy assignment from s2): \"" << s1.c_str() << "\", length: " << s1.length() << endl;

    // 6️⃣ Move assignment
    s1 = String("Moved String");
    cout << "s1 (after move assignment): \"" << s1.c_str() << "\", length: " << s1.length() << endl;

    // 7️⃣ Concatenation operator+
    String s5 = s2 + String(" World");
    cout << "s5 (s2 + \" World\"): \"" << s5.c_str() << "\", length: " << s5.length() << endl;

    // 8️⃣ Comparison operator==
    cout << "s5 == \"Hello World\"? " << (s5 == String("Hello World") ? "true" : "false") << endl;
    cout << "s5 == s2? " << (s5 == s2 ? "true" : "false") << endl;

    // 9️⃣ Clear
    s5.clear();
    cout << "s5 after clear: \"" << s5.c_str() << "\", length: " << s5.length() << endl;

    // 🔹 Test GetMeAString() move semantics
    auto GetMeAString = []() -> String {
        std::cout << "GetMeAString() called\n";
        return String("another string");
        };

    String s6 = GetMeAString();
    cout << "s6 from GetMeAString(): \"" << s6.c_str() << "\", length: " << s6.length() << endl;

    cout << "--- End of Test ---" << endl;
    return 0;
}