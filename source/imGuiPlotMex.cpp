
/// Matlab C++ API
#include "mex.hpp"
#include "mexAdapter.hpp"

/// Matlab to imGui plot support
#include "MatlabImGuiPlot.h"

/// Current Matlab ImGui Plot version
#define MATLAB_IMGUI_MEX_VERSION 2

class MexFunction : public matlab::mex::Function
{
  private:
    /// Factory helper for creating Matlab arrays
    matlab::data::ArrayFactory mFactory;

    /// Declaring variables
    std::array<size_t, 2U> mDataDimension;
    size_t                 mColumnDimension;

    /// type def
    typedef matlab::mex::ArgumentList                            mArgument_t;
    typedef std::map<std::string, ImVec4>                        mImVec4_t;
    typedef std::map<std::array<std::string, 2U>, ImPlotMarker_> mMarkerType_t;
    typedef matlab::data::TypedArray<matlab::data::MATLABString> mString_t;
    typedef std::vector<std::string>                             mVecString_t;

    /// Populate unique numbers
    template <class T>
    struct UniqueNumber_t
    {
        T current;
        UniqueNumber_t()
        {
            current = 0;
        }
        T operator()()
        {
            return ++current;
        }
    };

    /// Plot infomations
    std::string                        mFigureConfiguration;
    std::vector<ImPlot::MatlabInput_t> mInputFromMatlab;
    std::vector<ImPlot::PlotData_t>    mStructurePlottingInfo;
    std::vector<double>                mSubPlotDimension;

    /// Handle to Matlab engine
    std::shared_ptr<matlab::engine::MATLABEngine> mMatlabPtr = getEngine();

    /// Helper functions

    bool checkStructureElements(matlab::data::StructArray const& matlabStructArray);

    void displayError(std::string errorMessage);

    template <class T, class U>
    U dataFormat(T& data);

    void displayOnMATLAB(std::ostringstream& stream);

    void emptyFieldInformation(std::string fieldName, size_t index);

    template <class T>
    ImPlot::PlotData_t formatStructures(matlab::data::StructArray& matlabStructArray);

    mImVec4_t getAvailableColorTypes() const;

    mVecString_t getAvailableInputVariableNames() const;

    mMarkerType_t getAvailableMarkerInfo() const;

    template <class T, class U>
    U getColorFormat(T& data);

    template <class T, class U>
    U getMarkerFormat(T& inputs);

    template <class T, class U, class W>
    void inputDataExtractions(T& data, U& formattedData);

    void invalidFieldInformation(std::string fieldName, size_t index);

    template <class T>
    void process(mArgument_t& data);

    void toLower(std::string& data);

    bool validateArguments(mArgument_t outputs, mArgument_t inputs);

  public:
    /// main entry point
    void operator()(mArgument_t outputs, mArgument_t inputs);
};

void MexFunction::operator()(mArgument_t outputs, mArgument_t inputs)
{
    // Check to verify the validity of the Matlab’s input
    if (validateArguments(outputs, inputs))
    {
        if (inputs.size() > ImPlot::Dimension_e::ZERO)
        {
            /// Processs the input data
            mStructurePlottingInfo = {};
            mInputFromMatlab       = {};
            process<double>(inputs);
            std::shared_ptr<MatlabImGuiPlot> run(new MatlabImGuiPlot(mInputFromMatlab));
            mStructurePlottingInfo = {};
            mInputFromMatlab       = {};
        }
    }
}

// Make sure that the passed structure has valid data.
bool MexFunction::checkStructureElements(matlab::data::StructArray const& matlabStructArray)
{
    bool               status = true;
    std::ostringstream stream;
    size_t             nfields               = matlabStructArray.getNumberOfFields();
    auto               fields                = matlabStructArray.getFieldNames();
    size_t             total_num_of_elements = matlabStructArray.getNumberOfElements();
    mVecString_t       fieldNames(fields.begin(), fields.end());

    // Produce error if structure has more than 2 fields.
    if (nfields == 0)
    {
        displayError("Struct must consist number of entries atleast one data.");
        status = false;
    }
    return status;
}

template <class T, class U>
U MexFunction::dataFormat(T& data)
{
    T object(data);
    return U(object.begin(), object.end());
}

// Helper function to generate an error message from given string,
// and display it over MATLAB command prompt.

void MexFunction::displayError(std::string errorMessage)
{
    mMatlabPtr->feval(u"error", 0, std::vector<matlab::data::Array>({mFactory.createScalar(errorMessage)}));
}

void MexFunction::displayOnMATLAB(std::ostringstream& stream)
{
    /// Pass stream content to MATLAB fprintf function
    mMatlabPtr->feval(u"fprintf", 0, std::vector<matlab::data::Array>({mFactory.createScalar(stream.str())}));
    stream.str(""); /// Clear stream buffer
}

