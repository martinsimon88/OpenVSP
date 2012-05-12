//
// This file is released under the terms of the NASA Open Source Agreement (NOSA)
// version 1.3 as detailed in the LICENSE file which accompanies this software.
//

//******************************************************************************
//
//   Map Source and KD-Tree
//
//   Rob McDonald 4/4/12
//
//******************************************************************************

#ifndef MAPSOURCE_H
#define MAPSOURCE_H

#include "vec3d.h"
#include "GridDensity.h"

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#include "nanoflann.hpp"

using namespace nanoflann;

struct MapSource;
struct MSCloud;

typedef KDTreeSingleIndexAdaptor< L2_Simple_Adaptor< double, MSCloud > ,MSCloud, 3 > MSTree;

typedef vector< pair< size_t, double > > MSTreeResults;

struct MapSource
{
	MapSource( vec3d pt, double *strptr )
	{
		m_pt = pt;
		m_strptr = strptr;
	};

	vec3d m_pt;
	double *m_strptr;
};

// The data source fed into the KD-tree library must adhere to an interface.  The following
// struct implements that interface for the edge source kd-tree.

struct MSCloud
{
	// Underlying storage a vector.
	vector< MapSource > sources;

	// Must return the number of data points
	inline size_t kdtree_get_point_count() const { return sources.size(); }

	// Returns the distance between the vector "p1[0:size-1]" and the data point with index "idx_p2" stored in the class:
	inline double kdtree_distance(const double *p1, const size_t idx_p2, size_t size) const
	{
		const double d0 = p1[0] - sources[idx_p2].m_pt.x();
		const double d1 = p1[1] - sources[idx_p2].m_pt.y();
		const double d2 = p1[2] - sources[idx_p2].m_pt.z();
		return d0*d0 + d1*d1 + d2*d2;
	}

	// Returns the dim'th component of the idx'th point in the class:
	// Since this is inlined and the "dim" argument is typically an immediate value, the
	//  "if/else's" are actually solved at compile time.
	inline double kdtree_get_pt(const size_t idx, int dim) const
	{
		if (dim==0) return sources[idx].m_pt.x();
		else if (dim==1) return sources[idx].m_pt.y();
		else return sources[idx].m_pt.z();
	}

	// Optional bounding-box computation: return false to default to a standard bbox computation loop.
	//   Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
	//   Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
	template <class BBOX>
	bool kdtree_get_bbox(BBOX &bb) const { return false; }

	void prune_edge_sources( MSTree &es_tree, GridDensity* grid_den );
	void free_strengths();

};



#endif