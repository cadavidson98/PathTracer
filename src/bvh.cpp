#include "bvh.h"

#include <algorithm>
#include <stack>

#include "collision.h"

using namespace std;

/**
 * Create a new bvh, that's it...
 */ 
bvh::bvh(vector<Triangle*> tris) {
    // start by making the bvh leaves
    triangles_ = tris;
    vector<triangle_info> leaves = boundTriangles();
    // now construct the bvh
    root = new node;
    root -> AABB_ = getExtent(leaves);
    
    Dimension scene_dim = getExtent(leaves);
    compressed_node rt;
    bvh_nodes_.push_back(rt);
    buildRecurse(0, leaves);
}

/**
 * Delete the entire bvh, starting from the leaves and working up
 */ 
void bvh::destruct(node * cur_node) {
    if(cur_node == nullptr) {
        return;
    }
    destruct(cur_node -> l_child_);
    destruct(cur_node -> r_child_);

    delete cur_node;
}

/**
 * Construct a leaf node containing 1 singular triangle
 * This is trivially done by calculating the extent of the triangle
 */ 
vector<triangle_info> bvh::boundTriangles() {
    size_t num_tri = triangles_.size();
    vector<triangle_info> tri_nodes(num_tri);
    for(size_t i = 0; i < num_tri; ++i) {
        tri_nodes[i].tri_offset_ = i;
        
        Dimension tri_bnd;
        pair<float, float> x_bnds = minmax({triangles_[i]->p1_.x, triangles_[i]->p2_.x, triangles_[i]->p3_.x});
        pair<float, float> y_bnds = minmax({triangles_[i]->p1_.y, triangles_[i]->p2_.y, triangles_[i]->p3_.y});
        pair<float, float> z_bnds = minmax({triangles_[i]->p1_.z, triangles_[i]->p2_.z, triangles_[i]->p3_.z});
        
        tri_nodes[i].AABB_.min_x = x_bnds.first;
        tri_nodes[i].AABB_.max_x = x_bnds.second;
        
        tri_nodes[i].AABB_.min_y = y_bnds.first;
        tri_nodes[i].AABB_.max_y = y_bnds.second;
        
        tri_nodes[i].AABB_.min_z = z_bnds.first;
        tri_nodes[i].AABB_.max_z = z_bnds.second;

        tri_nodes[i].centroid_ = Vec3(.5f * x_bnds.second + .5f * x_bnds.first, .5f * y_bnds.second + .5f * y_bnds.first, .5f * z_bnds.second+.5f * z_bnds.first);
    }
    return tri_nodes;
}

/**
 * Determine the smallest boundary that encapsulates all the
 * triangles
 */ 
Dimension bvh::getExtent(vector<triangle_info> tris) {
    Dimension extent;
    size_t num_tris = tris.size();
    for(size_t i = 0; i < num_tris; ++i) {
        extent.max_x = max(extent.max_x, tris[i].AABB_.max_x);
        extent.max_y = max(extent.max_y, tris[i].AABB_.max_y);
        extent.max_z = max(extent.max_z, tris[i].AABB_.max_z);

        extent.min_x = min(extent.min_x, tris[i].AABB_.min_x);
        extent.min_y = min(extent.min_y, tris[i].AABB_.min_y);
        extent.min_z = min(extent.min_z, tris[i].AABB_.min_z);
    }
    return extent;
}

/**
 * Determine the smallest boundary that encapsulates all the
 * nodes.
 */ 
Dimension bvh::getExtent(vector<node*> nodes) {
    Dimension extent;
    size_t num_nodes = nodes.size();
    for(size_t i = 0; i < num_nodes; ++i) {
        extent.max_x = max(extent.max_x, nodes[i]->AABB_.max_x);
        extent.max_y = max(extent.max_y, nodes[i]->AABB_.max_y);
        extent.max_z = max(extent.max_z, nodes[i]->AABB_.max_z);

        extent.min_x = min(extent.min_x, nodes[i]->AABB_.min_x);
        extent.min_y = min(extent.min_y, nodes[i]->AABB_.min_y);
        extent.min_z = min(extent.min_z, nodes[i]->AABB_.min_z);
    }
    return extent;
}

