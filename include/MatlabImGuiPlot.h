#pragma once

/// STL headers
#include <algorithm>
#include <array>
#include <functional>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <map>
#include <math.h>
#include <memory>
#include <mutex>
#include <numeric>
#include <string>
#include <stdio.h>
#include <time.h>
#include <vector>

#include "../bindings/imgui_impl_glfw.h"
#include "../bindings/imgui_impl_opengl3.h"
#include "imgui.h"
#include "implot.h"

/// Miscellaneous plot info
namespace ImPlot
{
/// Dimension verifier
enum Dimension_e : size_t
{
    ZERO,
    ONE,
    TWO,
    MORE,
};

enum Miscellaneous_e : size_t
{
    PLOT_TYPES,
    MARKER_SHAPES,
    COLORS,
    LINE_WIDTHS,
    MARKER_SIZES,
    TITLE,
    LABELS,
    LEGENDS,
    LIMITS,
    UNCERN_LBOUND,
    UNCERN_UBOUND,
};

struct PlotInfo_t
{
    bool plotTypesAvailable;
    bool markerShapesAvailable;
    bool colorsAvailable;
    bool lineWidthAvailable;
    bool markerSizeAvailable;
    bool titleAvailable;
    bool labelsAvailable;
    bool legendsAvailable;
    bool limitsAvailable;
    bool uncertaintyLowerBoundAvailable;
    bool uncertaintyUpperBoundAvailable;
    bool onlyStructures;
};

/// Plot infomations
struct PlotData_t
{
    std::vector<std::vector<double>> data1;
    std::vector<std::vector<double>> data2;
    std::vector<std::string>         plotTypes;
    std::vector<ImPlotMarker_>       markerShapes;
    std::vector<ImVec4>              colors;
    std::vector<double>              lineWidth;
    std::vector<double>              markerSize;
    std::vector<std::string>         title;
    std::vector<std::string>         labels;
    std::vector<std::string>         legends;
    std::vector<double>              limits;
    std::vector<std::vector<double>> uncertaintyLowerBound;
    std::vector<std::vector<double>> uncertaintyUpperBound;

    PlotInfo_t plotInfo;

    std::vector<std::vector<double>> getData1() const
    {
        return data1;
    }
    std::vector<std::vector<double>> getData2() const
    {
        return data2;
    }
    std::vector<std::string> getPlotTypes() const
    {
        return plotTypes;
    }
    std::vector<ImPlotMarker_> getMarkerShapes() const
    {
        return markerShapes;
    }
    std::vector<ImVec4> getColors() const
    {
        return colors;
    }
    std::vector<double> getLineWidth() const
    {
        return lineWidth;
    }
    std::vector<double> getMarkerSize() const
    {
        return markerSize;
    }
    std::vector<std::string> getTitle() const
    {
        return title;
    }
    std::vector<std::string> getLabels() const
    {
        return labels;
    }
    std::vector<std::string> getLegends() const
    {
        return legends;
    }
    std::vector<double> getLimits() const
    {
        return limits;
    }
    std::vector<std::vector<double>> getUncertaintyLowerBound() const
    {
        return uncertaintyLowerBound;
    }
    std::vector<std::vector<double>> getUncertaintyUpperBound() const
    {
        return uncertaintyUpperBound;
    }

    PlotInfo_t getPlotInfo() const
    {
        return plotInfo;
    }
};

struct MatlabInput_t
{
    std::string             figureConfig;
    std::vector<double>     subModuleDimensions;
    std::vector<PlotData_t> plotData;

    std::string getMatlabFigureNames() const
    {
        return figureConfig;
    }

    std::vector<double> getSubModuleDimensions() const
    {
        return subModuleDimensions;
    }

    std::vector<PlotData_t> getMatlabPlotData() const
    {
        return plotData;
    }
};

} // namespace ImPlot

class MatlabImGuiPlot
{
  public:
    MatlabImGuiPlot() = default;
    MatlabImGuiPlot(std::vector<ImPlot::MatlabInput_t>& data);
    ~MatlabImGuiPlot(){};

    static std::vector<std::string> getAvailableInputVariableNames()
    {
        return {
            "PlotTypes",
            "MarkerShapes",
            "Colors",
            "LineWidths",
            "MarkerSizes",
            "Title",
            "Labels",
            "Legends",
            "Limits",
            "UncertaintyLowerBound",
            "UncertaintyUpperBound",
        };
    }

    static std::vector<std::string> getAvailablePlotTypes()
    {
        return {
            "Line",
            "Scatter",
            "Bars",
        };
    }

