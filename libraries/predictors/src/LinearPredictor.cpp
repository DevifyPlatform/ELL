////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     LinearPredictor.cpp (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LinearPredictor.h"

// layers
#include "Coordinate.h"
#include "Coordinatewise.h"
#include "CoordinateListTools.h"
#include "Sum.h"

// stl
#include <memory>

namespace predictors
{
    LinearPredictor::LinearPredictor(uint64_t dim) : _w(dim), _b(0)
    {}

    void LinearPredictor::Reset()
    {
        _w.Reset();
        _b = 0;
    }

    double LinearPredictor::Predict(const dataset::IDataVector& dataVector) const
    {
        return dataVector.Dot(_w) + _b;
    }

    std::vector<double> LinearPredictor::GetWeightedElements(const dataset::IDataVector& dataVector) const
    {
        std::vector<double> weightedElements(_w.Size());
        dataVector.AddTo(weightedElements);
        for (size_t i = 0; i < _w.Size(); ++i)
        {
            weightedElements[i] *= _w[i];
        }
        return weightedElements;
    }

    void LinearPredictor::Scale(double scalar)
    {
        _w.Scale(scalar);
        _b *= scalar;
    }

    layers::CoordinateList LinearPredictor::AddToModel(layers::Model& model, layers::CoordinateList inputCoordinates) const
    {
        auto weightsLayer = std::make_unique<layers::Coordinatewise>(std::vector<double>(_w), std::move(inputCoordinates), layers::Coordinatewise::OperationType::multiply);
        auto weightsLayerCoordinates = model.AddLayer(std::move(weightsLayer));

        auto sumLayer = std::make_unique<layers::Sum>(std::move(weightsLayerCoordinates));
        auto sumLayerCoordinates = model.AddLayer(std::move(sumLayer));

        auto biasLayer = std::make_unique<layers::Coordinatewise>(_b, sumLayerCoordinates[0], layers::Coordinatewise::OperationType::add);
        auto biasLayerCoordinates = model.AddLayer(std::move(biasLayer));

        return biasLayerCoordinates;
    }
}