/**
 * Determine the smallest boundary that encapsulates all the
 * points
 */ 
Dimension bvh::getExtent(vector<Vec3> pts) {
    Dimension extent;
    size_t num_pts = pts.size();
    for(size_t i = 0; i < num_pts; ++i) {
        extent.max_x = std::max(extent.max_x, pts[i].x);
        extent.max_y = std::max(extent.max_y, pts[i].y);
        extent.max_z = std::max(extent.max_z, pts[i].z);

        extent.min_x = std::min(extent.min_x, pts[i].x);
        extent.min_y = std::min(extent.min_y, pts[i].y);
        extent.min_z = std::min(extent.min_z, pts[i].z);
    }
    return extent;
}

/**
 * Calculate the surace area of the axis aligned bounding box 
 */
float bvh::surfaceArea(Dimension AABB) {
    return 
    2 * ((AABB.max_x - AABB.min_x) * (AABB.max_y - AABB.min_y)) +
    2 * ((AABB.max_x - AABB.min_x) * (AABB.max_z - AABB.min_z)) +
    2 * ((AABB.max_z - AABB.min_z) * (AABB.max_y - AABB.min_y));
}

/**
 * Construct a bvh by using the Surface Area Heuristic to subdivide
 * the leaf nodes provided 
 */
void bvh::buildRecurse(int node_offset, vector<triangle_info> tris) {
    if(tris.size() == 2) {
        compressed_node l_child, r_child;
        l_child.AABB_ = tris[0].AABB_;
        l_child.tri_offsets_.push_back(tris[0].tri_offset_);

        r_child.AABB_ = tris[1].AABB_;
        r_child.tri_offsets_.push_back(tris[1].tri_offset_);

        bvh_nodes_.push_back(l_child);
        bvh_nodes_.push_back(r_child);

        bvh_nodes_[node_offset].AABB_ = getExtent(tris);
        bvh_nodes_[node_offset].l_child_ = node_offset+1;
        bvh_nodes_[node_offset].r_child_ = node_offset+2;
        return;
    }
    if(tris.size() == 1) {
        compressed_node l_child;
        l_child.AABB_ = tris[0].AABB_;
        l_child.tri_offsets_.push_back(tris[0].tri_offset_);

        bvh_nodes_.push_back(l_child);

        bvh_nodes_[node_offset].AABB_ = getExtent(tris);
        bvh_nodes_[node_offset].l_child_ = node_offset+1;
        return;
    }
    bvh_nodes_[node_offset].AABB_ = getExtent(tris);
    vector<triangle_info> bin_1(0), bin_2(0);
    if(!splitSAH(tris, bin_1, bin_2)) {
        bvh_nodes_[node_offset].l_child_ = 
        bvh_nodes_[node_offset].r_child_ = -1;
        bvh_nodes_[node_offset].AABB_ = getExtent(tris);
        for(size_t i = 0; i < tris.size(); ++i) {
            bvh_nodes_[node_offset].tri_offsets_.push_back(tris[i].tri_offset_);
        }
        //cout << "created a leaf with " << tris.size() << " triangles" << endl;
        return;
    }
    // create left child
    compressed_node new_node;
    bvh_nodes_.push_back(new_node);
    int l_offset = node_offset + 1;
    bvh_nodes_[node_offset].l_child_ = l_offset;
    buildRecurse(l_offset, bin_1);
    // create right child
    bvh_nodes_.push_back(new_node);
    int r_offset = bvh_nodes_.size() - 1;
    bvh_nodes_[node_offset].r_child_ = r_offset;
    buildRecurse(r_offset, bin_2);
}

struct Bucket {
    Dimension bounds_;
    vector<triangle_info> tris_;
};

