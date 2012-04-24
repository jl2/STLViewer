/*
  stlfile.h

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

#ifndef STL_FILE_HEADER
#define STL_FILE_HEADER

#include <vector>
#include <string>
#include <cstring>
struct Triangle {
    float normal[3];
    float verts[3*3];
    
    Triangle(float tr[12]) {
        std::memcpy(normal, tr, sizeof(float)*3);
        std::memcpy(verts, tr + 3, sizeof(float)*3*3);
    }
};

typedef std::vector<Triangle> tri_vect_t;

class STLFile {
public:
    STLFile();
    STLFile(std::string fname);
    ~STLFile();
    // void draw();
    void fillBuffers(size_t max_tris, float *verts, float *norms, unsigned int *indices);
    size_t getNumTris();
    float getBoundingRadius();

private:
    void read_ascii_file(FILE *inf);
    void read_binary_file(FILE *inf);
    
private:
    tri_vect_t tris;
    char header[80];
    float most_extreme_point[3];
};

#endif
