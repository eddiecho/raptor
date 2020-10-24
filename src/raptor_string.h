// TODO - string interning
struct String {
  char *value;
  u32 length;

  String() {
    value = 0;
    length = 0;
  };

  String(const char *val) {
    length = strlen(val);
    value = new char[length + 1];
    strcpy(value, val);
  };

  String(const char *val, const u32 strlength) {
    length = strlength;
    value = new char[strlength + 1];
    // TODO - this is prime for buffer overrun!
    strcpy(value, val);
  };

  // copy constructor
  String(const String &src) {
    value = new char[src.length + 1];
    length = src.length;
    strcpy(value, src.value);
  };

  // move constructor
  String(String &&src) {
    value = src.value;
    length = src.length;

    src.value = 0;
  };

  ~String() { delete value; };
};
