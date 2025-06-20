//
// Created by Hippolyte Cosserat on 19/06/2025.
//

#ifndef COSMOS_H
#define COSMOS_H

#include <list>
#include "Astra.h"
#include "Vector2.h"

class Cosmos {
    std::list<Astra *> astras;

public:
    explicit Cosmos(int windows_count = 2, Vector2<int> windows_size = Vector2(500, 500));

    ~Cosmos();

    void draw() const;

    void update(double dt) const;
};

#endif //COSMOS_H
