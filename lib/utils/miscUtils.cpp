//
// Created by yanci on 14/04/23.
//

#include "miscUtils.h"

bool isGameFinished(Map map) {
    return (map.getBomberCount() == 1);
}
