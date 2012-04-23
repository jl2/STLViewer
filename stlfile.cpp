/*
  stlfile.cpp

  Copyright (c) 2012, Jeremiah LaRocco jeremiah.larocco@gmail.com

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include "stlfile.h"

#include <string>

#include <cstdio>
#include <cstring>
#include <stdexcept>

STLFile::STLFile(std::string fname) {
    // Because FILE* is easier to use than the C++ stuff
    FILE *inf = std::fopen(fname.c_str(), "rb");
    if (inf == NULL) {
        throw std::runtime_error(std::string("Cannot open file ") + fname);
    }
    char buffer[80];
    size_t bytes_read = fread(buffer, 1, 6, inf);
    if (bytes_read != 6) {
        throw std::runtime_error("Invalid STL file - could not read first 6 bytes.");
    }
    buffer[bytes_read] = '\0';
    if (0 == std::memcmp(buffer, "solid ", 6)) {
        read_ascii_file(inf);
    } else {
        read_binary_file(inf);
    }
    std::fclose(inf);
}

STLFile::STLFile() {
}

STLFile::~STLFile() {
}

void STLFile::read_ascii_file(FILE *inf) {
    std::rewind(inf);
    throw std::runtime_error("ASCII files not yet supported.");
}

void STLFile::read_binary_file(FILE *inf) {
    std::rewind(inf);
}
