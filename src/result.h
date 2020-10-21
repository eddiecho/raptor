template <typename T> struct Ok {
  T val;

  Ok(const T &val) : val(val) {}
};

template <> struct Ok<void> {};

template <typename E> struct Err {
  Err(const E &error) : error(error) {}
  E error;
};

template <typename T, typename E, typename F, typename M> struct Result {
  static_assert(!std::is_same<E, void>::value,
                "Result error type may not be void");

  bool32 ok;
  union {
    T val;
    E err;
  };

  Result(Ok<T> ok) {
    val = ok.val;
    ok = 1;
  };

  Result(Ok<void> ok) { ok = 1; };

  Result(Err<E> error) {
    err = error.error;
    ok = 0;
  };

  Result() { ok = 0; }

  T unwrap() {
    if (this->ok)
      return val;
    panic(err);
  };

  M map(F f) { return f(this->unwrap()); }
};

