//
// Created by Ryan Matimu on 20/08/2025.
//
# pragma once
# include "core/primitives.h"

/**
 * Creates an n by n grid of on a flat plane in space.
 * @param gridVertices the vector of vertices
 * @param gridIndices  the vector of the indices of the vertices
 */
void createGrid(std::vector<PosColorVertex> &gridVertices, std::vector<uint16_t> &gridIndices);


