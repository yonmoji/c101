#pragma once
#include <Arduino.h>
#include <memory>
#include <vector>

// Print to multiple `Print`
class MultiPrint : public Print {
public:
  std::vector<Print*> prints;
  MultiPrint(std::vector<Print*> ps) : prints(ps) {}

  size_t write(const uint8_t u) {
    for (auto p: prints) {
      p->write(u);
    }
    return 1;
  }

  size_t write(const uint8_t *buffer, const size_t size) {
    for (auto p: prints) {
      p->write(buffer, size);
    }
    return size;
  }
};

// Buffering Print bytes. For logger API
class BufferedPrint : public Print {
public:
  size_t i = 0;
  const size_t cap;
  std::unique_ptr<uint8_t[]> data;

  BufferedPrint(std::unique_ptr<uint8_t[]> d, const size_t cap) : cap(cap) {
    data = std::move(d);
  }

  size_t write(const uint8_t u) {
    data[i] = u;
    i += 1;
    if (i == cap) {
      i = 0;
    }
    return 1;
  }

  size_t write(const uint8_t *buffer, const size_t size) {
    auto i_from = 0;
    auto left = size;
    while (0 < left) {
      auto append_size = left;
      if (cap < i + append_size) {
        append_size = cap - i;
      } // cap=10, i=8, size=3 => append_size=2
      if (0 < append_size) {
        memcpy(&data[i], &buffer[i_from], append_size);
      }
      i_from += append_size;
      left -= append_size;
      i += append_size;

      if (i == cap) {
        i = 0;
      }
    }
    return size;
  }

  size_t dump(uint8_t *dst, const size_t size) {
    auto sz1 = min(size, cap-i);
    sz1 = max((size_t)0, sz1);

    // data[i,i+sz1) を dst[0,sz1) に sz1 byte コピー
    if (sz1 < 0 || data[i] == 0x0) {
      sz1 = 0;
    }
    if (0 < sz1) {
      memcpy(&dst[0], &data[i], sz1);
    }

    auto sz2 = min(i, size);
    sz2 = max((size_t)0, sz2);
    // data[sz1,sz2) を dst[sz1,sz1+sz2) に sz2 byte コピー
    if (0 < sz2) {
      memcpy(&dst[sz1], &data[0], sz2);
    }

    return sz1+sz2;
  }
};