// Helper function to information about an empty field in the structure.
void MexFunction::emptyFieldInformation(std::string fieldName, size_t index)
{
    std::ostringstream stream;
    stream << "Field: " << std::string(fieldName) << " of the element at index: " << index + 1 << " is empty."
           << std::endl;
    displayOnMATLAB(stream);
}

template <class T>
ImPlot::PlotData_t MexFunction::formatStructures(matlab::data::StructArray& matlabStructArray)
{
    ImPlot::PlotData_t plottingInfo            = {};
    size_t             miscellaneousIndexStart = ImPlot::Dimension_e::ONE;
    auto               inputTypes              = getAvailableInputVariableNames();
    for (auto& type : inputTypes)
    {
        toLower(type);
    }

    if (matlabStructArray.getType() == matlab::data::ArrayType::STRUCT)
    {
        size_t total_num_of_elements = matlabStructArray.getNumberOfElements();
        auto   fields                = matlabStructArray.getFieldNames();

        // Content of each structure
        mVecString_t fieldNames(fields.begin(), fields.end());

        if (fieldNames.size() > 0 && fieldNames[0].compare("data1") == ImPlot::Dimension_e::ZERO)
        {
            matlab::data::TypedArray<T> structField = matlabStructArray[0][fieldNames[0]];
            mDataDimension   = {structField.getDimensions().at(0), structField.getDimensions().at(1)};
            mColumnDimension = mDataDimension[ImPlot::Dimension_e::ONE];

            inputDataExtractions<matlab::data::TypedArray<T>, std::vector<std::vector<T>>, std::vector<T>>(
                structField, plottingInfo.data1);
            plottingInfo.plotInfo.onlyStructures = true;
        }

        if (fieldNames.size() > 1)
        {
            for (auto str : fieldNames)
            {
                const auto internalStr = str;
                toLower(str);

                // y-data if there are any.
                if (str.compare("data2") == ImPlot::Dimension_e::ZERO)
                {
                    matlab::data::TypedArray<T> structField = matlabStructArray[0][internalStr];
                    inputDataExtractions<matlab::data::TypedArray<T>, std::vector<std::vector<T>>, std::vector<T>>(
                        structField, plottingInfo.data2);
                    miscellaneousIndexStart = ImPlot::Dimension_e::TWO;
                }

                // PlotTypes
                if (inputTypes[ImPlot::Miscellaneous_e::PLOT_TYPES].compare(str) == ImPlot::Dimension_e::ZERO)
                {
                    mString_t internalInput                  = matlabStructArray[0][internalStr];
                    plottingInfo.plotTypes                   = dataFormat<mString_t, mVecString_t>(internalInput);
                    plottingInfo.plotInfo.plotTypesAvailable = true;
                }

                // MarkerShapes
                if (inputTypes[ImPlot::Miscellaneous_e::MARKER_SHAPES].compare(str) == ImPlot::Dimension_e::ZERO)
                {
                    mString_t internalInput   = matlabStructArray[0][internalStr];
                    plottingInfo.markerShapes = getMarkerFormat<mString_t, std::vector<ImPlotMarker_>>(internalInput);
                    plottingInfo.plotInfo.markerShapesAvailable = true;
                }

                // Colors
                if (inputTypes[ImPlot::Miscellaneous_e::COLORS].compare(str) == ImPlot::Dimension_e::ZERO)
                {
                    mString_t internalInput = matlabStructArray[0][internalStr];
                    plottingInfo.colors     = getColorFormat<mString_t, std::vector<ImVec4>>(internalInput);
                    plottingInfo.plotInfo.colorsAvailable = true;
                }
                // LineWidths
                if (inputTypes[ImPlot::Miscellaneous_e::LINE_WIDTHS].compare(str) == ImPlot::Dimension_e::ZERO)
                {
                    matlab::data::TypedArray<T> internalInput = matlabStructArray[0][internalStr];
                    plottingInfo.lineWidth = dataFormat<matlab::data::TypedArray<T>, std::vector<T>>(internalInput);
                    plottingInfo.plotInfo.lineWidthAvailable = true;
                }

                // MarkerSizes
                if (inputTypes[ImPlot::Miscellaneous_e::MARKER_SIZES].compare(str) == ImPlot::Dimension_e::ZERO)
                {
                    matlab::data::TypedArray<T> internalInput = matlabStructArray[0][internalStr];
                    plottingInfo.markerSize = dataFormat<matlab::data::TypedArray<T>, std::vector<T>>(internalInput);
                    plottingInfo.plotInfo.markerSizeAvailable = true;
                }

                // Title
                if (inputTypes[ImPlot::Miscellaneous_e::TITLE].compare(str) == ImPlot::Dimension_e::ZERO)
                {
                    mString_t internalInput              = matlabStructArray[0][internalStr];
                    plottingInfo.title                   = dataFormat<mString_t, mVecString_t>(internalInput);
                    plottingInfo.plotInfo.titleAvailable = true;
                }
                // Labels
                if (inputTypes[ImPlot::Miscellaneous_e::LABELS].compare(str) == ImPlot::Dimension_e::ZERO)
                {
                    mString_t internalInput               = matlabStructArray[0][internalStr];
                    plottingInfo.labels                   = dataFormat<mString_t, mVecString_t>(internalInput);
                    plottingInfo.plotInfo.labelsAvailable = true;
                }
                // Legends
                if (inputTypes[ImPlot::Miscellaneous_e::LEGENDS].compare(str) == ImPlot::Dimension_e::ZERO)
                {
                    mString_t internalInput                = matlabStructArray[0][internalStr];
                    plottingInfo.legends                   = dataFormat<mString_t, mVecString_t>(internalInput);
                    plottingInfo.plotInfo.legendsAvailable = true;
                }

                // Limits
                if (inputTypes[ImPlot::Miscellaneous_e::LIMITS].compare(str) == ImPlot::Dimension_e::ZERO)
                {
                    matlab::data::TypedArray<T> internalInput = matlabStructArray[0][internalStr];
                    plottingInfo.limits = dataFormat<matlab::data::TypedArray<T>, std::vector<T>>(internalInput);
                    plottingInfo.plotInfo.limitsAvailable = true;
                }

                // Uncertainty Lower-Bound
                if (inputTypes[ImPlot::Miscellaneous_e::UNCERN_LBOUND].compare(str) == ImPlot::Dimension_e::ZERO)
                {
                    matlab::data::TypedArray<T> internalInput = matlabStructArray[0][internalStr];

                    inputDataExtractions<matlab::data::TypedArray<T>, std::vector<std::vector<T>>, std::vector<T>>(
                        internalInput, plottingInfo.uncertaintyLowerBound);
                    plottingInfo.plotInfo.uncertaintyLowerBoundAvailable = true;
                }

                // Uncertainty Upper-Bound
                if (inputTypes[ImPlot::Miscellaneous_e::UNCERN_UBOUND].compare(str) == ImPlot::Dimension_e::ZERO)
                {
                    matlab::data::TypedArray<T> internalInput = matlabStructArray[0][internalStr];
                    inputDataExtractions<matlab::data::TypedArray<T>, std::vector<std::vector<T>>, std::vector<T>>(
                        internalInput, plottingInfo.uncertaintyUpperBound);
                    plottingInfo.plotInfo.uncertaintyUpperBoundAvailable = true;
                }
            }
        }

        // Only 1D data is available so available data is considered y data and populate unique x data
        if (miscellaneousIndexStart == ImPlot::Dimension_e::ONE)
        {
            plottingInfo.data2 = plottingInfo.data1;
            plottingInfo.data1 = {};
            for (const auto data : plottingInfo.data2)
            {
                std::vector<T>    vec(data.size());
                UniqueNumber_t<T> GenerateData_t;
                std::generate(vec.begin(), vec.end(), GenerateData_t);
                plottingInfo.data1.push_back(vec);
            }
        }
    }

    return plottingInfo;
}