bool bvh::splitSAH(vector<triangle_info> in_tris, vector<triangle_info> &bin_1,
                   vector<triangle_info> &bin_2) {
    // bound the node
    Dimension scene_bnds = getExtent(in_tris);
    vector<Vec3> centroid_pts;
    for(size_t i = 0; i < in_tris.size(); ++i) {
        centroid_pts.push_back(in_tris[i].centroid_);
    }
    float min_sah = INFINITY;
    Dimension centroid_bnds = getExtent(centroid_pts);
    float range_x = centroid_bnds[0].second - centroid_bnds[0].first;
    float range_y = centroid_bnds[1].second - centroid_bnds[1].first;
    float range_z = centroid_bnds[2].second - centroid_bnds[2].first;
    float ranges[3] = { range_x, range_y, range_z };
    int axis = 0;
    // parition along semi-major axis, but using centroids to mitigate
    // large triangle issues...
    for(int i = 1; i < 3; i++) {
        if(ranges[i] > ranges[axis]) {
            axis = i;
        }
    }
    if(ranges[axis] < .0001) {
        return false;
    }
    float parent_sa = surfaceArea(scene_bnds);
    
    // try binning the triangles
    int num_bins = 16;
    Bucket bins[16];
        
    for(size_t i = 0; i < in_tris.size(); ++i) {
        // find out what bin this triangle belongs in
        // this can be done by manually finding the threshhold value
        // or just normalizing its value
        float range = ranges[axis];
        float centroid_pos = (axis == 0) ? in_tris[i].centroid_.x : (axis == 1) ? in_tris[i].centroid_.y : in_tris[i].centroid_.z;
        float normalized_position = (centroid_pos - centroid_bnds[axis].first) / range;
        int bucket_index = min(int(num_bins * normalized_position), num_bins - 1);
        bins[bucket_index].tris_.push_back(in_tris[i]);
    }

    // get the size of each bin
    for(int i = 0; i < num_bins; ++i) {
            bins[i].bounds_ = getExtent(bins[i].tris_);
    }
    float costs[15];
    // now get all the possible SAH values
    for(int i = 0; i < num_bins - 1; ++i) {
        int j = 0;
        Dimension bound1, bound2;
        int num_tris1(0), num_tris2(0);
        // this is one half split
        for(; j <= i; ++j) {
            bound1 = bound1.Union(bins[j].bounds_);
            num_tris1 += bins[j].tris_.size();
        }
        // this is the other half split
        for(; j < num_bins; ++j) {
            bound2 = bound2.Union(bins[j].bounds_);
            num_tris2 += bins[j].tris_.size();
        }
        // finally, calculate the SAH!
        costs[i] = .125 + (num_tris1 * surfaceArea(bound1) + num_tris2 * surfaceArea(bound2)) / parent_sa;
    }
    // now that we have all the costs, we can find the best split
    float min_cost = costs[0];
    int min_split = 0;
    for(int i = 0; i < num_bins - 1; ++i) {
        if(costs[i] < min_cost) {
            min_split = i;
        }
    }
    // Finally, determine whether or not we need to create a leaf node
    float parent_cost = .125 + in_tris.size();
    if(min_cost < parent_cost || in_tris.size() > max_tri) {
        // we need to subdivide
        float cen_min = centroid_bnds[axis].first;
        auto mid = partition(in_tris.begin(), in_tris.end(), [=](const triangle_info & tri) {
            float centroid_pos = (axis == 0) ? tri.centroid_.x : (axis == 1) ? tri.centroid_.y : tri.centroid_.z;
            float normalized_position = (centroid_pos - cen_min) / ranges[axis];
            int bucket_index = min(int(num_bins * normalized_position), num_bins - 1);
            return bucket_index <= min_split;
        });
        bin_1 = vector<triangle_info>(in_tris.begin(), mid);
        bin_2 = vector<triangle_info>(mid, in_tris.end());
        // we can continue to subdivide;
        return true;
    } else {
        // this needs to become a leaf node
        return false;
    }
}   

