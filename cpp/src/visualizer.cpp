#include "visualizer.h"
#include <iostream>

Visualizer::~Visualizer()
{

}

void BasicVisualizer::render()
{
    for (VisualizerData const& data : collectedData)
    {
        std::cout << "[Visualizer] Car at " << data.position << " in " << data.objectLabel << "\n"; // May want to sort by object label, but currently unimportant.
    }
    reset();
}

void BasicVisualizer::reset()
{
    collectedData.clear();
}

void BasicVisualizer::update(VisualizerData const& data)
{
    collectedData.push_back(data);
}