    static std::map<std::string, ImVec4> getAvailableColorTypes()
    {
        return {

            {"Background", ImVec4(0.800f, 0.800f, 0.800f, 1.00f)},
            {"LightGray", ImVec4(0.800f, 0.800f, 0.800f, 1.00f)},
            {"Gray", ImVec4(0.502f, 0.502f, 0.502f, 1.00f)},
            {"Indigo", ImVec4(0.294f, 0.000f, 0.510f, 1.00f)},
            {"BlueViolet", ImVec4(0.541f, 0.169f, 0.886f, 1.00f)},
            {"DarkestBlue", ImVec4(0.000f, 0.000f, 0.500f, 1.00f)},
            {"DarkBlue", ImVec4(0.000f, 0.000f, 0.750f, 1.00f)},
            {"Blue", ImVec4(0.000f, 0.000f, 1.000f, 1.00f)},
            {"LightBlue", ImVec4(0.350f, 0.350f, 1.000f, 1.00f)},
            {"DodgerBlue", ImVec4(0.118f, 0.565f, 1.000f, 1.00f)},
            {"Cyan", ImVec4(0.000f, 1.000f, 1.000f, 1.00f)},
            {"DarkCyan", ImVec4(0.000f, 0.600f, 0.600f, 1.00f)},
            {"Lime", ImVec4(0.000f, 1.000f, 0.000f, 1.00f)},
            {"Green", ImVec4(0.000f, 0.800f, 0.000f, 1.00f)},
            {"LightGreen", ImVec4(0.000f, 1.000f, 0.000f, 1.00f)},
            {"DarkGreen", ImVec4(0.000f, 0.600f, 0.000f, 1.00f)},
            {"Yellow", ImVec4(1.000f, 1.000f, 0.000f, 1.00f)},
            {"DarkYellow", ImVec4(0.700f, 0.700f, 0.000f, 1.00f)},
            {"Orange", ImVec4(1.000f, 0.647f, 0.000f, 1.00f)},
            {"Red", ImVec4(1.000f, 0.000f, 0.000f, 1.00f)},
            {"DarkRed", ImVec4(0.700f, 0.000f, 0.000f, 1.00f)},
            {"Brown", ImVec4(0.545f, 0.000f, 0.000f, 1.00f)},
            {"Magenta", ImVec4(1.000f, 0.000f, 1.000f, 1.00f)},
            {"DarkMagenta", ImVec4(0.600f, 0.000f, 0.600f, 1.00f)},
            {"Black", ImVec4(0.000f, 0.000f, 0.000f, 1.00f)},
            {"White", ImVec4(1.000f, 1.000f, 1.000f, 1.00f)},
            {"IndianRed", ImVec4(0.80F, 0.36F, 0.36F, 1.00f)},
            {"MediumVioletRed", ImVec4(0.7804F, 0.0824F, 0.5216F, 1.00f)},
            {"OrangeRed", ImVec4(1.0F, 0.27F, 0.0F, 1.00f)},
            {"VioletRed", ImVec4(0.97F, 0.33F, 0.58F, 1.00f)},
            {"DarkOliveGreen", ImVec4(0.33F, 0.42F, 0.18F, 1.00f)},
            {"ForestGreen", ImVec4(0.137255F, 0.556863F, 0.137255F, 1.00f)},
            {"GreenYellow", ImVec4(0.68F, 1.0F, 0.18F, 1.00f)},
            {"LimeGreen", ImVec4(0.196078F, 0.8F, 0.196078F, 1.00f)},
            {"OliveDrab", ImVec4(0.419608F, 0.556863F, 0.137255F, 1.00f)},
            {"MediumSeaGreen", ImVec4(0.24F, 0.7F, 0.44F, 1.00f)},
            {"MediumSpringGreen", ImVec4(0.0F, 0.98F, 0.6F, 1.00f)},
            {"PaleGreen", ImVec4(0.60F, 0.98F, 0.60F, 1.00f)},
            {"SeaGreen", ImVec4(0.18F, 0.55F, 0.34F, 1.00f)},
            {"SpringGreen", ImVec4(0.0F, 1.0F, 0.498039F, 1.00f)},
            {"YellowGreen", ImVec4(0.6F, 0.8F, 0.196078F, 1.00f)},
            {"GreenCopper", ImVec4(0.32F, 0.49F, 0.46F, 1.00f)},
            {"DarkGreenCopper", ImVec4(0.29F, 0.46F, 0.43F, 1.00f)},
            {"HuntersGreen", ImVec4(0.21F, 0.37F, 0.23F, 1.00f)},
            {"CadetBlue", ImVec4(0.372549F, 0.623529F, 0.623529F, 1.00f)},
            {"CornflowerBlue", ImVec4(0.39F, 0.58F, 0.93F, 1.00f)},
            {"DarkSlateBlue", ImVec4(0.28F, 0.24F, 0.55F, 1.00f)},
            {"LightSteelBlue", ImVec4(0.27F, 0.51F, 0.71F, 1.00f)},
            {"MediumBlue", ImVec4(0.0F, 0.0F, 0.8F, 1.00f)},
            {"MediumSlateBlue", ImVec4(0.48F, 0.41F, 0.93F, 1.00f)},
            {"MidnightBlue", ImVec4(0.10F, 0.10F, 0.44F, 1.00f)},
            {"NavyBlue", ImVec4(0.0F, 0.0F, 0.50F, 1.00f)},
            {"SkyBlue", ImVec4(0.53F, 0.81F, 0.92F, 1.00f)},
            {"SlateBlue", ImVec4(0.42F, 0.35F, 0.80F, 1.00f)},
            {"SteelBlue", ImVec4(0.27F, 0.51F, 0.71F, 1.00f)},
            {"RichBlue", ImVec4(0.35F, 0.35F, 0.67F, 1.00f)},
            {"NeonBlue", ImVec4(0.27F, 0.40F, 1.0F, 1.00f)},
            {"NewMidnightBlue", ImVec4(0.00F, 0.00F, 0.61F, 1.00f)},
            {"DimGray", ImVec4(0.41F, 0.41F, 0.41F, 1.00f)},
            {"VLightGray", ImVec4(0.83F, 0.83F, 0.83F, 1.00f)},
            {"DarkSlateGray", ImVec4(0.184314F, 0.309804F, 0.309804F, 1.00f)},
            {"MandarinOrange", ImVec4(0.96F, 0.60F, 0.02F, 1.00f)},
            {"Aquamarine", ImVec4(0.50F, 1.0F, 0.83F, 1.00f)},
            {"Coral", ImVec4(1.0F, 0.498039F, 0.31F, 1.00f)},
            {"DarkOrchid", ImVec4(0.6F, 0.196078F, 0.8F, 1.00f)},
            {"DarkTurquoise", ImVec4(0.0F, 0.81F, 0.82F, 1.00f)},
            {"Firebrick", ImVec4(0.70F, 0.13F, 0.13F, 1.00f)},
            {"Gold", ImVec4(1.0F, 0.84F, 0.0F, 1.00f)},
            {"Goldenrod", ImVec4(0.85F, 0.65F, 0.13F, 1.00f)},
            {"Khaki", ImVec4(0.94F, 0.90F, 0.55F, 1.00f)},
            {"Maroon", ImVec4(0.50F, 0.0F, 0.0F, 1.00f)},
            {"MediumAquamarine", ImVec4(0.4F, 0.8F, 0.67F, 1.00f)},
            {"MediumGoldenrod", ImVec4(0.917647F, 0.917647F, 0.678431F, 1.00f)},
            {"MediumOrchid", ImVec4(0.73F, 0.33F, 0.83F, 1.00f)},
            {"MediumTurquoise", ImVec4(0.28F, 0.82F, 0.80F, 1.00f)},
            {"Orchid", ImVec4(0.858824F, 0.439216F, 0.858824F, 1.00f)},
            {"Pink", ImVec4(1.0F, 0.75F, 0.80F, 1.00f)},
            {"Plum", ImVec4(0.87F, 0.63F, 0.87F, 1.00f)},
            {"Salmon", ImVec4(0.98F, 0.50F, 0.45F, 1.00f)},
            {"Sienna", ImVec4(0.63F, 0.32F, 0.18F, 1.00f)},
            {"Tan", ImVec4(0.82F, 0.71F, 0.55F, 1.00f)},
            {"Thistle", ImVec4(0.847059F, 0.74902F, 0.847059F, 1.00f)},
            {"Turquoise", ImVec4(0.25F, 0.88F, 0.82F, 1.00f)},
            {"Violet", ImVec4(0.93F, 0.51F, 0.93F, 1.00f)},
            {"Wheat", ImVec4(0.96F, 0.87F, 0.70F, 1.00f)},
            {"SummerSky", ImVec4(0.22F, 0.69F, 0.87F, 1.00f)},
            {"Brass", ImVec4(0.71F, 0.65F, 0.26F, 1.00f)},
            {"Copper", ImVec4(0.72F, 0.45F, 0.20F, 1.00f)},
            {"Bronze", ImVec4(0.8F, 0.5F, 0.2F, 1.00f)},
            {"Silver", ImVec4(0.75F, 0.75F, 0.75F, 1.00f)},
            {"BrightGold", ImVec4(0.82F, 0.63F, 0.29F, 1.00f)},
            {"OldGold", ImVec4(0.81F, 0.71F, 0.23F, 1.00f)},
            {"Feldspar", ImVec4(0.82F, 0.57F, 0.46F, 1.00f)},
            {"Quartz", ImVec4(0.32F, 0.28F, 0.31F, 1.00f)},
            {"NeonPink", ImVec4(1.00F, 0.43F, 0.78F, 1.00f)},
            {"DarkPurple", ImVec4(0.19F, 0.10F, 0.20F, 1.00f)},
            {"CoolCopper", ImVec4(0.85F, 0.53F, 0.10F, 1.00f)},
            {"LightWood", ImVec4(0.91F, 0.76F, 0.65F, 1.00f)},
            {"MediumWood", ImVec4(0.65F, 0.50F, 0.39F, 1.00f)},
            {"DarkWood", ImVec4(0.52F, 0.37F, 0.26F, 1.00f)},
            {"SpicyPink", ImVec4(1.00F, 0.11F, 0.68F, 1.00f)},
            {"SemiSweetChocolate", ImVec4(0.42F, 0.26F, 0.15F, 1.00f)},
            {"BakersChocolate", ImVec4(0.36F, 0.20F, 0.09F, 1.00f)},
            {"Flesh", ImVec4(0.96F, 0.80F, 0.69F, 1.00f)},
            {"NewTan", ImVec4(0.92F, 0.78F, 0.62F, 1.00f)},
            {"VeryDarkBrown", ImVec4(0.35F, 0.16F, 0.14F, 1.00f)},
            {"DarkBrown", ImVec4(0.36F, 0.25F, 0.20F, 1.00f)},
            {"DarkTan", ImVec4(0.59F, 0.41F, 0.31F, 1.00f)},
            {"DustyRose", ImVec4(0.52F, 0.39F, 0.39F, 1.00f)},
            {"Scarlet", ImVec4(1.0F, 0.14F, 0.0F, 1.00f)},
            {"MediumPurple", ImVec4(0.58F, 0.44F, 0.86F, 1.00f)},
            {"LightPurple", ImVec4(0.87F, 0.58F, 0.98F, 1.00f)},
            {"VeryLightPurple", ImVec4(0.94F, 0.81F, 0.99F, 1.00f)}};
    }

