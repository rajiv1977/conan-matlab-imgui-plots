#include "MatlabImGuiPlot.h"

void MatlabImGuiPlot::errorCheck(ImPlot::PlotData_t& data)
{
    size_t     dimensions = data.getData1().size();
    const auto plotInfo   = data.getPlotInfo();

    if (plotInfo.plotTypesAvailable)
    {
        if (data.getPlotTypes().size() != dimensions)
        {
            throw std::invalid_argument("Input and plot type dimensions are not equal");
        }
    }

    if (plotInfo.markerShapesAvailable)
    {
        if (data.getMarkerShapes().size() != dimensions)
        {
            throw std::invalid_argument("Input and marker shape dimensions are not equal");
        }
    }

    if (plotInfo.colorsAvailable)
    {
        if (data.getColors().size() != dimensions)
        {
            throw std::invalid_argument("Input and marker color dimensions are not equal");
        }
    }

    if (plotInfo.lineWidthAvailable)
    {
        if (data.getLineWidth().size() != dimensions)
        {
            throw std::invalid_argument("Input and marker line width dimensions are not equal");
        }
    }

    if (plotInfo.markerSizeAvailable)
    {
        if (data.getMarkerSize().size() != dimensions)
        {
            throw std::invalid_argument("Input and marker size dimensions are not equal");
        }
    }

    if (plotInfo.titleAvailable)
    {
        if (data.getTitle().size() != ImPlot::Dimension_e::ONE)
        {
            throw std::invalid_argument("Input and title dimensions are not equal");
        }
    }

    if (plotInfo.labelsAvailable)
    {
        if (data.getLabels().size() != ImPlot::Dimension_e::TWO)
        {
            throw std::invalid_argument("Input and lable dimensions are not equal");
        }
    }

    if (plotInfo.legendsAvailable)
    {
        if (data.getLegends().size() != dimensions)
        {
            throw std::invalid_argument("Input and legend dimensions are not equal");
        }
    }

    if (plotInfo.limitsAvailable)
    {
        if (data.getLimits().size() != ImPlot::Dimension_e::TWO * 2)
        {
            throw std::invalid_argument("Input and x and y limits dimensions are not equal");
        }
    }

    if (plotInfo.uncertaintyLowerBoundAvailable)
    {
        if (data.getUncertaintyLowerBound().size() != dimensions)
        {
            throw std::invalid_argument("Input and UncertaintyLowerBound dimensions are not equal");
        }
    }

    if (plotInfo.uncertaintyLowerBoundAvailable)
    {
        if (data.getUncertaintyUpperBound().size() != dimensions)
        {
            throw std::invalid_argument("Input and UncertaintyUpperBound dimensions are not equal");
        }
    }
}

MatlabImGuiPlot::MatlabImGuiPlot(std::vector<ImPlot::MatlabInput_t>& data)
{
    mtx.lock();

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        std::cout << "glfw is not ini" << std::endl;
    }
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Matlab ImGui", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Issue creating window's graphics context" << std::endl;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    bool err = glewInit() != GLEW_OK;
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        std::cout << "Issue from glew ini" << std::endl;
    }

    int screen_width, screen_height;
    glfwGetFramebufferSize(window, &screen_width, &screen_height);
    glViewport(0, 0, screen_width, screen_height);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
    // io.ConfigViewportsNoAutoMerge = true; // Force all windows to use separate viewport
    io.ConfigWindowsMoveFromTitleBarOnly = true; // Allows mouse functions in world visualization

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark(); // Setup Dear ImGui style

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        // feed inputs to dear imgui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::CreateContext();
        ImPlot::CreateContext();

        processPlots(data);

        // Render dear imgui into screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    mtx.unlock();
};