MexFunction::mImVec4_t MexFunction::getAvailableColorTypes() const
{
    return MatlabImGuiPlot::getAvailableColorTypes();
}

MexFunction::mVecString_t MexFunction::getAvailableInputVariableNames() const
{
    return MatlabImGuiPlot::getAvailableInputVariableNames();
}

MexFunction::mMarkerType_t MexFunction::getAvailableMarkerInfo() const
{
    return MatlabImGuiPlot::getAvailableMarkerInfo();
}

template <class T, class U>
U MexFunction::getColorFormat(T& data)
{
    T    internalInput    = data;
    auto colorInfo        = dataFormat<T, mVecString_t>(internalInput);
    auto colorDefinitions = getAvailableColorTypes();
    U    output           = {};
    for (auto& color : colorInfo)
    {
        for (auto& def : colorDefinitions)
        {
            if ((color.compare(def.first) == ImPlot::Dimension_e::ZERO))
            {
                output.push_back(def.second);
            }
        }
    }
    return output;
}

// std::vector<ImPlotMarker_>
template <class T, class U>
U MexFunction::getMarkerFormat(T& data)
{
    T    internalInput     = data;
    auto markerShapes      = dataFormat<T, mVecString_t>(internalInput);
    auto markerDefinitions = getAvailableMarkerInfo();
    U    output            = {};
    for (auto& shape : markerShapes)
    {
        for (auto& def : markerDefinitions)
        {
            if ((shape.compare(def.first.at(ImPlot::Dimension_e::ZERO)) == ImPlot::Dimension_e::ZERO) ||
                (shape.compare(def.first.at(ImPlot::Dimension_e::ONE)) == ImPlot::Dimension_e::ZERO))
            {
                output.push_back(def.second);
            }
        }
    }

    return output;
}

