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
#include <cmath>
#include <cstdio>
#include <cstring>

#include <cstdlib>

#include <stdexcept>

#include <QDebug>

void computeMostDistant(float *cur_vert, float *new_verts);
    
STLFile::STLFile(std::string fname) {
    // Because FILE* is easier to use than the C++ stuff
    FILE *inf = std::fopen(fname.c_str(), "rb");
    if (inf == NULL) {
        throw std::runtime_error(std::string("Cannot open file ") + fname);
    }
    
    char buffer[6];
    size_t bytes_read = fread(buffer, 1, 6, inf);
    if (bytes_read != 6) {
        throw std::runtime_error("Invalid STL file - could not read first 6 bytes.");
    }
    std::rewind(inf);
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

void read_vert_from_line(const char *buffer, const char *prefix, float *verts) {
    size_t preLen = strlen(prefix);
    size_t off = 0;
    while (buffer[off] == ' ' || buffer[off]=='\t') ++off;
    
    if (0 != std::memcmp(off+buffer, prefix, preLen-1)) {
        throw std::runtime_error(buffer);
    }
    const char *tmp = (char*)buffer+preLen+off+1;
    char *end;
    verts[0] = strtof(tmp, &end);
    verts[1] = strtof(end+1, &end);
    verts[2] = strtof(end+1, &end);
}

void STLFile::read_ascii_file(FILE *inf) {
    char buffer[256] = {'\0'};
    char *rval = 0;

    rval = fgets(buffer, 255, inf);
    if (rval == NULL) {
        throw std::runtime_error("Could not read first line of ASCII file.");
    }

    most_extreme_point[0] = most_extreme_point[1] = most_extreme_point[2] = 0.0f;
    rval = fgets(buffer, 255, inf);

    while (0 != std::memcmp(buffer, "endsolid", 8)) {

        float next_tri[12];
        read_vert_from_line(buffer, "facet normal", next_tri);

        // Read and ignore "outer loop"
        rval = fgets(buffer, 255, inf);

        // Read vertices
        rval = fgets(buffer, 255, inf);
        read_vert_from_line(buffer, "vertex ", next_tri+3);
        
        rval = fgets(buffer, 255, inf);
        read_vert_from_line(buffer, "vertex ", next_tri+6);
        
        rval = fgets(buffer, 255, inf);
        read_vert_from_line(buffer, "vertex ", next_tri+9);

        computeMostDistant(most_extreme_point, next_tri + 3);
        tris.push_back(Triangle(next_tri));

        // Read and ingore "endloop"
        rval = fgets(buffer, 255, inf);

        // Read and ignore "endfacet"
        rval = fgets(buffer, 255, inf);

        // Read first line of next triangle or "endsolid"
        rval = fgets(buffer, 255, inf);
    }
}

void computeMostDistant(float *cur_vert, float *new_verts) {
    double old_dist = cur_vert[0]*cur_vert[0] + cur_vert[1]*cur_vert[1] + cur_vert[2]*cur_vert[2];
    for (size_t i=0;i<3; ++i) {
        double new_dist = new_verts[3*i+0]*new_verts[3*i+0] + new_verts[3*i+1]*new_verts[3*i+1] + new_verts[3*i+2]*new_verts[3*i+2];
        if (new_dist > old_dist) {
            cur_vert[0] = new_verts[3*i+0];
            cur_vert[1] = new_verts[3*i+1];
            cur_vert[2] = new_verts[3*i+2];
            old_dist = new_dist;
        }
    }
}
void STLFile::read_binary_file(FILE *inf) {
    char buffer[80];
    size_t num_read = fread(buffer, 1, 80, inf);
    if (num_read != 80) {
        throw std::runtime_error("Invalid binary STL file - could not read 80 byte header.");
    }
    std::memcpy(header, buffer, 80);

    unsigned int num_tris = 0;
    num_read = std::fread(&num_tris, sizeof(unsigned int), 1, inf);
    if (num_read != 1) {
        throw std::runtime_error("Invalid binary STL file - could not read number of triangles from binary STL file.");
    }
    most_extreme_point[0] = most_extreme_point[1] = most_extreme_point[2] = 0.0f;
    
    float next_tri[12];
    for (size_t i=0; i< num_tris; ++i) {
        num_read = fread(next_tri, sizeof(float), 12, inf);
        if (num_read != 12) {
            throw std::runtime_error("Could not read a full triangle.");
        }
        computeMostDistant(most_extreme_point, next_tri + 3);
        tris.push_back(Triangle(next_tri));

        unsigned short whatever = 0;
        num_read = fread(&whatever, 2, 1, inf);
        if (num_read != 1) {
            throw std::runtime_error("Could not read a full triangle.");
        }
    }
    qDebug() << "Most extreme point: " << most_extreme_point[0] << most_extreme_point[1] << most_extreme_point[2];
}

size_t STLFile::getNumTris() {
    return tris.size();
}

float STLFile::getBoundingRadius() {
    return 1.1*std::sqrt(most_extreme_point[0]*most_extreme_point[0] +
                         most_extreme_point[1]*most_extreme_point[1] +
                         most_extreme_point[2]*most_extreme_point[2]);
}
void STLFile::fillBuffers(size_t max_tris, float *verts, float *norms, unsigned int *indices) {

    size_t nt = max_tris;
    if (tris.size()<max_tris) {
        nt = tris.size();
    }
    
    for (size_t i=0; i<nt; ++i) {
        memcpy(norms + 3*(3*i+0), tris[i].normal, sizeof(float)*3);
        memcpy(norms + 3*(3*i+1), tris[i].normal, sizeof(float)*3);
        memcpy(norms + 3*(3*i+2), tris[i].normal, sizeof(float)*3);

        memcpy(verts+i*3*3, tris[i].verts, sizeof(float)*3*3);
        indices[3*i + 0] = 3*i+0;
        indices[3*i + 1] = 3*i+1;
        indices[3*i + 2] = 3*i+2;
    }
}