void MatlabImGuiPlot::processPlots(std::vector<ImPlot::MatlabInput_t>& info)
{
    for (auto& in : info)
    {
        auto subPlotDimensions = in.getSubModuleDimensions();
        auto dataArray         = in.getMatlabPlotData();

        ImGui::Begin(in.getMatlabFigureNames().c_str());

        static ImPlotShadedFlags flags                = 0;
        static float             barSize              = 0.25f;
        static float             markerSize           = 1.0f;
        static float             uncertaintyIntensity = 0.25f;

        if (ImPlot::BeginSubplots("##ItemSharing", subPlotDimensions[0], subPlotDimensions[1], ImVec2(-1, -1), flags))
        {
            for (auto& data : dataArray)
            {

                errorCheck(data);

                double minData1Elem = DBL_MAX;
                double maxData1Elem = DBL_TRUE_MIN;
                MatlabImGuiPlot::getDataMinMax<double>(data.getData1(), minData1Elem, maxData1Elem);

                double minData2Elem = DBL_MAX;
                double maxData2Elem = DBL_TRUE_MIN;
                MatlabImGuiPlot::getDataMinMax<double>(data.getData2(), minData2Elem, maxData2Elem);

                size_t dimensions  = data.getData1().size();
                size_t numElements = data.getData1().at(ImPlot::Dimension_e::ZERO).size();

                /// title selection
                std::string internalTitle = {};
                internalTitle             = (data.getTitle().size() > ImPlot::Dimension_e::ZERO)
                                                ? data.getTitle().at(ImPlot::Dimension_e::ZERO)
                                                : "Figure";

                if (ImPlot::BeginPlot(internalTitle.c_str()))
                {
                    ImPlot::SetupLegend(ImPlotLocation_South, ImPlotLegendFlags_Outside | ImPlotLegendFlags_Horizontal);

                    // label selections
                    if (data.plotInfo.labelsAvailable)
                    {
                        ImPlot::SetupAxes(data.getLabels()[ImPlot::Dimension_e::ZERO].c_str(),
                                          data.getLabels()[ImPlot::Dimension_e::ONE].c_str());
                    }

                    // set the axis limits
                    if (data.plotInfo.limitsAvailable)
                    {
                        ImPlot::SetupAxesLimits(
                            data.getLimits().at(ImPlot::Dimension_e::ZERO),
                            data.getLimits().at(ImPlot::Dimension_e::ONE),
                            data.getLimits().at(ImPlot::Dimension_e::TWO),
                            data.getLimits().at(ImPlot::Dimension_e::ONE + ImPlot::Dimension_e::TWO));
                    }
                    else
                    {
                        ImPlot::SetupAxesLimits(minData1Elem, maxData1Elem, minData2Elem, maxData2Elem);
                    }

                    for (size_t index = ImPlot::Dimension_e::ZERO; index < dimensions; index++)
                    {

                        // style
                        if (data.plotInfo.markerSizeAvailable)
                        {
                            markerSize = data.getMarkerSize().at(index);
                        }
                        ImPlot::PushStyleVar(ImPlotStyleVar_MarkerSize, markerSize);

                        double xData[SHRT_MAX];
                        double yData[SHRT_MAX];
                        copyVector<double>(data.getData1().at(index), xData);
                        copyVector<double>(data.getData2().at(index), yData);

                        double yUpperBoundUncertainty[SHRT_MAX];
                        double yLowerBoundUncertainty[SHRT_MAX];

                        if (data.plotInfo.uncertaintyLowerBoundAvailable &&
                            data.plotInfo.uncertaintyUpperBoundAvailable)
                        {
                            copyVector<double>(data.getUncertaintyLowerBound().at(index), yLowerBoundUncertainty);
                            copyVector<double>(data.getUncertaintyUpperBound().at(index), yUpperBoundUncertainty);
                        }

                        std::string internalLegend = {};
                        if (data.plotInfo.legendsAvailable)
                        {
                            internalLegend =
                                (data.getLegends().size() > ImPlot::Dimension_e::ZERO) ? data.getLegends()[index] : " ";
                        }

                        
                        if (data.plotInfo.plotTypesAvailable)
                        {
                            // Line plots
                            if (data.getPlotTypes()[index].compare("Line") == 0)
                            {
                                if (data.plotInfo.colorsAvailable)
                                {
                                    ImPlot::PushStyleColor(ImPlotCol_Line, data.getColors().at(index));
                                }
                                if (data.plotInfo.markerShapesAvailable)
                                {
                                    ImPlot::SetNextMarkerStyle(data.getMarkerShapes().at(index));
                                }
                                ImPlot::PlotLine(internalLegend.c_str(), xData, yData, numElements);
                                if (data.plotInfo.colorsAvailable)
                                {
                                    ImPlot::PopStyleColor();
                                }
                            }

                            /// Bar plots
                            if (data.getPlotTypes()[index].compare("Bars") == 0)
                            {
                                if (data.plotInfo.colorsAvailable)
                                {
                                    ImPlot::PushStyleColor(ImPlotCol_Fill, data.getColors().at(index));
                                }

                                ImPlot::PlotBars(internalLegend.c_str(), xData, yData, numElements, barSize);

                                if (data.plotInfo.colorsAvailable)
                                {
                                    ImPlot::PopStyleColor();
                                }
                            }

                            /// Scatter plots
                            if (data.getPlotTypes()[index].compare("Scatter") == 0)
                            {
                                if (data.plotInfo.colorsAvailable)
                                {
                                    ImPlot::PushStyleColor(ImPlotCol_Fill, data.getColors().at(index));
                                }
                                if (data.plotInfo.markerShapesAvailable)
                                {
                                    ImPlot::SetNextMarkerStyle(data.getMarkerShapes().at(index));
                                }
                                ImPlot::PlotScatter(internalLegend.c_str(), xData, yData, numElements);
                                if (data.plotInfo.colorsAvailable)
                                {
                                    ImPlot::PopStyleColor();
                                }
                            }
                        }
                        else
                        {
                            if (data.plotInfo.colorsAvailable)
                            {
                                ImPlot::PushStyleColor(ImPlotCol_Line, data.getColors().at(index));
                            }
                            if (data.plotInfo.markerShapesAvailable)
                            {
                                ImPlot::SetNextMarkerStyle(data.getMarkerShapes().at(index));
                            }
                            ImPlot::PlotLine(internalLegend.c_str(), xData, yData, numElements);
                            if (data.plotInfo.colorsAvailable)
                            {
                                ImPlot::PopStyleColor();
                            }
                        }

                        /// If uncertainty info
                        if (data.plotInfo.uncertaintyLowerBoundAvailable &&
                            data.plotInfo.uncertaintyUpperBoundAvailable)
                        {
                            ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, uncertaintyIntensity);
                            if (data.plotInfo.colorsAvailable)
                            {
                                ImPlot::PushStyleColor(ImPlotCol_Fill, data.getColors().at(index));
                            }
                            ImPlot::PlotShaded(internalLegend.c_str(),
                                               xData,
                                               yUpperBoundUncertainty,
                                               yLowerBoundUncertainty,
                                               numElements);
                            if (data.plotInfo.colorsAvailable)
                            {
                                ImPlot::PopStyleColor();
                            }
                            ImPlot::PopStyleVar();
                        }
                    }
                    ImPlot::EndPlot();
                }
            }
            ImPlot::EndSubplots();
        }
        ImGui::End();
    }
}
