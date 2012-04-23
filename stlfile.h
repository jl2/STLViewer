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

struct Vertex {
    float x,y,z;
};

struct Triangle {
    Vertex normal;
    Vertex verts[3];
    Triangle(float tr[12]) {
        normal.x = tr[0];
        normal.y = tr[1];
        normal.z = tr[2];

        verts[0].x = tr[3];
        verts[0].y = tr[4];
        verts[0].z = tr[5];

        verts[1].x = tr[6];
        verts[1].y = tr[7];
        verts[1].z = tr[8];

        verts[2].x = tr[9];
        verts[2].y = tr[10];
        verts[2].z = tr[11];
    }
    Triangle(float nx, float ny, float nz,
             float v1x, float v1y, float v1z,
             float v2x, float v2y, float v2z,
             float v3x, float v3y, float v3z) {

        normal.x = nx;
        normal.y = ny;
        normal.z = nz;

        verts[0].x = v1x;
        verts[0].y = v1y;
        verts[0].z = v1z;

        verts[1].x = v2x;
        verts[1].y = v2y;
        verts[1].z = v2z;

        verts[2].x = v3x;
        verts[2].y = v3y;
        verts[2].z = v3z;
    }
};

typedef std::vector<Triangle> tri_vect_t;

class STLFile {
public:
    STLFile();
    STLFile(std::string fname);
    ~STLFile();
    void draw();

private:
    void read_ascii_file(FILE *inf);
    void read_binary_file(FILE *inf);

private:
    tri_vect_t tris;
    char header[80];
};

#endif
