//=============================================================INLCUDE
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <array>
#include <unordered_map>
#include <boost/functional/hash.hpp>
#include <limits>
#include <queue>
#include <typeinfo>

//=============================================================FUNCTIONS

std::string mazePath = "/home/hexogen/1.txt";
std::string line;
std::ifstream maze(mazePath);


void restart() {
  maze.clear();
  maze.seekg(0);
}


int countCols() {
  restart();
  int i = 0;
  std::getline(maze, line);
  i = line.size();
  return i;
}


int countRows() {
  restart();
  int i = 0;
  while(std::getline(maze, line)) {
    i++;
  }
  return i;
}


int totalRows = 0;
int totalCols = 0;

void setup(std::string path, int mazeNum) {
  mazePath = path + std::to_string(mazeNum) + ".txt";
  maze.close();
  maze.open(mazePath);
  totalRows = countRows();
  totalCols = countCols();
}


void printMaze() {
  restart();
  while (std::getline(maze, line)) {
    std::cout << line << std::endl;
  }
}


int countRowObs(int row) {
  restart();
  int obstacles = 0; 

  for (int i = 0; i <= row; i++) {
  std::getline(maze, line);
  }

  char *ch = &line[0];
  for (int i = 0; i < line.size(); i++) {
    if (*ch == '-' || *ch == '|' || *ch == 'o') {
      obstacles++; 
    }
  ch++;
  }
  return obstacles;
}



struct mazeInfo {
  int vObstacle = 0;
  int hObstacle = 0;
  int post = 0;
  int total = 0;
};


mazeInfo totalObstacles() {
  restart();
  mazeInfo info;

  while (std::getline(maze, line)) {
    char *ch = &line[0];
    for (int i = 0; i < line.size(); i++) {
      if (*ch == '|') {
        info.vObstacle++;
        info.total++;
      } else if (*ch == '-') {
        info.hObstacle++;
        info.total++;
      } else if (*ch == 'o') {
        info.post++;
        info.total++;
      }
      ch++;
    }
  }

  return info;
}


typedef std::vector<std::vector<int>> Matrix;
Matrix mazeMatrix(totalRows, std::vector<int>(totalCols, 0));


Matrix mazeToNums() {
  restart();
  Matrix converted(totalRows, std::vector<int>(totalCols, 0));

  for (int i = 0; i < totalRows; i++) {
    std::getline(maze, line);
    char *ch = &line[0];
    for (int j = 0; j < totalCols; j++) {
      if (*ch == ' ') {
        converted[i][j] = 0;
      } else if (*ch == 'o') {
        converted[i][j] = 1;
      } else if (*ch == '-') {
        converted[i][j] = 2;
      } else if (*ch == '|') {
        converted[i][j] = 3;
      } else if (*ch == 'G') {
        converted[i][j] = 4;
      } else if (*ch == 'S') {
        converted[i][j] = 5;
      }
      ch++;
    }
  }
  return converted;
}


void printMatrix(Matrix matrix) {
  int numRows = matrix.size();
  int numCols = matrix[0].size();
  for (int i = 0; i < numRows; i++) {
    for (int j = 0; j < numCols; j++) {
      std::cout << matrix[i][j];
    } std::cout << std::endl;
  }
}

int manhattanDistance(int x1, int y1, int x2, int y2) {
    return abs(x2 - x1) + abs(y2 - y1);
}



typedef std::vector<int> Coord; 
typedef std::pair<int, int> pCoord;

typedef std::unordered_map<
  std::pair<int, int>, 
  std::vector<std::pair<std::pair<int, int>, int>>,
  boost::hash<std::pair<int, int>>
> AdjList; 


typedef std::priority_queue<
  std::pair<int, std::pair<int, int>>,
  std::vector<std::pair<int, std::pair<int, int>>>,
  std::greater<>
> priorityQueue; 


typedef std::unordered_map<
  std::pair<int, int>, int,
  boost::hash<std::pair<int, int>>
> distanceMap;   


