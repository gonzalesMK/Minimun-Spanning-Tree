#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <list>

#define DEBUG 0
#define DEBUG_PRIM 0
const int SIZE = 20;
const int DENSITY = 40;
const int DENSITY_2 = 40;
const int RANGE = 10;
const int D_MIN = 1;
const int N = 50;

struct Path{
    int cost;
    int start;
    int end;
};
//! A path is equal to other if both start and end are equal ( It's assumed that is just one path between two vertices)
inline bool operator==( Path path1, Path path2 ){
    if(path1.start == path2.start && path1.end == path2.end) {
        return true;
    }else{
        return false;
    }
}
//! Print the path in the screen (for debug)
inline std::ostream& operator<<( std::ostream& out , Path& path){
    out << "Path Cost: " << path.cost <<" \t Path Start: " << path.start << "\t Path End: " << path.end << std::endl;
    return out;
}
/// Class that lists the paths and index it
class Queue{
public:
    std::list<Path> set;
    Queue(){};                                      /// Default Constructor
    void Insert( int cost, int start, int end );    /// Insert a new Path
    Path Best(){ return set.front();}               /// Return Best Path
    void Delete(Path path){set.remove(path);}       /// Delete the path
    void Delete(int end);                           /// Delete all paths for that vertice
    bool empty(){ return set.empty();}              /// Return true if Queue is empty
};
/// Print Queue in the Screen
inline std::ostream& operator<<( std::ostream& out , Queue& queue){
        for( std::list<Path>::iterator it = queue.set.begin() ; it != queue.set.end(); it++)
            out << *it << std::endl;
    return out;
}

//! Insert some NEW path in ORDER of COST (BEST IS FIRST)
inline void Queue::Insert( int cost, int start, int end){
    Path path = {cost, start, end};
    #if DEBUG
        std::cout << "Inserting new path: " << path << std::endl;
    #endif // DEBUG
    // If set is empty, it puts the first element
    if( this->set.empty()){
        set.push_back(path);
    } // If not, put path in the right position
    else{
        std::list<Path>::iterator it = set.begin();
        for(; it != set.end(); it++){
            if( path.cost < it->cost){
                set.insert(it, path);
                break;
            }
        }
        if (!(it != set.end())) set.push_back(path); // Insert in the end of the list (If not inserted yet)
    }
    #if DEBUG
        std::cout << "Actual Set: ";
        for( std::list<Path>::iterator it = set.begin() ; it != set.end(); it++)
                std::cout << " *****" << *it << std::endl;
    #endif // DEBUG
}

inline void Queue::Delete(int vertice){
    for( int i = 0 ; i < SIZE ; i ++){
        Path path = { 0 , i , vertice};
        set.remove(path);
    }
}
/*************CLASS GRAPH ****************/
class Graph{
public:
    std::vector< std::vector<int> > graph;
    Graph(int d = DENSITY, int s = SIZE):size(s),density(d){
        for( int i = 0; i < size; ++i){
            std::vector<int> row; // Create an empty row
            for( int j=0; j < size; j++)
                row.push_back(0); // Add elements to the row
            graph.push_back(row); // Add the row to the main vector
        }
        FillGraph();
    }
    void FillGraph();                                      /// Fill the graph
    bool adjacent(int x, int y);                           /// Return true if points are connected
    std::vector<int> neighbors(int x);                     /// Return a vector with neighbors points
    int GSize(){ return size;}                             /// Return size
    int Short_Path( int source, int target);               /// Return shortest path ( Dijkstra algorithm)
    void MST( int source);                                 /// Return Minimum Spanning Tree ( Jarnik-Prim algorithms)
    friend std::ostream& operator<<( std::ostream& out, Graph& g);
private:
   int size, density;
};

/// Fill the Graph with distances
inline void Graph::FillGraph(){
    srand( time(NULL));
    for(int i = 0 ; i < size ; i++)
        for(int j = i; j < size ; j++)
            if(i == j) graph[i][j] = 0;
                else graph[i][j] = graph[j][i] = ((rand() % 100) < density) ? (rand()% RANGE + D_MIN) : 0 ;
}