// Helper function to information about an invalid field in the structure.
void MexFunction::invalidFieldInformation(std::string fieldName, size_t index)
{
    std::ostringstream stream;
    stream << "Field: " << std::string(fieldName) << " of the element at index: " << index + 1
           << " contains wrong value." << std::endl;
    displayOnMATLAB(stream);
}

template <class T, class U, class W>
void MexFunction::inputDataExtractions(T& data, U& formattedData)
{
    auto   internalInput = dataFormat<T, W>(data);
    size_t numElements   = internalInput.size() / mColumnDimension;
    size_t numEleCounter = ImPlot::Dimension_e::ONE;
    W      internalVec   = {};

    for (auto& in : internalInput)
    {
        internalVec.push_back(in);
        if (numEleCounter == numElements)
        {
            formattedData.push_back(internalVec);
            internalVec   = {};
            numEleCounter = ImPlot::Dimension_e::ZERO;
        }
        numEleCounter++;
    }
}

template <class T>
void MexFunction::process(mArgument_t& data)
{
    // Go through all the Matlab's input
    for (auto& d : data)
    {
        // Plot types i.e., plots or subplots.
        if (d.getType() == matlab::data::ArrayType::MATLAB_STRING)
        {
            mString_t internalInput = d;
            mFigureConfiguration    = dataFormat<mString_t, mVecString_t>(internalInput)[0];
        }

        // If subplots, then the dimension.
        if (d.getType() == matlab::data::ArrayType::DOUBLE)
        {
            matlab::data::TypedArray<T> internalInput = d;
            mSubPlotDimension = dataFormat<matlab::data::TypedArray<T>, std::vector<T>>(internalInput);
        }

        // Plot info only as structures.
        if (d.getType() == matlab::data::ArrayType::STRUCT)
        {
            matlab::data::StructArray matlabStructArray = d;
            auto                      res               = formatStructures<double>(matlabStructArray);
            mStructurePlottingInfo.push_back(res);
            if (static_cast<size_t>(mSubPlotDimension[0] * mSubPlotDimension[1]) == mStructurePlottingInfo.size())
            {
                mInputFromMatlab.push_back({mFigureConfiguration, mSubPlotDimension, mStructurePlottingInfo});
                mFigureConfiguration   = {};
                mSubPlotDimension      = {};
                mStructurePlottingInfo = {};
            }
        }
    }
}

void MexFunction::toLower(std::string& data)
{
    std::transform(data.begin(), data.end(), data.begin(), [](unsigned char c) { return std::tolower(c); });
}

bool MexFunction::validateArguments(mArgument_t outputs, mArgument_t inputs)
{
    bool   status  = true;
    size_t minSize = 3; // Need to be Name, Dimension and Structures with plot info.
    if (inputs.size() >= minSize)
    {
        for (size_t index = 0; index < inputs.size(); index++)
        {
            if (inputs[index].getType() == matlab::data::ArrayType::MATLAB_STRING)
            {
                size_t innerLoop = index + 1;
                if (inputs[innerLoop].getType() != matlab::data::ArrayType::DOUBLE)
                {
                    status               = false;
                    std::string errorMsg = "Input should be a vector with subplots arrangements";
                    displayError(errorMsg);
                }

                if (inputs[innerLoop].getType() == matlab::data::ArrayType::DOUBLE)
                {
                    matlab::data::TypedArray<double> internalInput = inputs[innerLoop];
                    mSubPlotDimension =
                        dataFormat<matlab::data::TypedArray<double>, std::vector<double>>(internalInput);

                    for (size_t innerId = 1; innerId <= mSubPlotDimension[0] * mSubPlotDimension[1]; innerId++)
                    {
                        if (inputs[innerLoop + innerId].getType() == matlab::data::ArrayType::STRUCT)
                        {
                            matlab::data::StructArray matlabStructArray = inputs[innerLoop + innerId];
                            status                                      = checkStructureElements(matlabStructArray);
                        }
                        else
                        {
                            status               = false;
                            std::string errorMsg = "Input should be structures.";
                            displayError(errorMsg);
                        }
                    }
                }
            }
        }
    }
    else
    {
        status = false;
        std::string errorMsg =
            "Incorrect input order and should be Name, Dimension, Structures with plot information.";
        displayError(errorMsg);
    }

    // Verify the output conditions.
    if (outputs.size() != ImPlot::Dimension_e::ZERO)
    {
        status               = false;
        std::string errorMsg = "No output arguments.";
        displayError(errorMsg);
    }

    return status;
}
