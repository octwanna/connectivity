#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

struct Parameters
{
    double x, y;
    int nx, ny, np;
    double dx, dy;
    int num_verts;

    Parameters(int x_, int y_, int nx_, int ny_) : 
        x(x_), y(y_), nx(nx_), ny(ny_) {}
};

struct Node 
{
    double x, y;
    int neighbors[4] = {-1, -1, -1, -1};
};

struct Edge
{
    int e_start;
    int e_end;
};

class Geometry
{
private:
    Parameters geometry_params;

    std::vector<Node> vertices;
    std::vector<Node> cell_centers;
    std::vector<Edge> edge_list;

public:
    Geometry(Parameters inputs_) : geometry_params(inputs_) {};

    void calculateVertices();
    void calculateCellCenters();
    void calculateConnectivity();
    void generateEdgeList();
    void findFaceNeighbors(int, int, int);

};

int main (void)
{
    Parameters grid_inputs(0.3, 0.4, 3, 4);

    grid_inputs.np = grid_inputs.nx*grid_inputs.ny;
    grid_inputs.num_verts = (grid_inputs.nx + 1)*(grid_inputs.ny + 1);
    grid_inputs.dx = grid_inputs.x/grid_inputs.nx;
    grid_inputs.dy = grid_inputs.y/grid_inputs.ny;

    Geometry geom(grid_inputs);

    return(0);
}

void Geometry::calculateVertices()
{
    double xc = 0.0; 
    double yc = 0.0;
    
    for (int ii = 0; ii < (geometry_params.nx + 1); ++ii)
    {
        for (int jj = 0; jj < (geometry_params.ny + 1); ++jj)
        {
            Node vc;
            vc.x = xc; 
            vc.y = yc;

            vertices.push_back(vc);

            yc += geometry_params.dy;
        }
        xc += geometry_params.dx;
        yc = 0.0;
    }
}

void Geometry::calculateCellCenters()
{
    double xc = 0.0 + geometry_params.dx/2.0;
    double yc = 0.0 + geometry_params.dy/2.0;

    for (int ii = 0; ii < geometry_params.nx; ++ii)
    {
        for (int jj = 0; jj < geometry_params.ny; ++jj)
        {
            Node cel;

            cel.x = xc;
            cel.y = yc;

            cell_centers.push_back(cel);

            yc += geometry_params.dy;
        }
        xc += geometry_params.dx;
        yc = 0.0 + geometry_params.dy/2.0;
    }
    for (int ii = 0; ii < geometry_params.np; ++ii)
    {
        std::cout << ii << ": " << cell_centers[ii].x << ", " << cell_centers[ii].y << std::endl;
    }
}

void Geometry::calculateConnectivity()
{
    double eps = 1e-8;

    for (int ii = 0; ii < geometry_params.np; ++ii)
    {
        Node cc = cell_centers[ii];

        for (int jj = 0; jj < geometry_params.num_verts; ++jj)
        {
            Node vc = vertices[jj];

            if (std::fabs((cc.x - geometry_params.dx/2.0) - vc.x) < eps)
            {
                if (std::fabs((cc.y - geometry_params.dy/2.0) - vc.y) < eps)
                {
                    std::cout << "Cell " << ii << "match with SW vert " << jj << " in x" << std::endl;
                    cell_centers[ii].neighbors[0] = jj;
                    vertices[jj].neighbors[1] = ii;
                }
                else if (std::fabs((cc.y + geometry_params.dy/2.0) - vc.y) < eps)
                {
                    std::cout << "Cell " << ii << "match with NW vert " << jj << " in x" << std::endl;
                    cell_centers[ii].neighbors[3] = jj;
                    vertices[jj].neighbors[0] = ii;
                }
            }
            else if (std::fabs((cc.x + geometry_params.dx/2.0) - vc.x) < eps)
            {
                if (std::fabs((cc.y - geometry_params.dy/2.0) - vc.y) < eps)
                {
                    std::cout << "Cell " << ii << "match with SE vert " << jj << " in x" << std::endl;
                    cell_centers[ii].neighbors[1] = jj;
                    vertices[jj].neighbors[2] = ii;
                }
                else if (std::fabs((cc.y + geometry_params.dy/2.0) - vc.y) < eps)
                {
                    std::cout << "Cell " << ii << "match with NE vert " << jj << " in x" << std::endl;
                    cell_centers[ii].neighbors[2] = jj;
                    vertices[jj].neighbors[3] = ii;
                }
            }
        }
    }
}

