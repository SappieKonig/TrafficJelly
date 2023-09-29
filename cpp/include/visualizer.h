#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <string>
#include <vector>

/*
 * This visualizer data forms the basic object that may be pushed to the visualizer to be rendered.
 * Currently this data is somewhat clumsily put together and may be altered.
 */
struct VisualizerData
{
    std::string objectLabel;
    float position;
};

/*
 * This visualizer interface visualizes traffic models.
 * The traffic model provides the visualizer with data and the visualizer manages the rendering.
 */
class Visualizer
{
public:
    virtual ~Visualizer();
    virtual void update(VisualizerData const& data) = 0;
    virtual void reset() = 0; // Need not reset, just to call at the begin of a time step for potential cleanups.
};

/*
 * The basic visualizer visualizes collected data on updates by printing on a render call in some format.
 * Every render the collected data resets for the next time step.
 */
class BasicVisualizer : public Visualizer
{
private:
    std::vector<VisualizerData> collectedData;

public:
    void render();
    void reset() override;
    void update(VisualizerData const& data) override;
};

#endif