/// Overloading operator << for printing the graph
inline std::ostream& operator<<( std::ostream& out, Graph& g ){
        out << "- | ";
        for(int i = 0 ; i < g.size ; i++)
            out << i << " " ;
        out << std::endl;
        for( int i =0; i < g.size; i++){
            out << i <<" | " ;
            for( int j = 0; j< g.size; j++)
                out << g.graph[i][j] << " " ;
            out <<"|" << std::endl ;
        }
    return out;
}

/// Check if X is connected with Y
inline bool Graph::adjacent(int x, int y){
    if( graph[x][y] != 0) return true;
    else return false;
}

/// Return all nodes connected with X
inline std::vector<int> Graph::neighbors(int x){
    std::vector<int> node;
    for(int i =0; i < size; i++)
        if(graph[x][i] != 0) node.push_back(i);
    return node;
}
/// Dijkstra's algorithm
int Graph::Short_Path(int source, int target){
    std::list<int> Q; /// List Q of all vertices
    int cost[size], prev[size];
    bool goal = false;

    for( int i=0; i < size; i++){   /// Initialization
        cost[i] = 32000;          /// 64000 is equal to infinity
        prev[i] = -1 ;              /// -1 is equal to undefined
        Q.emplace_back(i);
    }
    cost[source] = 0;
    while(!Q.empty() && !goal){             /// For all vertices e Q or reach the target
        int u = target ;
        for(std::list<int>::iterator i = Q.begin(); i != Q.end(); ++i){
            u = cost[*i] < cost[u] ? (*i) : u ;
        }
        Q.remove(u);    /// It takes Q's lowest value and delete it (to not have redundant search)
        std::vector<int> vec = neighbors(u);
        if (u == target)
            goal=true;                  /// Quit search if target is found
        for(std::vector<int>::iterator it = vec.begin(); it != vec.end(); ++it){
            int new_cost = cost[u] + graph[u][*it];
            if( new_cost < cost[*it]){     /// A shorter path to v has been found
                cost[*it] = new_cost;
                prev[*it] = u;
            }
        }
    }
    return cost[target];
}

void Graph::MST(int source){
     std::list<int> Close_set;
     int cost = 0;
     Queue Open_set;
     Close_set.push_back(source);

    // Inicialize Open Set with source neighbors
    std::vector<int> neighbor = neighbors(source);
    for( std::vector<int>::iterator it = neighbor.begin() ; it != neighbor.end(); it++ ) //! Add all sources neighbors
        Open_set.Insert( graph[source][*it] , source , *it);

    int vertices = 1;
    while( vertices < (size) && !Open_set.empty()){
        int  flag = 0;
        Path path = Open_set.Best();              //! Get best path
        Open_set.Delete(path);                   //! Delete that path to not have redundant paths
        for( std::list<int>::iterator it = Close_set.begin() ; it != Close_set.end(); it++ ){ //! Check if there is already a path for that Vertice
            if (path.end == *it) flag++;
        }

        if( flag == 0 ){  //! If it's a new vertice...
            Close_set.push_back(path.end);
            cost += path.cost;
            std::vector<int> vertice_neighbors = neighbors(path.end);
            for( std::vector<int>::iterator it = vertice_neighbors.begin() ; it != vertice_neighbors.end(); it++ ) //! Put all neighbors of Vertice in the Open Set
                Open_set.Insert( graph[path.end][*it] , path.end , *it );
            for( std::list<int>::iterator it = Close_set.begin(); it != Close_set.end() ; it++)
                Open_set.Delete( *it );
            vertices++;
           #if DEBUG_PRIM
                std::cout << " OPEN SET ATUAL: " << std::endl << Open_set << std::endl << "CLOSE SET ATUAL(" << vertices << "): " ;
                for( std::list<int>::iterator it = Close_set.begin() ; it != Close_set.end(); it++ ){ //! Check if there is already a path for that Vertice
                    std::cout<< *it << " " ;
                }
                std::cout << std::endl << "CAMINHO ESCOLHIDO: " << path ;
                system("pause");
            #endif

        }
    }
    // If a path is found
    if ( vertices == (size )){
        std::cout<< "The best path is: ";
        for( std::list<int>::iterator it = Close_set.begin() ; it != Close_set.end(); it++ ){ //! Check if there is already a path for that Vertice
                std::cout<< *it << " " ;
            }
        std::cout<< std::endl << "With cost: " << cost << std::endl;
    }
    else std::cout << "No path found" ;


}

int main(){
    Graph graph;
    std::cout<< graph ;
    graph.MST(1);

}