class Solver {
  private:
    int totalNodes = 0;
    int connections = 0;
    bool nodesCounted = false;
  public:
    Solver() {
    }
    
  
    Matrix getNodeMatrix() {
      Matrix currentMaze = mazeToNums();  
      Matrix nodes;
      nodesCounted = true;

      for (int i = 0; i < totalRows; i++) {
        for (int k = 0; k < totalCols; k++) {
          if (currentMaze[i][k] == 0) {
            if (currentMaze[i][k + 1] == 0 && currentMaze[i][k - 1] == 0) {
              if ((currentMaze[i][k + 2] == 0 || currentMaze[i][k - 2] == 0) && (currentMaze[i + 1][k] == 0 || currentMaze[i - 1][k] == 0)) {
                if ((currentMaze[i + 1][k + 1] == 0 && currentMaze[i + 1][k - 1] == 0) || (currentMaze[i - 1][k + 1] == 0 && currentMaze[i - 1][k - 1] == 0)) {
                  nodes.push_back({k, i});
                  totalNodes++;
                }
              } 
            } 
          } else if (currentMaze[i][k] == 5) {
              nodes.push_back({k, i});
              totalNodes++;
          } else if (currentMaze[i][k] == 4) {
              nodes.push_back({k, i});
              totalNodes++;
          }
        }
      }

      return nodes;
    }
    

    Coord locateChar(char target) {
      restart();
      Coord output(2, -1);
      int k = 0;
      bool found = false;

      while (std::getline(maze, line) && !found) {
        for (int i = 0; i < line.size() && !found; i++) {
          if (line[i] == target) {
            output[0] = i;
            output[1] = k;
            found = true;
          }
        }
        k++;
      }     
      return output;
    }
    

    int getNodeNum() {
      if (nodesCounted) {
        return totalNodes;
      } else {
        return 0;
      }
    }
      

    int getConnections() {
      return connections;
    }


    Matrix showNodes() {
      Matrix mazeWithNodes = mazeToNums();
      Solver servant;
      Matrix nodes = servant.getNodeMatrix();
      int numRows = nodes.size();
      bool isNode;
      int current;
      
      for (int k = 0; k < mazeWithNodes.size(); k++) {
        for (int i = 0; i < mazeWithNodes[0].size(); i++) {
          isNode = false;
          current = mazeWithNodes[k][i];
          for (int pairIndex = 0; pairIndex < numRows; pairIndex++) {
            if (nodes[pairIndex][0] == i && nodes[pairIndex][1] == k) {
              mazeWithNodes[k][i] = 9;
              isNode = true;
            } 
          }
          if (!isNode) {
            mazeWithNodes[k][i] = current;
          }
        }
      }
      return mazeWithNodes;
    }

  
    AdjList getAdjNodes() {
      AdjList adjNodes;
      Solver servant;
      Matrix nodes = servant.getNodeMatrix();
      Matrix mazeWithNodes = servant.showNodes();
      int numRows = nodes.size();
      bool hitObstacle = false;
      bool rightNeighbour = false;
      bool leftNeighbour = false;
      bool upperNeighbour = false;
      bool lowerNeighbour = false;
      int jump = 1;
      int neighbourX, neighbourY, weight;
      connections = 0;


      for (int y = 0; y < totalRows; y++) {
        for (int x = 0; x < totalCols; x++) {
          for (int pairIndex = 0; pairIndex < numRows; pairIndex++) {
            if (nodes[pairIndex][0] == x && nodes[pairIndex][1] == y) {
              
              jump = 1;
              hitObstacle = false;
              rightNeighbour = false;
              while (!rightNeighbour && !hitObstacle && x + jump >= 0 && x + jump < totalCols) { 
                neighbourX = x + jump;
                neighbourY = y;
                if (mazeWithNodes[y][neighbourX] == 9) { 
                  rightNeighbour = true;
                  connections++;
                  weight = manhattanDistance(x, y, neighbourX, neighbourY);
                  adjNodes[{x, y}].push_back({{neighbourX, y}, weight});
                  adjNodes[{neighbourX, y}].push_back({{x, y}, weight});  

                } else if (mazeWithNodes[y][x + jump] == 1 || mazeWithNodes[y][x + jump] == 2 || mazeWithNodes[y][x + jump] == 3) {
                  hitObstacle = true;
                } else {
                  jump++;
                }
              }
              
              jump = 1;
              hitObstacle = false;
              lowerNeighbour = false;
              while (!lowerNeighbour && !hitObstacle && y + jump >= 0 && y + jump < totalRows) { 
                neighbourY = y + jump;
                neighbourX = x;
                if (mazeWithNodes[neighbourY][x] == 9) { 
                  lowerNeighbour = true;
                  connections++;
                  weight = manhattanDistance(x, y, neighbourX, neighbourY);
                  adjNodes[{x, y}].push_back({{x, neighbourY}, weight});
                  adjNodes[{x, neighbourY}].push_back({{x, y}, weight});
                  
                } else if (mazeWithNodes[y + jump][x] == 1 || mazeWithNodes[y + jump][x] == 2 || mazeWithNodes[y + jump][x] == 3) {
                  hitObstacle = true;
                } else {
                  jump++;
                }
              }                   
            } 
          }
        }
      } 
        
      return adjNodes;
    }
    
    
    void dijkstra() {
      std::pair<int, pCoord> currNode;
      distanceMap   distances;  
      priorityQueue pq;
      pCoord  currCoord;
      Solver  servant;
      int     currDist;
      int     nProcessed = 0;
      AdjList adjacency  = servant.getAdjNodes();
      Coord   temp       = servant.locateChar('S');
      Matrix  nodeMatrix = servant.getNodeMatrix();
      pCoord  start      = {temp[0], temp[1]};
              temp       = servant.locateChar('G');
      pCoord  goal       = {temp[0], temp[1]};
      
   
      for (const auto& node : nodeMatrix) {    // initializing distance map.
        if (node[0] == start.first && node[1] == start.second) {
          distances[{node[0], node[1]}] = 0;
        } else {
          distances[{node[0], node[1]}] = INT_MAX;
        }
      }
      
      pq.push({0, start});

      while (!pq.empty()) {
        currNode   = pq.top();
        currDist   = currNode.first;
        currCoord  = currNode.second;
        pq.pop();
        nProcessed++;
        if (currCoord == goal) {
          break;
        } else {
          for (const auto& node : adjacency[currCoord]) {
            std::cout << "Neighbor: (" << node.first.first << ", " << node.first.second << "), Weight: " << node.second << std::endl;
          }
        }
      }
    }
};





