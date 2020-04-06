#pragma once
#include <vector>
#include "Node.h"

std::vector<Input*> CreateInputs(std::vector<DataPort> itemarray);

std::vector<Output*> CreateOutputs(std::vector<DataPort> itemarray);