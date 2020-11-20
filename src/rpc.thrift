# Copyright: (C) 2020 iCub Fondazione Istituto Italiano di Tecnologia (IIT) 
# All Rights Reserved.
#
# rpc.thrift

/**
* rpc_IDL
*
* IDL Interface
*/


service rpc_IDL
{

   /**
   * Segment out the object point cloud.
   * @return true/false on success/failure.
   */
   bool segment();

   /**
   * Fit the object point cloud with the provided model.
   * @return true/false on success/failure.
   */
   bool fit();
    
   /**
   * Get number of points whose fit score is equal or below 1.
   * @return inliers.
   */
   double get_inliers();

   /**
   * Get score computed as difference from superquadric size.
   * @return score.
   */
   double get_score();

   /**
   * Run the pipeline.
   * @return fit score.
   */
   double go();
}
