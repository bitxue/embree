// ======================================================================== //
// Copyright 2009-2014 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#include "subdivpatch1cached_intersector1.h"
#include "bicubic_bezier_patch.h"

namespace embree
{
  
  void SubdivPatch1CachedIntersector1::intersect_subdiv_patch(const Precalculations& pre,
                                                              Ray& ray,
                                                              const Primitive& subdiv_patch,
                                                              const void* geom)
  {
    PING;
  }

  bool SubdivPatch1CachedIntersector1::occluded_subdiv_patch(const Precalculations& pre,
                                                             Ray& ray,
                                                             const Primitive& subdiv_patch,
                                                             const void* geom)
  {
    PING;
  }

};