    static std::map<std::array<std::string, 2U>, ImPlotMarker_> getAvailableMarkerInfo()
    {
        return {
            {{"o", "circle"}, ImPlotMarker_Circle},
            {{"s", "square"}, ImPlotMarker_Square},
            {{"d", "diamond"}, ImPlotMarker_Diamond},
            {{"^", "triangle (up)"}, ImPlotMarker_Up},
            {{"v", "triangle (down)"}, ImPlotMarker_Down},
            {{"<", "triangle (left)"}, ImPlotMarker_Left},
            {{">", "triangle (right)"}, ImPlotMarker_Right},
            {{"x", "x-mark"}, ImPlotMarker_Cross},
            {{"+", "plus"}, ImPlotMarker_Plus},
            {{"*", "star"}, ImPlotMarker_Asterisk},
        };
    }

  private:
    std::mutex mtx; // mutex for critical section

    /// <summary>
    /// Copy vector from std::vector to data[]
    /// </summary>
    template <class T>
    void copyVector(std::vector<T>& vec, T data[])
    {
        size_t index = 0;
        for (auto v : vec)
        {
            data[index] = v;
            index++;
        }
    }

    /// <summary>
    /// Plotting data errors check
    /// </summary>
    void errorCheck(ImPlot::PlotData_t& data);

    /// <summary>
    /// glfw error check by callback
    /// </summary>
    static void glfw_error_callback(int error, const char* description)
    {
        fprintf(stderr, "Glfw Error %d: %s\n", error, description);
    }

    /// <summary>
    /// Find the min max values of x and y data limits
    /// </summary>
    template <class T>
    void getDataMinMax(const std::vector<std::vector<T>>& data, T& min, T& max)
    {
        std::vector<T> internalVector = {};
        for (auto& dv : data)
        {
            for (auto& d : dv)
            {
                internalVector.push_back(d);
            }
        }
        if (internalVector.size() > 0)
        {
            std::sort(internalVector.begin(), internalVector.end());
            std::vector<double>::iterator it;
            it = std::unique(internalVector.begin(), internalVector.end());
            internalVector.resize(std::distance(internalVector.begin(), it));

            min = internalVector.at(0);
            max = internalVector.at(internalVector.size() - 1);
        }
    }

    /// <summary>
    /// Process the plot data
    /// </summary>
    /// <param name="info">Matlab's info</param>
    void processPlots(std::vector<ImPlot::MatlabInput_t>& info);
};