//function to be removed later=============================================================================:
void printNodeWeights(const AdjList& adjList) {
    for (const auto& nodeEntry : adjList) {
        const std::pair<int, int>& node = nodeEntry.first;
        const std::vector<std::pair<std::pair<int, int>, int>>& neighbors = nodeEntry.second;
        
        std::cout << "Node (" << node.second + 1 << ", " << node.first + 1 << "):\n";

        for (const auto& neighbor : neighbors) {
            const std::pair<int, int>& neighborNode = neighbor.first; 
            float weight = neighbor.second;  

            std::cout << "  Neighbor (" << neighborNode.second + 1 << ", " << neighborNode.first + 1
                      << ") with weight: " << weight << "\n";
        }

        std::cout << "\n"; 
    }
}


//=============================================================MAIN
int main() {
  setup("/home/hexogen/mazes/", 4); 
  //=============================================Block of code===========================
  printMaze();                                //displays maze, then nodes, and nodecount.
  Solver slave;
  Matrix outp = slave.getNodeMatrix();
  std::cout << std::endl << "Node count: " << slave.getNodeNum() << std::endl;
  //AdjList temp = slave.getAdjNodes();
  //printNodeWeights(temp);
  //std::cout << std::endl;
  slave.dijkstra();
  std::cout << std::endl;
  //=====================================================================================
  
  

  return 0;
//NOTE: add out of bound checks to everything;
//NOTE: try to improve and reach O(1) lookups;
//NOTE: made a struct for int pairs;
//NOTE: look into unordered_map hashes;
//NOTE: consider replacing some vectors for sets;
              /*     //===========================================================================Commented due to double-counting==
              jump = 1;                                                                     //of the node connections.
              hitObstacle = false;                                                          //belongs to the getAdjNodes() method
              leftNeighbour  = false; 
              while (!leftNeighbour && !hitObstacle && x - jump >= 0 && x - jump < totalCols) { 
                if (mazeWithNodes[y][x - jump] == 9) { 
                  leftNeighbour = true;
                  connections++;
                  //attach the nodes together
                  //=============================
                } else if (mazeWithNodes[y][x - jump] == 1 || mazeWithNodes[y][x - jump] == 2 || mazeWithNodes[y][x - jump] == 3) {
                  hitObstacle = true;
                } else {
                  jump++;
                }
              }
              
              jump = 1;
              hitObstacle = false;
              upperNeighbour = false;
              while (!upperNeighbour && !hitObstacle && y - jump >= 0 && y - jump < totalRows) { 
                if (mazeWithNodes[y - jump][x] == 9) { 
                  upperNeighbour = true;
                  connections++;
                  //attach the nodes together
                  //=============================
                } else if (mazeWithNodes[y - jump][x] == 1 || mazeWithNodes[y - jump][x] == 2 || mazeWithNodes[y - jump][x] == 3) {
                  hitObstacle = true;
                } else {
                  jump++;
                }
              } */  //===============================================================================================================

}


