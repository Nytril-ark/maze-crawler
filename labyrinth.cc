//=============================================================INLCUDE
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <array>
#include <unordered_map>

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


typedef std::vector<int> Coord;
typedef std::vector<std::vector<int>> CoordMatrix;
typedef std::unordered_map<int, std::vector<int>> AdjList;


class Solver {
  private:
    int totalNodes = 0;
    bool nodesCounted = false;
  public:
    Solver() {
    }
    
  
    CoordMatrix getNodeMatrix() {
      Matrix currentMaze = mazeToNums();  
      CoordMatrix nodes;
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
    

    Matrix showNodes() {
      Matrix mazeWithNodes = mazeToNums();
      Solver servant;
      CoordMatrix nodes = servant.getNodeMatrix();
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
      CoordMatrix nodes = servant.getNodeMatrix();
      Matrix mazeWithNodes = servant.showNodes();
      int numRows = nodes.size();
      bool hitObstacle = false;
      bool rightNeighbour = false;
      bool leftNeighbour = false;
      bool upperNeighbour = false;
      bool lowerNeighbour = false;
      int jump = 1;

      for (int y = 0; y < totalRows; y++) {
        for (int x = 0; x < totalCols; x++) {
          for (int pairIndex = 0; pairIndex < numRows; pairIndex++) {
            if (nodes[pairIndex][0] == x && nodes[pairIndex][1] == y) { 
              while (!rightNeighbour && !hitObstacle) { 
                if (mazeWithNodes[y][x + jump] == 9) { 
                  rightNeighbour = true;
                  //attach the nodes together
                  //=============================
                  jump = 1;
                } else if (mazeWithNodes[y][x + jump] == 1 || mazeWithNodes[y][x + jump] == 2 || mazeWithNodes[y][x + jump] == 3) {
                  hitObstacle = true;
                  jump = 1;
                } else {
                  jump++;
                }
              }

              while (!leftNeighbour && !hitObstacle) { 
                if (true) {

                  leftNeighbour = true;
                }  
              }

              while (!upperNeighbour && !hitObstacle) { 
                if (true) {

                  upperNeighbour = true;  
                }
              }

              while (!lowerNeighbour && !hitObstacle) { 
                if (true) {

                  lowerNeighbour = true;
                }                   
              }
            } 
          }
        }
      } 
        
      return adjNodes;
    }
};



//=============================================================MAIN
int main() {
  setup("/home/hexogen/mazes/", 1); 
  //=============================================Block of code===========================
  printMaze();                                //displays maze, then nodes, and nodecount.
  Solver slave;
  CoordMatrix outp = slave.getNodeMatrix();
  std::cout << std::endl << "Node count: " << slave.getNodeNum() << std::endl;
  printMatrix(slave.showNodes());
  //=====================================================================================
  
  

  return 0;


}