void Geometry::findFaceNeighbors(int current_cell_index, int edge_start_vert, int edge_end_vert)
{
       for (int jj = 0; jj < 4; ++jj)
        {
            int neighbor_cell_index = vertices[edge_start_vert].neighbors[jj];

            // some vertices don't have neighbors in particular quadrants
            if (neighbor_cell_index == -1)
            {
                continue;
            }
            else
            {
                // Loop over associated vertices of cells that contain sw_vertex_index
                for (int nn = 0; nn < 4; ++nn)
                {
                    if (cell_centers[neighbor_cell_index].neighbors[nn] == edge_end_vert && neighbor_cell_index != current_cell_index)
                    {
                        std::cout << "Found an e-w edge with a neighbor: verts ( " << edge_start_vert << ", " << edge_end_vert << ")" << std::endl;
                        std::cout << "  Neighbor:" << current_cell_index << "(parent cell), with " << neighbor_cell_index << std::endl;

                        if (current_cell_index < neighbor_cell_index)
                        {
                            std::cout << "Found a unique edge:" << std::endl;
                        }
                    }
                    else
                    {
                        continue;
                    }
                }
            }
        }   
}

void Geometry::generateEdgeList()
{
    // Loop over all cells
    for (int ii = 0; ii < geometry_params.np; ++ii)
    {
        // For a South face of the cell, loop over the 
        int sw_vertex_index = cell_centers[ii].neighbors[0];
        int se_vertex_index = cell_centers[ii].neighbors[1];

        findFaceNeighbors(ii, sw_vertex_index, se_vertex_index);
    }
}

    //     // Loop over cells that use the sw_vertex_index 
 

    //     int ne_vertex_index = cell_centers[ii].neighbors[2];

    //     // Loop over cells that use the sw_vertex_index 
    //     for (int jj = 0; jj < 4; ++jj)
    //     {
    //         int neighbor_cell_index = vertices[se_vertex_index].neighbors[jj];

    //         // some vertices don't have neighbors in particular quadrants
    //         if (neighbor_cell_index == -1)
    //         {
    //             continue;
    //         }
    //         else
    //         {
    //             // Loop over associated vertices of cells that contain sw_vertex_index
    //             for (int nn = 0; nn < 4; ++nn)
    //             {
    //                 if (cell_centers[neighbor_cell_index].neighbors[nn] == ne_vertex_index && neighbor_cell_index != ii)
    //                 {
    //                     std::cout << "Found a n-s edge with a neighbor: verts ( " << se_vertex_index << ", " << ne_vertex_index << ")" << std::endl;
    //                     std::cout << "  Neighbor:" << ii << "(parent cell), with " << neighbor_cell_index << std::endl;

    //                     if (ii < neighbor_cell_index)
    //                     {
    //                         std::cout << "Found a unique edge:" << std::endl;
    //                     }
    //                 }
    //                 else
    //                 {
    //                     continue;
    //                 }
    //             }
    //         }
    //     }

    //     int nw_vertex_index = cell_centers[ii].neighbors[3];
    //     // Loop over cells that use the sw_vertex_index 
    //     for (int jj = 0; jj < 4; ++jj)
    //     {
    //         int neighbor_cell_index = vertices[ne_vertex_index].neighbors[jj];

    //         // some vertices don't have neighbors in particular quadrants
    //         if (neighbor_cell_index == -1)
    //         {
    //             continue;
    //         }
    //         else
    //         {
    //             // Loop over associated vertices of cells that contain sw_vertex_index
    //             for (int nn = 0; nn < 4; ++nn)
    //             {
    //                 if (cell_centers[neighbor_cell_index].neighbors[nn] == nw_vertex_index && neighbor_cell_index != ii)
    //                 {
    //                     std::cout << "Found a n-s edge with a neighbor: verts ( " << ne_vertex_index << ", " << nw_vertex_index << ")" << std::endl;
    //                     std::cout << "  Neighbor:" << ii << "(parent cell), with " << neighbor_cell_index << std::endl;

    //                     if (ii < neighbor_cell_index)
    //                     {
    //                         std::cout << "Found a unique edge:" << std::endl;
    //                     }
    //                 }
    //                 else
    //                 {
    //                     continue;
    //                 }
    //             }
    //         }
    //     }


    //     // Loop over cells that use the sw_vertex_index 
    //     for (int jj = 0; jj < 4; ++jj)
    //     {
    //         int neighbor_cell_index = vertices[nw_vertex_index].neighbors[jj];

    //         // some vertices don't have neighbors in particular quadrants
    //         if (neighbor_cell_index == -1)
    //         {
    //             continue;
    //         }
    //         else
    //         {
    //             // Loop over associated vertices of cells that contain sw_vertex_index
    //             for (int nn = 0; nn < 4; ++nn)
    //             {
    //                 if (cell_centers[neighbor_cell_index].neighbors[nn] == sw_vertex_index && neighbor_cell_index != ii)
    //                 {
    //                     std::cout << "Found a n-s edge with a neighbor: verts ( " << nw_vertex_index << ", " << sw_vertex_index << ")" << std::endl;
    //                     std::cout << "  Neighbor:" << ii << "(parent cell), with " << neighbor_cell_index << std::endl;

    //                     if (ii < neighbor_cell_index)
    //                     {
    //                         std::cout << "Found a unique edge:" << std::endl;
    //                     }
    //                 }
    //                 else
    //                 {
    //                     continue;
    //                 }
    //             }
    //         }
    //     }

    // }
