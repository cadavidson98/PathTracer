#ifndef bvh_h
#define bvh_h

#include <vector>

#include "ray.h"
#include "structs.h"

/**
 * Dimension is an Axis Aligned Bounding Box, which is used to sub-divide the
 * Scene space into smaller, easier to search sections
 */
struct Dimension {
    float min_x_;
    float min_y_;
    float min_z_;
    float max_x_;
    float max_y_;
    float max_z_;

    Dimension() {
        min_x_ = INFINITY;
        min_y_ = INFINITY;
        min_z_ = INFINITY;
        max_x_ = -INFINITY;
        max_y_ = -INFINITY;
        max_z_ = -INFINITY;
    };

    /**
     * @brief Find the total 3D region containing 2 bounding boxes
     * 
     * @param rhs A second bounding box, which is join with this one
     * @return Dimension spanning both bounding boxes.
     */
    Dimension Union(const Dimension & rhs) {
        Dimension join;
        join.min_x_ = std::min(rhs.min_x_, min_x_);
        join.min_y_ = std::min(rhs.min_y_, min_y_);
        join.min_z_ = std::min(rhs.min_z_, min_z_);
        join.max_x_ = std::max(rhs.max_x_, max_x_);
        join.max_y_ = std::max(rhs.max_y_, max_y_);
        join.max_z_ = std::max(rhs.max_z_, max_z_);
        return join;
    };

    /**
     * @brief returns the x, y, or z bounds of a bounding box
     * 
     * @param idx integer specifying which dimension to return:
     * 0 returns x, 1 returns y, everthing else returns z
     * @return std::pair<float, float> the min and max value in the requested dimension
     */
    std::pair<float, float> operator[](int idx) {
        if(idx == 0) {
            return std::pair<float, float>(min_x_, max_x_);
        }
        else if(idx == 1) {
            return std::pair<float, float>(min_y_, max_y_);
        }
        else {
            return std::pair<float, float>(min_z_, max_z_);
        }
    };
};

/**
 * @brief A small node in a bounding volume heirarchy.
 * 
 */
struct compressed_node {
    Dimension AABB_;  //! bounding box which encapsulates the node
    int l_child_ = -1;  //! integer offset to left child node
    int r_child_ = -1;  //! integer offset to right child node
    std::vector<int> tri_offsets_;  //! integer offsets to all trianges in this node
};

/**
 * @brief A structure for describing a triangle leaf node. Used during
 * bounding volume heirarchy construction to find best split across leaf nodes.
 */
struct triangle_info {
  Dimension AABB_;  //! bounding box which encapsulates the triangle
  Vec3 centroid_;  //! center of the bounding box
  int tri_offset_;  //! integer offset to the triangle stored in this node
};

/**
 * @brief A spatial data structure for storing and finding triangles in ray-scene intersections.
 * 
 */
class bvh {
  public:
    bvh() {};
    bvh(std::vector<Triangle> &tris);
    bool intersect(const Ray &ray, HitInfo &hit);
  private:
    std::vector<Triangle> triangles_;  //! All of the triangles in the bounding volume
    std::vector<compressed_node> bvh_nodes_;  //! A flattened tree containing all of the triangles encoded in world space
    int max_tri_ = 4;  //! The maximum number of triangles per leaf node

    Dimension getExtent(const std::vector<triangle_info> &tris);
    Dimension getExtent(const std::vector<Vec3> &pts);
    float surfaceArea(const Dimension &AABB);
    std::vector<triangle_info> boundTriangles();
    bool splitSAH(std::vector<triangle_info> &in_tris, std::vector<triangle_info> &bin_1, std::vector<triangle_info> &bin_2);
    bool splitMidpoint(std::vector<triangle_info> &in_tris, std::vector<triangle_info> &bin_1, std::vector<triangle_info> &bin_2);
    void buildRecurse(int node_offset, std::vector<triangle_info> &tris);
    bool intersectIterative(const Ray &ray, HitInfo &hit);
};

#endif  // bvh_h