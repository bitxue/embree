// ======================================================================== //
// Copyright 2009-2013 Intel Corporation                                    //
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

#pragma once

#include "builders/primrefalloc.h"
#include "builders/primrefblock.h"
#include "builders/primrefgen.h"
#include "builders/splitter.h"
#include "builders/splitter_parallel.h"
#include "geometry/primitive.h"

namespace embree
{
  /* BVH builder. The builder is multi-threaded and implements 3
   * different build strategies: 1) Small tasks are finished in a
   * single thread (BuildTask) 2) Medium sized tasks are split into
   * two tasks using a single thread (SplitTask) and 3) Large tasks are
   * split using multiple threads on one processor. */

  template<typename Heuristic>
    class BVH4Builder : public Builder
  {
    ALIGNED_CLASS;
  public:

    /*! Type shortcuts */
    //typedef typename BVH4::Node    Node;
    typedef typename BVH4::NodeRef NodeRef;

    /*! Split type of the split heuristic. */
    typedef typename Heuristic::Split Split;
    typedef typename Heuristic::PrimInfo PrimInfo;
    
    /*! Normal splitters based on heuristic */
    typedef PrimRefGen<Heuristic> PrimRefGenNormal;
    typedef MultiThreadedSplitter<Heuristic> MultiThreadedSplitterNormal;
    typedef Splitter<Heuristic> SplitterNormal;
    
  public:

    /*! builder entry point */
    void build(size_t threadIndex, size_t threadCount);

    /*! Constructor. */
    BVH4Builder (BVH4* bvh, BuildSource* source, void* geometry, const size_t minLeafSize = 1, const size_t maxLeafSize = inf);

    /*! build job */
    TASK_COMPLETE_FUNCTION_(BVH4Builder,buildFunction);
    void buildFunction(size_t threadIndex, size_t threadCount, TaskScheduler::Event* event);

    /*! finishes the build */
    TASK_COMPLETE_FUNCTION_(BVH4Builder,finish);
    void finish(size_t threadIndex, size_t threadCount, TaskScheduler::Event* event);

    /*! creates a leaf node */
    NodeRef createLeaf(size_t threadIndex, atomic_set<PrimRefBlock>& prims, const PrimInfo& pinfo);

    /*! creates a large leaf by adding additional internal nodes */
    NodeRef createLargeLeaf(size_t threadIndex, atomic_set<PrimRefBlock>& prims, const PrimInfo& pinfo, size_t depth);

    /*! Selects between full build and single-threaded split strategy. */
    void recurse(size_t threadIndex, size_t threadCount, TaskScheduler::Event* event, 
                 NodeRef& node, size_t depth, atomic_set<PrimRefBlock>& prims, const PrimInfo& pinfo, const Split& split);

    /***********************************************************************************************************************
     *                                      Single Threaded Build Task
     **********************************************************************************************************************/
    
    /*! Single-threaded task that builds a complete BVH4. */
    class BuildTask {
      ALIGNED_CLASS
    public:

      /*! Default task construction. */
      BuildTask(size_t threadIndex, size_t threadCount, TaskScheduler::Event* event, 
                BVH4Builder* parent, NodeRef& node, size_t depth, 
                atomic_set<PrimRefBlock>& prims, const PrimInfo& pinfo, const Split& split);

      /*! Task entry function. */
      TASK_COMPLETE_FUNCTION_(BuildTask,run);
      void run(size_t threadIndex, size_t threadCount, TaskScheduler::Event* event);

      /*! Recursively finishes the BVH4 construction. */
      NodeRef recurse(size_t depth, atomic_set<PrimRefBlock>& prims, const PrimInfo& pinfo, const Split& split);
      
    private:
      size_t threadIndex;
      TaskScheduler::Task task;
      
      BVH4Builder*                     parent;   //!< Pointer to parent task.
      NodeRef&                         dst;      //!< Reference to output the node.
      size_t                           depth;    //!< Recursion depth of the root of this subtree.
      atomic_set<PrimRefBlock>         prims;    //!< The list of primitives.
      PrimInfo                         pinfo;    //!< Bounding info of primitives.
      Split                            split;    //!< The best split for the primitives.
    };

    /***********************************************************************************************************************
     *                                      Single Threaded Split Task
     **********************************************************************************************************************/

    /*! Single-threaded task that builds a single node and creates subtasks for the children. */
    class SplitTask {
      ALIGNED_CLASS
    public:

      /*! Default task construction. */
      SplitTask(size_t threadIndex, size_t threadCount, TaskScheduler::Event* event, 
                BVH4Builder* parent, NodeRef& node, size_t depth, 
                atomic_set<PrimRefBlock>& prims, const PrimInfo& pinfo, const Split& split);

      /*! Task entry function. */
      TASK_COMPLETE_FUNCTION_(SplitTask,recurse);
      void recurse(size_t threadIndex, size_t threadCount, TaskScheduler::Event* event);
      
    private:
      TaskScheduler::Task task;
      BVH4Builder*    parent;         //!< Pointer to parent task.
      NodeRef&    dst;            //!< Reference to output the node.
      size_t          depth;          //!< Recursion depth of this node.
      atomic_set<PrimRefBlock> prims; //!< The list of primitives.
      PrimInfo        pinfo;          //!< Bounding info of primitives.
      Split           split;          //!< The best split for the primitives.
    };

  /***********************************************************************************************************************
   *                                           Parallel Split Task
   **********************************************************************************************************************/

    /*! Multi-threaded split task that builds a single node and creates subtasks for the children. */
    class ParallelSplitTask {
      ALIGNED_CLASS
    public:

      /*! Default task construction. */
      ParallelSplitTask (size_t threadIndex, size_t threadCount, TaskScheduler::Event* event, 
                         BVH4Builder* parent, NodeRef& node, size_t depth, 
                         atomic_set<PrimRefBlock>& prims, const PrimInfo& pinfo, const Split& split);

      /*! Called after the parallel binning to creates the node. */
      TASK_COMPLETE_FUNCTION_(ParallelSplitTask,loop);
      void loop(size_t threadIndex, size_t threadCount, TaskScheduler::Event* event);
      
    private:
      BVH4Builder*                parent;                 //!< Pointer to parent task.
      NodeRef&                    dst;                    //!< Reference to output the node ID.
      size_t                      depth;                  //!< Recursion depth of this node.
      atomic_set<PrimRefBlock>    cprims[BVH4::N];              //!< Primitive lists for the children.
      PrimInfo                    cinfo [BVH4::N];              //!< Bounding info for the children.
      Split                       csplit[BVH4::N];              //!< Best next split for the children.
      size_t                      numChildren;            //!< Current number of children.
      ssize_t                     bestChild;              //!< Best child selected for splitting.
      MultiThreadedSplitterNormal splitter;               //!< Parallel splitter
    };

  private:
    BuildSource* source;      //!< build source interface
    void* geometry;           //!< input geometry
    
  public:
    const PrimitiveType& primTy;          //!< triangle type stored in BVH4
    size_t minLeafSize;                 //!< minimal size of a leaf
    size_t maxLeafSize;                 //!< maximal size of a leaf
    PrimRefAlloc alloc;                 //!< Allocator for primitive blocks
    PrimRefGenNormal initStage;               //!< job to generate build primitives
    TaskScheduler::QUEUE taskQueue;     //!< Task queue to use

  public:
    BVH4* bvh;                      //!< Output BVH4
  };
}