bool bvh::splitMidpoint(vector<triangle_info> in_tris, vector<triangle_info> &bin_1,
                        vector<triangle_info> &bin_2) {
    Dimension d = getExtent(in_tris); 
    float thresh;
    float arr[3];
    arr[0] = abs(d.max_x - d.min_x);
    arr[1] = abs(d.max_y - d.min_y);
    arr[2] = abs(d.max_z - d.min_z);
    int index = 0;
    float max = arr[0];
    for(int i = 0; i < 3; ++i) {
        if(arr[i] > max) {
            index = i;
            max = arr[i];
        }
    }
    // now split
    if(index == 0) {
        // split on x
        std::sort(in_tris.begin(), in_tris.end(), [](const triangle_info& tri_1, const triangle_info &tri_2) {
            return tri_1.centroid_.x < tri_2.centroid_.x;
        });
    }
    else if (index == 1) {
        // split on y
        std::sort(in_tris.begin(), in_tris.end(), [](const triangle_info& tri_1, const triangle_info &tri_2) {
            return tri_1.centroid_.y < tri_2.centroid_.y;
        });
    }
    else {
        // split on z
        std::sort(in_tris.begin(), in_tris.end(), [](const triangle_info& tri_1, const triangle_info &tri_2) {
            return tri_1.centroid_.z < tri_2.centroid_.z;
        });
    }

    // now give half the leaves to each child
    int half = in_tris.size() / 2;
    bin_1 = vector<triangle_info>(in_tris.begin(), in_tris.begin()+half);
    bin_2 = vector<triangle_info>(in_tris.begin()+half, in_tris.end());
    return true;
}
/**
 * Check if the ray(p, d) intersects the tree
 */ 
bool bvh::intersect(Ray ray, HitInfo& hit) {
    float global_i_time = INFINITY;
    return intersectIterative(ray, hit);
    //return intersectRecurse(0, p, d, hit, global_i_time);
}

/**
 * Check if the ray(p, d) intersects cur_node
 */ 
bool bvh::intersectRecurse(int node_offset, Ray ray, HitInfo& hit, float &best_time) {
    // Base case - make sure the ray actually hits this box
    float b_t = INFINITY;
    if(node_offset == -1 || !AABBIntersect(ray, bvh_nodes_[node_offset].AABB_, b_t) || b_t > best_time) return false;
    compressed_node cur_node = bvh_nodes_[node_offset];
    if(cur_node.l_child_ == -1 && cur_node.r_child_ == -1) {
        // check all the triangles in the leaf
        size_t num_tri = cur_node.tri_offsets_.size();
        bool result = false;
        HitInfo tri_hit;
        for(size_t i = 0; i < num_tri; ++i) {
            HitInfo other_hit;
            result = TriangleIntersect(ray, *triangles_[cur_node.tri_offsets_[i]], other_hit) || result;
            tri_hit = (other_hit.t < tri_hit.t) ? other_hit : tri_hit;
        }
        hit = tri_hit;
        best_time = min(best_time, tri_hit.t);
        return result;
    }
    HitInfo hit1, hit2;
    
    bool result1 = intersectRecurse(cur_node.l_child_, ray, hit1, best_time);
    bool result2 = intersectRecurse(cur_node.r_child_, ray, hit2, best_time);
    if(result1 || result2) {
        hit = (hit1.t < hit2.t) ? hit1 : hit2;
        return true;
    }
    return false;
}

bool bvh::intersectIterative(Ray ray, HitInfo &hit) {
    stack<int> nodes;
    nodes.push(0);
    float best_t = INFINITY;
    bool result = false;
    while(!nodes.empty()) {
        int cur_node_idx = nodes.top();
        nodes.pop();
        float i_t = INFINITY;
        if(cur_node_idx == -1 || !AABBIntersect(ray, bvh_nodes_[cur_node_idx].AABB_, i_t) || i_t > best_t) {
            continue;
        }
        compressed_node cur_node = bvh_nodes_[cur_node_idx];
        if(cur_node.l_child_ == -1 && cur_node.r_child_ == -1) {
            // check to see if there is a collision
            size_t num_tri = cur_node.tri_offsets_.size();
            HitInfo tri_hit;
            for(size_t i = 0; i < num_tri; ++i) {
                HitInfo other_hit;
                result = TriangleIntersect(ray, *triangles_[cur_node.tri_offsets_[i]], other_hit) || result;
                tri_hit = (other_hit.t < tri_hit.t) ? other_hit : tri_hit;
            }
            hit = min(hit, tri_hit);
            best_t = min(best_t, tri_hit.t);
        }
        else {
            // add child nodes to the stack
            nodes.push(cur_node.r_child_);
            nodes.push(cur_node.l_child_);
        }
    }
    return result;
}
