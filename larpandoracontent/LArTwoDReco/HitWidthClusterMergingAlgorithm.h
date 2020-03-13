/**
 *  @file   HitWidthClusterMergingAlgorithm.h
 *
 *  @brief  Header file for the hit width cluster merging algorithm class.
 *
 *  $Log: $
 */

#ifndef LAR_HIT_WIDTH_CLUSTER_MERGING_ALGORITHM_H
#define LAR_HIT_WIDTH_CLUSTER_MERGING_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

#include "larpandoracontent/LArTwoDReco/LArClusterAssociation/ClusterAssociationAlgorithm.h"
#include "larpandoracontent/LArHelpers/LArHitWidthHelper.h"

namespace lar_content
{


/**
 *  @brief HitWidthClusterMergingAlgorithm class
 */
  class HitWidthClusterMergingAlgorithm : public ClusterAssociationAlgorithm
{

public:

    /**
     *  @brief  Default constructor
     */
    HitWidthClusterMergingAlgorithm();


private:
  
    struct SortByHigherXExtrema{

        SortByHigherXExtrema(const float maxConstituentHitWidth) : m_maxConstituentHitWidth(maxConstituentHitWidth) {}

        bool operator() (const pandora::Cluster *const pLhs, const pandora::Cluster *const pRhs) 
        {

	    LArHitWidthHelper::ConstituentHitVector lhsConstituentHitVector(LArHitWidthHelper::GetConstituentHits(pLhs, m_maxConstituentHitWidth));
	    LArHitWidthHelper::ConstituentHitVector rhsConstituentHitVector(LArHitWidthHelper::GetConstituentHits(pRhs, m_maxConstituentHitWidth));

	    pandora::CartesianVector lhsHigherXExtrema(LArHitWidthHelper::GetExtremalCoordinatesHigherX(lhsConstituentHitVector));
	    pandora::CartesianVector rhsHigherXExtrema(LArHitWidthHelper::GetExtremalCoordinatesHigherX(rhsConstituentHitVector));

	    float lhsMaxX(lhsHigherXExtrema.GetX()), rhsMaxX(rhsHigherXExtrema.GetX());
    
	    return (lhsMaxX < rhsMaxX);
	}

	const float m_maxConstituentHitWidth;
    };
    

  
  void GetListOfCleanClusters(const pandora::ClusterList *const pClusterList, pandora::ClusterVector &clusterVector) const;

  void PopulateClusterAssociationMap(const pandora::ClusterVector &clusterVector, ClusterAssociationMap &clusterAssociationMap) const;

  void CleanupClusterAssociations(const pandora::ClusterVector &clusterVector, ClusterAssociationMap &clusterAssociationMap) const;

  bool AreClustersAssociated(const LArHitWidthHelper::ClusterParameters &currentFitParameters, const LArHitWidthHelper::ClusterParameters &testFitParameters) const;

  bool IsExtremalCluster(const bool isForward, const pandora::Cluster *const pCurrentCluster,  const pandora::Cluster *const pTestCluster) const;

  pandora::CartesianVector GetClusterDirection(const LArHitWidthHelper::ClusterParameters &clusterFitParameters) const;

  pandora::CartesianVector GetClusterZIntercept(const LArHitWidthHelper::ClusterParameters &clusterFitParameters) const;

  void GetWeightedGradient(const LArHitWidthHelper::ClusterParameters &clusterFitParameters, bool isTransverse, pandora::CartesianVector &direction, pandora::CartesianVector &intercept, float &chiSquared) const;


  std::string m_clusterListName;
  float m_minClusterWeight;
  float m_maxXMergeDistance; //Distance either side of point
  float m_maxZMergeDistance; //Distance either side of point
  float m_maxMergeCosOpeningAngle; 

  float m_maxConstituentHitWidth;

  LArHitWidthHelper::ClusterToParametersMap m_clusterToFitParametersMap;

  pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

};

} //namespace lar_content

#endif //LAR_HIT_WIDTH_CLUSTER_MERGING_ALGORITHM_H



