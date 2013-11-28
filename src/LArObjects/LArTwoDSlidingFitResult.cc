/**
 *  @file   LArContent/src/LArObjects/LArTwoDSlidingFitResult.cc
 * 
 *  @brief  Implementation of the lar two dimensional sliding fit result class.
 * 
 *  $Log: $
 */

#include "LArCalculators/LArPseudoLayerCalculator.h"

#include "LArHelpers/LArGeometryHelper.h"

#include "LArObjects/LArTwoDSlidingFitResult.h"

#include "Objects/Cluster.h"

#include <algorithm>
#include <cmath>
#include <limits>

using namespace pandora;

namespace lar
{

TwoDSlidingFitResult::TwoDSlidingFitResult() :
    m_pCluster(NULL),
    m_layerFitHalfWindow(0),
    m_axisIntercept(0.f, 0.f, 0.f),
    m_axisDirection(0.f, 0.f, 0.f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

void TwoDSlidingFitResult::GetLocalPosition(const CartesianVector &position, float &rL, float &rT) const
{
    const CartesianVector displacement(position - m_axisIntercept);
    const CartesianVector crossProduct(displacement.GetCrossProduct(m_axisDirection));

    rL = displacement.GetDotProduct(m_axisDirection);
    rT = (crossProduct.GetY() < 0.f) ? (-1.f * crossProduct.GetMagnitude()) : crossProduct.GetMagnitude();
}

//------------------------------------------------------------------------------------------------------------------------------------------

void TwoDSlidingFitResult::GetGlobalPosition(const float rL, const float rT, CartesianVector &position) const
{
    const CartesianVector positiveTDirection(m_axisDirection.GetCrossProduct(CartesianVector(0.f, 1.f, 0.f)));
    position = m_axisIntercept + (m_axisDirection * rL) + (positiveTDirection * rT);
}

//------------------------------------------------------------------------------------------------------------------------------------------

int TwoDSlidingFitResult::GetLayer(const float rL) const
{
    return std::floor(rL / LArGeometryHelper::GetLArPseudoLayerCalculator()->GetZPitch());
}

//------------------------------------------------------------------------------------------------------------------------------------------

float TwoDSlidingFitResult::GetL(const int layer) const
{
    return static_cast<float>(layer) * LArGeometryHelper::GetLArPseudoLayerCalculator()->GetZPitch();
}

//------------------------------------------------------------------------------------------------------------------------------------------

void TwoDSlidingFitResult::GetLocalFitPosition(const float x, float &rL, float &rT, int &layer) const
{
    CartesianVector position(0.f, 0.f, 0.f);
    this->GetGlobalFitPosition(x, true, position);
    this->GetLocalPosition(position, rL, rT);
    layer = this->GetLayer(rL);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void TwoDSlidingFitResult::GetGlobalFitPosition(const float rL, CartesianVector &position) const
{
    float firstWeight(0.f), secondWeight(0.f);
    LayerFitResultMap::const_iterator firstLayerIter, secondLayerIter;
    this->GetSurroundingLayerIterators(rL, firstLayerIter, secondLayerIter, firstWeight, secondWeight);
    this->GetGlobalFitInterpolatedPosition(firstLayerIter, secondLayerIter, firstWeight, secondWeight, position);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void TwoDSlidingFitResult::GetGlobalFitDirection(const float rL, CartesianVector &direction) const
{
    float firstWeight(0.f), secondWeight(0.f);
    LayerFitResultMap::const_iterator firstLayerIter, secondLayerIter;
    this->GetSurroundingLayerIterators(rL, firstLayerIter, secondLayerIter, firstWeight, secondWeight);
    this->GetGlobalFitInterpolatedDirection(firstLayerIter, secondLayerIter, firstWeight, secondWeight, direction);
}

//------------------------------------------------------------------------------------------------------------------------------------------

float TwoDSlidingFitResult::GetRms(const float rL) const
{
    float firstWeight(0.f), secondWeight(0.f);
    LayerFitResultMap::const_iterator firstLayerIter, secondLayerIter;
    this->GetSurroundingLayerIterators(rL, firstLayerIter, secondLayerIter, firstWeight, secondWeight);

    const float firstRms(firstLayerIter->second.GetRms());

    if (firstLayerIter == secondLayerIter)
        return firstRms;

    const float secondRms(secondLayerIter->second.GetRms());

    if (firstWeight + secondWeight < std::numeric_limits<float>::epsilon())
        throw StatusCodeException(STATUS_CODE_FAILURE);

    return (firstRms * firstWeight + secondRms * secondWeight) / (firstWeight + secondWeight);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void TwoDSlidingFitResult::GetGlobalFitPosition(const float p, const bool useX, CartesianVector &position) const
{
    float firstWeight(0.f), secondWeight(0.f);
    LayerFitResultMap::const_iterator firstLayerIter, secondLayerIter;
    this->GetSurroundingLayerIterators(p, useX, firstLayerIter, secondLayerIter, firstWeight, secondWeight);
    this->GetGlobalFitInterpolatedPosition(firstLayerIter, secondLayerIter, firstWeight, secondWeight, position);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void TwoDSlidingFitResult::GetGlobalFitDirection(const float p, const bool useX, CartesianVector &direction) const
{
    float firstWeight(0.f), secondWeight(0.f);
    LayerFitResultMap::const_iterator firstLayerIter, secondLayerIter;
    this->GetSurroundingLayerIterators(p, useX, firstLayerIter, secondLayerIter, firstWeight, secondWeight);
    this->GetGlobalFitInterpolatedDirection(firstLayerIter, secondLayerIter, firstWeight, secondWeight, direction);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void TwoDSlidingFitResult::GetGlobalFitProjection(const CartesianVector &inputPosition, CartesianVector &projectedPosition) const
{
    float rL(0.f), rT(0.f);
    this->GetLocalPosition(inputPosition, rL, rT); 
    this->GetGlobalFitPosition(rL, projectedPosition);
}

//------------------------------------------------------------------------------------------------------------------------------------------

CartesianVector TwoDSlidingFitResult::GetGlobalMinLayerPosition() const
{
    if (m_layerFitResultMap.empty())
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    LayerFitResultMap::const_iterator iter = m_layerFitResultMap.begin();
    CartesianVector position(0.f, 0.f, 0.f);
    this->GetGlobalPosition(iter->second.GetL(), iter->second.GetFitT(), position);
    return position;
}

//------------------------------------------------------------------------------------------------------------------------------------------

CartesianVector TwoDSlidingFitResult::GetGlobalMaxLayerPosition() const
{
    if (m_layerFitResultMap.empty())
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    LayerFitResultMap::const_reverse_iterator iter = m_layerFitResultMap.rbegin();
    CartesianVector position(0.f, 0.f, 0.f);
    this->GetGlobalPosition(iter->second.GetL(), iter->second.GetFitT(), position);
    return position;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void TwoDSlidingFitResult::GetGlobalFitInterpolatedPosition(const LayerFitResultMap::const_iterator &firstLayerIter,
    const LayerFitResultMap::const_iterator &secondLayerIter, const float &firstWeight, const float &secondWeight,
    CartesianVector &position) const
{
    CartesianVector firstLayerPosition(0.f, 0.f, 0.f);
    this->GetGlobalPosition(firstLayerIter->second.GetL(), firstLayerIter->second.GetFitT(), firstLayerPosition);

    if (firstLayerIter == secondLayerIter)
    {
        position = firstLayerPosition;
        return;
    }

    CartesianVector secondLayerPosition(0.f, 0.f, 0.f);
    this->GetGlobalPosition(secondLayerIter->second.GetL(), secondLayerIter->second.GetFitT(), secondLayerPosition);

    if (firstWeight + secondWeight < std::numeric_limits<float>::epsilon())
        throw StatusCodeException(STATUS_CODE_FAILURE);

    position = (firstLayerPosition * firstWeight + secondLayerPosition * secondWeight) * (1.f / (firstWeight + secondWeight));
}

//------------------------------------------------------------------------------------------------------------------------------------------

void TwoDSlidingFitResult::GetGlobalFitInterpolatedDirection(const LayerFitResultMap::const_iterator &firstLayerIter,
    const LayerFitResultMap::const_iterator &secondLayerIter, const float &firstWeight, const float &secondWeight,
    CartesianVector &direction) const
{
    // Get surrounding layer directions
    const float firstLayerGrad(firstLayerIter->second.GetGradient());
    const float firstLayerPL(1.f / std::sqrt(1.f + firstLayerGrad * firstLayerGrad));
    const float firstLayerPT(firstLayerGrad / std::sqrt(1.f + firstLayerGrad * firstLayerGrad));

    CartesianVector firstLayerStep(0.f, 0.f, 0.f);
    this->GetGlobalPosition(firstLayerPL, firstLayerPT, firstLayerStep);
    const CartesianVector firstLayerDirection((firstLayerStep - m_axisIntercept).GetUnitVector());

    if (firstLayerIter == secondLayerIter)
    {
        direction = firstLayerDirection;
        return;
    }

    const float secondLayerGrad(secondLayerIter->second.GetGradient());
    const float secondLayerPL(1.f / std::sqrt(1.f + secondLayerGrad * secondLayerGrad));
    const float secondLayerPT(secondLayerGrad / std::sqrt(1.f + secondLayerGrad * secondLayerGrad));

    CartesianVector secondLayerStep(0.f, 0.f, 0.f);
    this->GetGlobalPosition(secondLayerPL, secondLayerPT, secondLayerStep);
    const CartesianVector secondLayerDirection((secondLayerStep - m_axisIntercept).GetUnitVector());

    // Linear interpolation
    if (firstWeight + secondWeight < std::numeric_limits<float>::epsilon())
        throw StatusCodeException(STATUS_CODE_FAILURE);

    direction = (firstLayerDirection * firstWeight + secondLayerDirection * secondWeight).GetUnitVector();
}

//------------------------------------------------------------------------------------------------------------------------------------------

void TwoDSlidingFitResult::GetSurroundingLayerIterators(const float rL, LayerFitResultMap::const_iterator &firstLayerIter,
    LayerFitResultMap::const_iterator &secondLayerIter, float &firstWeight, float &secondWeight) const
{
    if (m_layerFitResultMap.empty())
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED); 

    const int minLayer(m_layerFitResultMap.begin()->first), maxLayer(m_layerFitResultMap.rbegin()->first);
    const int startLayer(this->GetLayer(rL));

    if ((startLayer < minLayer) || (startLayer > maxLayer))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);  

    if (startLayer == minLayer)
    {
        firstWeight = 0.5f;
        secondWeight = 0.5f;
        firstLayerIter = m_layerFitResultMap.begin();
        secondLayerIter = m_layerFitResultMap.begin();
        return;
    }

    if (startLayer == maxLayer)
    {
        firstWeight = 0.5f;
        secondWeight = 0.5f;
        firstLayerIter = --(m_layerFitResultMap.end());
        secondLayerIter = --(m_layerFitResultMap.end());
        return;
    }

    // First layer iterator
    firstLayerIter = m_layerFitResultMap.end();

    for (int iLayer = startLayer; iLayer >= minLayer; --iLayer)
    {
        firstLayerIter = m_layerFitResultMap.find(iLayer);

        if (m_layerFitResultMap.end() != firstLayerIter)
            break;
    }

    if (m_layerFitResultMap.end() == firstLayerIter)
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);

    // Second layer iterator
    secondLayerIter = m_layerFitResultMap.end();

    for (int iLayer = startLayer+1; iLayer <= maxLayer; ++iLayer)
    {
        secondLayerIter = m_layerFitResultMap.find(iLayer);

        if (m_layerFitResultMap.end() != secondLayerIter)
            break;
    }

    if (m_layerFitResultMap.end() == secondLayerIter)
        throw StatusCodeException(STATUS_CODE_NOT_FOUND); 

    // Weights
    const float deltaL(rL - firstLayerIter->second.GetL());
    const float deltaLLayers(secondLayerIter->second.GetL() - firstLayerIter->second.GetL());    

    if (std::fabs(deltaLLayers) < std::numeric_limits<float>::epsilon())
        throw StatusCodeException(STATUS_CODE_FAILURE);

    firstWeight = 1.f - deltaL / deltaLLayers;
    secondWeight = deltaL / deltaLLayers;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void TwoDSlidingFitResult::GetSurroundingLayerIterators(const float p, const bool useX, LayerFitResultMap::const_iterator &firstLayerIter,
    LayerFitResultMap::const_iterator &secondLayerIter, float &firstWeight, float &secondWeight) const
{
    if (m_layerFitResultMap.empty())
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    if (useX && (std::fabs(m_axisDirection.GetX()) < std::numeric_limits<float>::epsilon()))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    if (!useX && (std::fabs(m_axisDirection.GetZ()) < std::numeric_limits<float>::epsilon()))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    // Find start layer
    const float firstL(useX ? (p - m_axisIntercept.GetX()) / m_axisDirection.GetX() : (p - m_axisIntercept.GetZ()) / m_axisDirection.GetZ());
    const int minLayer(m_layerFitResultMap.begin()->first), maxLayer(m_layerFitResultMap.rbegin()->first);
    const int startLayer(this->GetLayer(firstL));

    if ((startLayer < minLayer) || (startLayer > maxLayer))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    if (startLayer == minLayer)
    {
        firstWeight = 0.5f;
        secondWeight = 0.5f;
        firstLayerIter = m_layerFitResultMap.begin();
        secondLayerIter = m_layerFitResultMap.begin();
        return;
    }

    if (startLayer == maxLayer)
    {
        firstWeight = 0.5f;
        secondWeight = 0.5f;
        firstLayerIter = --(m_layerFitResultMap.end());
        secondLayerIter = --(m_layerFitResultMap.end());
        return;
    }

    // First layer iterator
    firstLayerIter = m_layerFitResultMap.end();
    CartesianVector firstLayerPosition(0.f, 0.f, 0.f);

    for (int iLayer = startLayer; iLayer < maxLayer; ++iLayer)
    {
        firstLayerIter = m_layerFitResultMap.find(iLayer);

        if (m_layerFitResultMap.end() != firstLayerIter)
            break;
    }

    if (m_layerFitResultMap.end() == firstLayerIter)
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);
    
    this->GetGlobalPosition(firstLayerIter->second.GetL(), firstLayerIter->second.GetFitT(), firstLayerPosition);

    const bool firstIsAhead(useX ? (firstLayerPosition.GetX() > p) : (firstLayerPosition.GetZ() > p));
    const bool increasesWithLayers = (useX ? (m_axisDirection.GetX() > 0.f) : (m_axisDirection.GetZ() > 0.f));
    const int increment = ((firstIsAhead == increasesWithLayers) ? -1 : +1);

    // Second layer iterator
    secondLayerIter = m_layerFitResultMap.end();

    CartesianVector secondLayerPosition(0.f, 0.f, 0.f);

    for (int iLayer = firstLayerIter->first + increment; (iLayer >= minLayer) && (iLayer <= maxLayer); iLayer += increment)
    {
        LayerFitResultMap::const_iterator tempIter = m_layerFitResultMap.find(iLayer);

        if (m_layerFitResultMap.end() == tempIter)
            continue;

        secondLayerIter = tempIter;

        this->GetGlobalPosition(secondLayerIter->second.GetL(), secondLayerIter->second.GetFitT(), secondLayerPosition);
        const bool secondIsAhead(useX ? (secondLayerPosition.GetX() > p) : (secondLayerPosition.GetZ() > p));

        if (firstIsAhead != secondIsAhead)
            break;
    }

    if (m_layerFitResultMap.end() == secondLayerIter)
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);  

    // Weights
    const float deltaP = useX ? (p - firstLayerPosition.GetX()) : (p - firstLayerPosition.GetZ());
    const float deltaPLayers = useX ? (secondLayerPosition.GetX() - firstLayerPosition.GetX()) : (secondLayerPosition.GetZ() - firstLayerPosition.GetZ());

    if (std::fabs(deltaPLayers) < std::numeric_limits<float>::epsilon())
        throw StatusCodeException(STATUS_CODE_FAILURE);

    firstWeight = 1.f - deltaP / deltaPLayers;
    secondWeight = deltaP / deltaPLayers;
}
 
//------------------------------------------------------------------------------------------------------------------------------------------

void TwoDSlidingFitResult::GetSurroundingLayerIterators(const CartesianVector &position, LayerFitResultMap::const_iterator &firstLayerIter,
    LayerFitResultMap::const_iterator &secondLayerIter, float &firstWeight, float &secondWeight) const
{
    float rL(0.f), rT(0.f);
    this->GetLocalPosition(position, rL, rT);
    this->GetSurroundingLayerIterators(rL, firstLayerIter, secondLayerIter, firstWeight, secondWeight);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

TwoDSlidingFitResult::LayerFitResult::LayerFitResult(const double l, const double fitT, const double gradient, const double rms) :
    m_l(l),
    m_fitT(fitT),
    m_gradient(gradient),
    m_rms(rms)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

TwoDSlidingFitResult::LayerFitContribution::LayerFitContribution() :
    m_sumT(0.),
    m_sumL(0.),
    m_sumTT(0.),
    m_sumLT(0.),
    m_sumLL(0.),
    m_nPoints(0)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

void TwoDSlidingFitResult::LayerFitContribution::AddPoint(const float l, const float t)
{
    const double T = static_cast<double>(t);
    const double L = static_cast<double>(l);

    m_sumT += T;
    m_sumL += L;
    m_sumTT += T * T;
    m_sumLT += L * T;
    m_sumLL += L * L;
    ++m_nPoints;
}

} // namespace lar
