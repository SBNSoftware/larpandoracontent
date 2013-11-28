/**
 *  @file   LArContent/include/LArHelpers/LArClusterHelper.h
 * 
 *  @brief  Header file for the cluster helper class.
 * 
 *  $Log: $
 */
#ifndef LAR_CLUSTER_HELPER_H
#define LAR_CLUSTER_HELPER_H 1

#include "LArObjects/LArTwoDSlidingFitResult.h"

#include "Objects/Cluster.h"

namespace lar
{

/**
 *  @brief  ShowerEdge enum
 */
enum ShowerEdge
{
    POSITIVE_SHOWER_EDGE,
    NEGATIVE_SHOWER_EDGE
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  LArClusterHelper class
 */
class LArClusterHelper
{
public:
    typedef lar::TwoDSlidingFitResult TwoDSlidingFitResult;

    /**
     *  @brief  Perform two dimensional sliding fit, using a three dimensional fit to the cluster to define primary axis
     * 
     *  @param  pCluster address of the cluster
     *  @param  layerFitHalfWindow the layer fit half window
     *  @param  twoDSlidingFitResult to receive the fit result
     */
    static void LArTwoDSlidingFit(const pandora::Cluster *const pCluster, const unsigned int layerFitHalfWindow, TwoDSlidingFitResult &twoDSlidingFitResult);

    /**
     *  @brief  Perform two dimensional sliding fit, using z axis as primary axis, fitting x coordinates
     * 
     *  @param  pCluster address of the cluster
     *  @param  layerFitHalfWindow the layer fit half window
     *  @param  twoDSlidingFitResult to receive the fit result
     */
    static void LArTwoDSlidingXZFit(const pandora::Cluster *const pCluster, const unsigned int layerFitHalfWindow, TwoDSlidingFitResult &twoDSlidingFitResult);

    /**
     *  @brief  Perform two dimensional sliding fit, using the specified primary axis
     * 
     *  @param  pCluster address of the cluster
     *  @param  layerFitHalfWindow the layer fit half window
     *  @param  axisIntercept the axis intercept position
     *  @param  axisDirection the axis direction vector
     *  @param  twoDSlidingFitResult to receive the fit result
     */
    static void LArTwoDSlidingFit(const pandora::Cluster *const pCluster, const unsigned int layerFitHalfWindow, const pandora::CartesianVector &axisIntercept,
        const pandora::CartesianVector &axisDirection, TwoDSlidingFitResult &twoDSlidingFitResult);

    /**
     *  @brief  Perform two dimensional sliding fit to shower edge, using specified primary axis
     * 
     *  @param  pCluster address of the cluster
     *  @param  layerFitHalfWindow the layer fit half window
     *  @param  axisIntercept the axis intercept position
     *  @param  axisDirection the axis direction vector
     *  @param  showerEdge the shower edge
     *  @param  twoDSlidingFitResult to receive the fit result
     */
    static void LArTwoDShowerEdgeFit(const pandora::Cluster *const pCluster, const unsigned int layerFitHalfWindow, const pandora::CartesianVector &axisIntercept,
        const pandora::CartesianVector &axisDirection, const ShowerEdge showerEdge, TwoDSlidingFitResult &twoDSlidingFitResult);

    /**
     *  @brief  Whether fit results are multivalued in x
     * 
     *  @return boolean
     */
    static bool IsMultivaluedInX(const TwoDSlidingFitResult &twoDSlidingFitResult);

    /**
     *  @brief  Get the sliding fit width
     * 
     *  @return the sliding fit width
     */
    static float GetSlidingFitWidth(const TwoDSlidingFitResult &twoDSlidingFitResult);

    /**
     *  @brief  Find the largest scatter in the cluster, if above a threshold value
     * 
     *  @param  largestScatterPosition to receive the position of the largest scatter
     * 
     *  @return STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND
     */
    static pandora::StatusCode FindLargestScatter(const TwoDSlidingFitResult &twoDSlidingFitResult, pandora::CartesianVector &largestScatterPosition);

