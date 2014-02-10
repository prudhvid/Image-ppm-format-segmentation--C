Image-ppm-format-segmentation--C
================================

Image segmentation refers to partitioning an image into pairwise disjoint regions. In each region, the pixels are similar to one another. Minimum spanning trees can be used to produce image segmentation.

Algorithm::
I have taken an undirected weighted graph G as follows. The vertices of G are the pixels of the image.So, two vertices are adjacent if and only if their corresponding pixels (conceived as unit squares) share an edge or a corner. Therefore, each pixel has exactly eight neighbors (except those lying on the image boundary). Let the weight of an edge (u,v) in G be taken  the color distance between the pixels corresponding to u and v. If u and v have RGB values (r1,g1,b1) and (r2,g2,b2), I have taken the common Euclidean distance as color distance i.e |r1 − r2| + |g1 − g2| + |b1 − b2|.So, Similar pixels therefore have small distances.

After the graph is prepared,I have computed the MST by using Prim's algorithm (good enough). Suppose that we want to break the image into s segments. The s − 1 costliest edges in T are identified and deleted from T. We get a forest with s connected components, each standing for a region in the segmented image :).

From now its obvious run multiple BFS/DFS traversals on this forest to identify the s components. For each component, use a single color, and store the segmented image in the ppm format :)
 
 
 
 
 
 
 
 
 
I have shown both the original file and segmented image genrated from the algorithm.
