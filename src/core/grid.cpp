//
// Created by Ryan Matimu on 20/08/2025.
//

# include <iostream>
# include "bx/math.h"
# include <unistd.h> // for getcwd
# include "core/primitives.h"

const int GRID_SIZE = 40;
const int GRID_SPACING = 1.0f;
const uint32_t GRID_COLOR = 0xff0000ff;

void createGrid(std::vector<PosColorVertex> &gridVertices, std::vector<uint16_t> &gridIndices) {
    for (int i = 0; i <= GRID_SIZE; i++) {
        for (int j = 0; j <= GRID_SIZE; j++) {
            float x =  (float) (i - GRID_SIZE / 2)* GRID_SPACING;
            float z = (float) (j - GRID_SIZE / 2) * GRID_SPACING;
            gridVertices.push_back({x, 0.0f, z, GRID_COLOR});
        }
    }
    for (int i = 0; i <= GRID_SIZE ; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            gridIndices.push_back(i * (GRID_SIZE + 1) + j);
            gridIndices.push_back(i * (GRID_SIZE + 1) + j + 1);
        }
    }
    for (int j = 0; j <= GRID_SIZE; j++) {
        for (int i = 0; i < GRID_SIZE; i++) {
            gridIndices.push_back(i * (GRID_SIZE + 1) + j);
            gridIndices.push_back( (i + 1) * (GRID_SIZE + 1) + j);
        }
    }
}