    /**
     *  @brief  Measure width of cluster using multiple straight line fits
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return float
     */
    static float LArTrackWidth(const pandora::Cluster *const pCluster);

    /**
     *  @brief  Get length squared of cluster
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return the length squared
     */
    static float GetLengthSquared(const pandora::Cluster *const pCluster);

    /**
     *  @brief  Get length of cluster
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return the length
     */
    static float GetLength(const pandora::Cluster *const pCluster);

    /**
     *  @brief  Get energy of cluster, based on length
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return the energy
     */
    static float GetEnergyFromLength(const pandora::Cluster *const pCluster);

    /**
     *  @brief  Get number of layers spanned by cluster (1+Last-First)
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return the layer span
     */

    static unsigned int GetLayerSpan(const pandora::Cluster *const pCluster);

    /**
     *  @brief  Fraction of occupied layers in cluster
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return float
     */
    static float GetLayerOccupancy(const pandora::Cluster *const pCluster);

    /**
     *  @brief  Fraction of occupied layers in a pair of clusters
     * 
     *  @param  pCluster1 address of the first cluster
     *  @param  pCluster2 address of the second cluster
     * 
     *  @return float
     */
    static float GetLayerOccupancy(const pandora::Cluster *const pCluster1, const pandora::Cluster *const pCluster2);

    /**
     *  @brief  Get closest distance between the layer centroids of a pair of clusters
     * 
     *  @param  pCluster1 address of the first cluster
     *  @param  pCluster2 address of the second cluster
     * 
     *  @return the closest distance
     */
    static float GetClosestDistance(const pandora::Cluster *const pCluster1, const pandora::Cluster *const pCluster2);

    /**
     *  @brief  Get closest distance between a specified position vector and the layer centroids of a specified cluster
     * 
     *  @param  position the position vector
     *  @param  pCluster address of the cluster
     * 
     *  @return the closest distance
     */
    static float GetClosestDistance(const pandora::CartesianVector &position, const pandora::Cluster *const pCluster);

    /**
     *  @brief  Sort clusters by inner layer (then use SortByNOccupiedLayers method in event of a tie)
     * 
     *  @param  pLhs address of first cluster
     *  @param  pRhs address of second cluster
     */
    static bool SortByInnerLayer(const pandora::Cluster *const pLhs, const pandora::Cluster *const pRhs);

    /**
     *  @brief  Sort clusters by number of occupied layers, and by inner layer, then energy in event of a tie
     * 
     *  @param  pLhs address of first cluster
     *  @param  pRhs address of second cluster
     */
    static bool SortByNOccupiedLayers(const pandora::Cluster *const pLhs, const pandora::Cluster *const pRhs);

    /**
     *  @brief  Sort clusters by number of hits and by layer span, then energy in event of a tie
     * 
     *  @param  pLhs address of first cluster
     *  @param  pRhs address of second cluster
     */
    static bool SortByNHits(const pandora::Cluster *const pLhs, const pandora::Cluster *const pRhs);

    /**
     *  @brief  Read the vertex helper settings
     * 
     *  @param  xmlHandle the relevant xml handle
     */
    static pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

private:
    /**
     *  @brief  Perform two dimensional sliding fit, using the information stored in the sliding fit result object
     * 
     *  @param  twoDSlidingFitResult to receive the fit result
     */
    static void StoreSlidingFitResults(TwoDSlidingFitResult &twoDSlidingFitResult);

    static unsigned int             m_layerFitHalfWindow;           ///< The layer fit half window for sliding 2d x-z fits
    static float                    m_trackFitMaxRms;               ///< Max RMS of track segment to be considered for kink splitting
    static float                    m_minCosScatteringAngle;        ///< Min kink angle at which to enable kink splitting
    static float                    m_multiValuedTanThetaCut;       ///< Tan theta cut for finding sliding fits multivalued in x
    static float                    m_multiValuedStepFractionCut;   ///< Step fraction cut for finding sliding fits multivalued in x
    static float                    m_trackResidualQuantile;        ///< Track residual quantile, used for calculating sliding track width
};

} // namespace lar

#endif // #ifndef LAR_CLUSTER_HELPER_H
