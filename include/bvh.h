#ifndef bvh_h
#define bvh_h

#include <vector>

#include "ray.h"
#include "structs.h"

using namespace std;

struct Dimension {
    float min_x;
    float min_y;
    float min_z;
    float max_x;
    float max_y;
    float max_z;

    Dimension() {
        min_x = INFINITY;
        min_y = INFINITY;
        min_z = INFINITY;
        max_x = -INFINITY;
        max_y = -INFINITY;
        max_z = -INFINITY;
    };
    Dimension(const Dimension& d) {
        min_x = d.min_x;
        min_y = d.min_y;
        min_z = d.min_z;
        max_x = d.max_x;
        max_y = d.max_y;
        max_z = d.max_z;
    };
    Dimension Union(const Dimension & rhs) {
        Dimension join;
        join.min_x = min(rhs.min_x, min_x);
        join.min_y = min(rhs.min_y, min_y);
        join.min_z = min(rhs.min_z, min_z);
        join.max_x = max(rhs.max_x, max_x);
        join.max_y = max(rhs.max_y, max_y);
        join.max_z = max(rhs.max_z, max_z);
        return join;
    };
    pair<float, float> operator[](int idx) {
        if(idx == 0) {
            return pair<float, float>(min_x, max_x);
        }
        else if(idx == 1) {
            return pair<float, float>(min_y, max_y);
        }
        else {
            return pair<float, float>(min_z, max_z);
        }
    };
};

struct node {
    Dimension AABB_;
    node * l_child_ = nullptr;
    node * r_child_ = nullptr;
    vector<int> tri_offsets_;
};

struct compressed_node {
    Dimension AABB_;
    int l_child_ = -1;
    int r_child_ = -1;
    vector<int> tri_offsets_;
};

struct triangle_info {
  Dimension AABB_;
  Vec3 centroid_;
  int tri_offset_;
};

class bvh {
  public:
    bvh() {};
    bvh(vector<Triangle*> tris);
    ~bvh() {};
    bool intersect(Ray ray, HitInfo& hit);
  private:
    node * root = nullptr;
    vector<Triangle*> triangles_;
    vector<compressed_node> bvh_nodes_;
    int max_tri = 4;

    Dimension getExtent(vector<node*> nodes);
    Dimension getExtent(vector<triangle_info> tris);
    Dimension getExtent(vector<Vec3> pts);
    float surfaceArea(Dimension AABB);
    vector<triangle_info> boundTriangles();
    bool splitSAH(vector<triangle_info> in_tris, vector<triangle_info> &bin_1, vector<triangle_info> &bin_2);
    bool splitMidpoint(vector<triangle_info> in_tris, vector<triangle_info> &bin_1, vector<triangle_info> &bin_2);
    void buildRecurse(int node_offset, vector<triangle_info> tris);
    bool intersectRecurse(int node_offset, Ray ray, HitInfo& hit, float &best_time);
    bool intersectIterative(Ray ray, HitInfo& hit);
    void destruct(node * cur_node);
};

#endif  // bvh_h