
//demonstrate api pulls
//demonstrate menu + graph
#include <iostream>
#include <fstream>
#include <cpr/cpr.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <dos.h>
#include <Windows.h>
#include <cstdlib>
#include <chrono>
#include <sstream> 
#include <thread>
#include <limits.h>
#include <iomanip>

using namespace std::chrono;
typedef high_resolution_clock Clock;
using namespace std;

//enter ur riot api key here
string RIOTAPIKEY = "";

vector<pair<int, int>> GetRelations(string file, int championID, int FirstChampion) {
    ifstream inFile(file);
    string line;

    while (getline(inFile, line)) {
        std::istringstream stream(line);
        string fullLine;
        getline(stream, fullLine);
        //cout << fullLine << endl;
        vector<string> ids;
        vector<pair<int, int>> relations;
        std::string delimiter = ",";
        stringstream ss(fullLine);

        while (ss.good()) {
            string substr;
            getline(ss, substr, ',');
            substr.erase(std::remove_if(substr.begin(), substr.end(), [](char c) { return !std::isdigit(c); }), substr.end());
            ids.push_back(substr);
        }

        if (stoi(ids[0]) == FirstChampion) {
            for (int i = 1; i < ids.size() - 1; i += 2) {
                if (stoi(ids[i]) == championID) {
                    relations.push_back(make_pair(9998, stoi(ids[i + 1])));
                }
            }
        }

        if (stoi(ids[0]) == championID) {
            relations.push_back(make_pair(9999, championID));
            for (int i = 1; i < ids.size() - 1; i += 2) {
                relations.push_back(make_pair(stoi(ids[i]), stoi(ids[i + 1])));
            }
            return relations;
        }
    }
}

//if someone mains them: Return vector containing champID's of all characters that people main in the dataset
vector<int> getMainExists(string file, int championID) {
    ifstream inFile(file);
    string line;
    vector<int> mains;

    while (getline(inFile, line)) {
        istringstream stream(line);
        string fullLine;
        getline(stream, fullLine);
        //cout << fullLine << endl;
        vector<string> ids;
        std::string delimiter = ",";
        stringstream ss(fullLine);

        while (ss.good()) {
            string substr;
            getline(ss, substr, ',');
            substr.erase(std::remove_if(substr.begin(), substr.end(), [](char c) { return !std::isdigit(c); }), substr.end());
            ids.push_back(substr);
        }

        mains.push_back(stoi(ids[0]));

    }
    return mains;
}

void getIDS(string huge, vector<string>& summonerIDS) {
    std::string s = huge;
    std::string delimiter = "\"summonerId\":\"";

    size_t pos = 0;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        string suicidal = s.substr(pos + delimiter.length(), 48);
        s.erase(0, pos + delimiter.length());
        summonerIDS.push_back(suicidal);
    }
}

vector<unordered_map<int, pair<int, int>>> getChampData(vector<string> summonerIDS) {
    unordered_map<string, vector<int>> toReturn;
    string base = "https://na1.api.riotgames.com/lol/champion-mastery/v4/champion-masteries/by-summoner/";
    string extra = "?api_key=";
    std::string delimiter = "\"championId\":";
    //cout << delimiter;
    vector<unordered_map<int, pair<int, int>>> championMaps;

    for (int i = 0; i < 999; i++) {
        //unordered_map for each character with relation data
        //{Champion ID, How Related the calling champion is to that ID}
        unordered_map<int, pair<int, int>> myMap;
        championMaps.push_back(myMap);
    }

    int count1 = 0;
    int count3 = 0;
    int countPage = 1;
    for (int i = 0; i < summonerIDS.size(); i++) {
        if (count3 == 205) {
            //cout << "-----------------------------------------------------------------" << countPage << "-------------------------------------------------------------------------" << endl;
            countPage++;
            count3 = 0;
        }
        if (count1 == 90) {
            //this_thread::sleep_for(chrono::milliseconds(120000));
            break;
            count1 = 0;
        }
        string RiotCall = base + summonerIDS[i] + extra + RIOTAPIKEY;
        cpr::Response r = cpr::Get(cpr::Url{ RiotCall });
        std::string s = r.text;
        count1++;
        count3++;

        vector<int> championMastery;
        size_t pos = 0;
        int count = 0;
        while ((pos = s.find(delimiter)) != std::string::npos) {

            if (count == 7) {
                //cout << "NEXT ::::::" << endl;
                break;
            }

            count++;
            string suicidal = s.substr(pos + delimiter.length(), 3);
            suicidal.erase(std::remove_if(suicidal.begin(), suicidal.end(), [](char c) { return !std::isdigit(c); }), suicidal.end());
            s.erase(0, pos + delimiter.length());
            championMastery.push_back(stoi(suicidal));
        }

        if (!championMastery.empty()) {
            int callingChampion = championMastery[0];
            for (int i = 0; i < championMastery.size() - 1; i++) {
                if (i == 0) {
                    //cout << "------------------:" << championMastery[0] << endl;
                }

                if (i > 0) {
                    championMaps[callingChampion][championMastery[i]].first = championMastery[i];
                    championMaps[callingChampion][championMastery[i]].second++;
                }
                //cout << championMastery[i] << endl;
                // cout << championMaps[callingChampion][championMastery[i]];
            }
        }
        //toReturn[summonerIDS[i]] = championMastery;
    }
    return championMaps;
}

void getSummonerIDS(vector<string>& summonerIDS) {
    string base = "https://na1.api.riotgames.com/lol/league/v4/entries/RANKED_SOLO_5x5/SILVER/I?page=";
    string apiEnd = "&api_key=" + RIOTAPIKEY;

    for (int i = 1; i < 2; i++) {
        auto pageInt = std::to_string(i);
        string final = base + pageInt + apiEnd;
        cpr::Response r = cpr::Get(cpr::Url{ final });
        getIDS(r.text, summonerIDS);
    }
}

void makeTextFile(vector<unordered_map<int, pair<int, int>>> inputMap) {
    //Each index in vector is for a champ
    //Each champ in vector is a map
    //Each map has other champion IDs and their compatability index.
    ofstream myfile;
    myfile.open("lol44_resources/data/summonerIDs.txt");

    for (int i = 1; i < inputMap.size(); i++) {
        if (inputMap[i].size() < 1) {
            continue;
        }
        myfile << i << ", ";
        //cout << "Calling Champion id: " << i << endl;
        for (int j = 0; j < 900; j++) {
            if (inputMap[i].count(j)) {
                myfile << inputMap[i][j].first << ", " << inputMap[i][j].second << ", ";
                //cout << "champion: " << inputMap[i][j].first << "relatability:" << inputMap[i][j].second << endl;
            }

        }

        myfile << endl;
    }
    myfile.close();
}

bool sortbysec(const pair<int, int>& a, const pair<int, int>& b) {
    return (a.second < b.second);
}

void sortRelation(vector<pair<int, int>>& inputVector) {
    pair<int, int> temp = inputVector[0];
    inputVector.erase(inputVector.begin());
    sort(inputVector.begin(), inputVector.end(), sortbysec);
    inputVector.push_back(temp);
    reverse(inputVector.begin(), inputVector.end());
}

vector<vector<pair<int, int>>> getData(string file, int championID) {
    ifstream inFile(file);
    string line;
    vector<vector<pair<int, int>>> fullGraph;

    while (getline(inFile, line)) {
        istringstream stream(line);
        string fullLine;
        getline(stream, fullLine);
        //cout << fullLine << endl;
        vector<string> ids;
        vector<pair<int, int>> relations;
        std::string delimiter = ",";
        stringstream ss(fullLine);

        while (ss.good()) {
            string substr;
            getline(ss, substr, ',');
            substr.erase(std::remove_if(substr.begin(), substr.end(), [](char c) { return !std::isdigit(c); }), substr.end());
            //cout << substr << endl;
            ids.push_back(substr);
        }

        if (stoi(ids[0]) == championID) {
            relations.push_back(make_pair(9999, championID));
            for (int i = 1; i < ids.size() - 1; i += 2) {
                relations.push_back(make_pair(stoi(ids[i]), stoi(ids[i + 1])));
            }
            sortRelation(relations);
        }

        int currentChamp = 0;
        for (int i = 0; i < relations.size(); i++) {
            if (i == 0) {
                currentChamp = relations[i].second;
            }

            if (i < 7 && i > 0) {
                currentChamp = relations[i].first;
            }

            if (i < 7) {
                std::vector<int> v = getMainExists(file, championID);
                int key = currentChamp;

                if (std::count(v.begin(), v.end(), key)) {
                    vector<pair<int, int>> sortedChamp = GetRelations(file, currentChamp, championID);
                    sortRelation(sortedChamp);
                    vector<pair<int, int>> realSortedChamp;
                    for (int j = 0; j < 8; j++) {        //CHANGE IF YOU WANT MORE ELEMENTS IN GRAPH
                        realSortedChamp.push_back(sortedChamp[j]);
                    }

                    fullGraph.push_back(realSortedChamp);
                }
            }
        }
    }
    //cout << "hi" << endl;
    return fullGraph;
}

// a structure to represent a weighted edge in graph
struct Edge {
    int src, dest, weight;
};

// a structure to represent a connected, directed and
// weighted graph
struct Graph {
    // V-> Number of vertices, E-> Number of edges
    int V = 0;
    int E = 0;

    // graph is represented as an array of edges.
    struct Edge* edge;
};

// Creates a graph with V vertices and E edges
struct Graph* createGraph(int V, int E) {
    struct Graph* graph = new Graph;
    graph->V = V;
    graph->E = E;
    graph->edge = new Edge[E];
    return graph;
}

vector < vector<pair<int, int>>> CutEdges(vector<vector<pair<int, int>>> inputData) {
    vector<vector<pair<int, int>>> toReturn;
    vector<int> MainRelations;

    for (int i = 1; i < inputData.size(); i++) {
        int MainRelation = inputData.at(i).at(0).second;
        MainRelations.push_back(MainRelation);
    }

    toReturn.push_back(inputData.at(0));

    //EXAMPLE 5 INPUT, Main Relations = 11, 24, 64, 104, 51, 141, 22
    for (int i = 1; i < inputData.size(); i++) {
        vector<pair<int, int>> newRelations;
        newRelations.push_back(inputData.at(i).at(0));
        for (int j = 1; j < inputData.at(i).size(); j++) {
            int key = inputData.at(i).at(j).first;
            if (std::count(MainRelations.begin(), MainRelations.end(), key)) {
                newRelations.push_back(make_pair(inputData.at(i).at(j).first, inputData.at(i).at(j).second));
            }

        }
        toReturn.push_back(newRelations);

    }

    int max = 0;
    for (int i = 0; i < toReturn.size(); i++) {
        for (int j = 1; j < toReturn.at(i).size(); j++) {
            if (toReturn.at(i).at(j).second > max) {
                max = toReturn.at(i).at(j).second;
            }
        }

    }

    for (int i = 0; i < toReturn.size(); i++) {
        for (int j = 1; j < toReturn.at(i).size(); j++) {
            toReturn.at(i).at(j).second = (int)((1 - (toReturn.at(i).at(j).second / ((double)max))) * 1000);
        }

    }

    return toReturn;
}

Graph* makeGraphFromFile(vector<vector<pair<int, int>>> inputData, int MainChampID, int champID) {
    int mainEdges = inputData.size();
    int additionalEdges = 0;
    for (int i = 1; i < inputData.size(); i++) {
        additionalEdges += (inputData.at(i).size() - 1);
    }

    //INPUT DATA HAS CONNECT VERTICES, ADD 1 FOR THE MAIN ONE
    int V = inputData.size();
    int E = mainEdges + additionalEdges;

    struct Graph* graph = createGraph(V, E);

    //MAKE THE EDGES FOR EACH NODE
    unordered_map<int, int> nodes;
    for (int i = 0; i < inputData.size(); i++) {

        if (i == 0) {
            nodes[inputData.at(0).at(i).second] = i;
        }
        else {
            nodes[inputData.at(0).at(i).first] = i;
        }
    }

    //MAIN EDGES FIRST
    for (int i = 0; i < inputData.size(); i++) {
        graph->edge[i].src = nodes[MainChampID];
        graph->edge[i].dest = nodes[inputData.at(0).at(i + 1).first];
        graph->edge[i].weight = inputData.at(0).at(i + 1).second;
        //cout << "EDGE NUMBER: " << i << endl;
        //cout << "Source: " << graph->edge[i].src << endl;
        //cout << "Destination: " << graph->edge[i].dest << endl;
        //cout << "Weight: " << graph->edge[i].weight << endl;
    }
    int edgeNumber = mainEdges;

    //OTHER EDGES NEXT
    for (int i = 0; i < inputData.size() - 1; i++) {
        for (int j = 0; j < inputData.at(i + 1).size() - 1; j++) {
            graph->edge[edgeNumber].src = nodes[inputData.at(i + 1).at(0).second];
            graph->edge[edgeNumber].dest = nodes[inputData.at(i + 1).at(j + 1).first];
            graph->edge[edgeNumber].weight = inputData.at(i + 1).at(j + 1).second;

          //  cout << "EDGE NUMBER: " << edgeNumber << endl;
           // cout << "Source: " << graph->edge[edgeNumber].src << endl;
           // cout << "Destination: " << graph->edge[edgeNumber].dest << endl;
           // cout << "Weight: " << graph->edge[edgeNumber].weight << endl;
            edgeNumber++;
        }
    }
    return graph;
}

// A utility function used to print the solution
void printArr(vector<int> dist, int n, int second) {
    printf("Vertex   Distance from Source\n");
    for (int i = 0; i < n; ++i) {
        //printf("%d \t\t %d\n", i, dist[i]);
        if (i == second) {
            cout << "-----------------" << dist[i] << "----------------" << endl;
        }
    }
}

int ReturnFinalFord(vector<int> dist, int n, int second) {
    //printf("Vertex   Distance from Source\n");
    for (int i = 0; i < n; ++i) {
        if (i == second) {
            return dist[i];
        }
    }
}

// The main function that finds shortest distances from src to
// all other vertices using Bellman-Ford algorithm.  The function
// also detects negative weight cycle
int BellmanFord(struct Graph* graph, int src, int champID){
    int V = graph->V;
    int E = graph->E;
    vector<int> dist;
    for (int i = 0; i < V; i++) {
        dist.push_back(0);
    }

    for (int i = 0; i < V; i++)
        dist[i] = INT_MAX;
    dist[src] = 0;

    for (int i = 1; i <= V - 1; i++) {
        for (int j = 0; j < E; j++) {
            int u = graph->edge[j].src;
            int v = graph->edge[j].dest;
            int weight = graph->edge[j].weight;
            if (dist[u] != INT_MAX && dist[u] + weight < dist[v])
                dist[v] = dist[u] + weight;
        }
    }

    //check for negative-weight cycles
    for (int i = 0; i < E; i++) {
        int u = graph->edge[i].src;
        int v = graph->edge[i].dest;
        int weight = graph->edge[i].weight;
        if (dist[u] != INT_MAX && dist[u] + weight < dist[v]) {
            //printf("Graph contains negative weight cycle");
            return -1;
            // If negative cycle is detected, simply return
        }
    }

    //printArr(dist, V, champID);
    return ReturnFinalFord(dist, V, champID);
}

int FordFinal(vector<vector<pair<int, int>>> graphInput, string file, int MainChampID, int champ1, int champ2) {
    vector<vector<pair<int, int>>> myGraph;
    myGraph = getData(file, MainChampID);
    myGraph = CutEdges(myGraph);
    struct Graph* newGraph = makeGraphFromFile(myGraph, MainChampID, champ1);
    int distance = BellmanFord(newGraph, champ1, champ2);
    //cout << "FINAL:                           " << distance << endl;
    return distance;
}

//divide int by 1000 which is percent related, if  > 100,000 its not related, nto connected
int minDistance(int dist[], bool sptSet[]){
    // Initialize min value
    int min = INT_MAX, min_index;

    for (int v = 0; v < 7; v++)
        if (sptSet[v] == false && dist[v] <= min)
            min = dist[v], min_index = v;

    return min_index;
}

// A utility function to print the constructed distance array
int printSolution(int dist[], int final){
    for (int i = 0; i < 7; i++) {
        if (i == final) {
            return dist[i];
        }
        
    }
}

// Function that implements Dijkstra's single source shortest path algorithm
// for a graph represented using adjacency matrix representation
int dijkstra(int graph[7][7], int src, int final){
    int dist[7]; // The output array.  dist[i] will hold the shortest
    // distance from src to i

    bool sptSet[7]; // sptSet[i] will be true if vertex i is included in shortest
    // path tree or shortest distance from src to i is finalized

    // Initialize all distances as INFINITE and stpSet[] as false
    for (int i = 0; i < 7; i++)
        dist[i] = INT_MAX, sptSet[i] = false;

    // Distance of source vertex from itself is always 0
    dist[src] = 0;

    // Find shortest path for all vertices
    for (int count = 0; count < 7 - 1; count++) {
        // Pick the minimum distance vertex from the set of vertices not
        // yet processed. u is always equal to src in the first iteration.
        int u = minDistance(dist, sptSet);

        // Mark the picked vertex as processed
        sptSet[u] = true;

        // Update dist value of the adjacent vertices of the picked vertex.
        for (int v = 0; v < 7; v++)

            // Update dist[v] only if is not in sptSet, there is an edge from
            // u to v, and total weight of path from src to  v through u is
            // smaller than current value of dist[v]
            if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX
                && dist[u] + graph[u][v] < dist[v])
                dist[v] = dist[u] + graph[u][v];
    }

    // print the constructed distance array
    return printSolution(dist, final);
}

// driver program to test above function
int djikstraMake(vector<vector<pair<int, int>>> graphInput, string file, int MainChampID, int champ1, int champ2) {
    unordered_map<int, int> nodes;
    for (int i = 0; i < graphInput.size(); i++) {
        if (i == 0) {
            nodes[graphInput.at(0).at(i).second] = i;
        }
        else {
            nodes[graphInput.at(0).at(i).first] = i;
        }
    }

    for (int i = 1; i < graphInput.size(); i++) {

        for (int j = 0; j < graphInput.at(i).size(); j++) {

            if (graphInput.at(i).at(j).second == 0) {
                graphInput.at(i).at(j).second = 1;
            }
        }
    }

    vector<vector<int>> graph;
    //First Row Exception
    vector<int> middleRow;
    middleRow.push_back(0);
    for (int i = 1; i < graphInput.at(0).size() - 1; i++) {
        //int test = graphInput.at(0).at(j).second;
        middleRow.push_back(graphInput.at(0).at(i).second);
    }
    graph.push_back(middleRow);          //ADD first List

    for (int i = 1; i < graphInput.size(); i++) {
        // 6 times, once per node to add a line
        middleRow = { 0,0, 0, 0, 0, 0, 0, };
        for (int j = 1; j < graphInput.at(i).size(); j++) {
            int nodeNumber = nodes[graphInput.at(i).at(j).first];
            int weight = graphInput.at(i).at(j).second;
            middleRow.at(nodeNumber) = weight;
        }
        graph.push_back(middleRow);
    }

    int input[7][7];
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            input[i][j] = graph.at(i).at(j);
        }

    }

    return dijkstra(input, 2, champ2);
}

void TextFileLiz(string file, int championID) {
    ifstream inFile(file);
    string line;
    vector<int> LizInts;

    while (getline(inFile, line)) {
        istringstream stream(line);
        string fullLine;
        getline(stream, fullLine);
        //cout << fullLine << endl;
        vector<string> ids;
        vector<pair<int, int>> relations;
        std::string delimiter = ",";
        stringstream ss(fullLine);

        while (ss.good()) {
            string substr;
            getline(ss, substr, ',');
            substr.erase(std::remove_if(substr.begin(), substr.end(), [](char c) { return !std::isdigit(c); }), substr.end());
            //cout << substr << endl;
            ids.push_back(substr);
        }

        if (stoi(ids[0]) == championID) {
            relations.push_back(make_pair(9999, championID));
            for (int i = 1; i < ids.size() - 1; i += 2) {
                relations.push_back(make_pair(stoi(ids[i]), stoi(ids[i + 1])));
            }
            sortRelation(relations);
        }
        vector<int> topRelations;
        int currentChamp = 0;
        for (int i = 0; i < relations.size(); i++) {

            if (i < 8) {
                currentChamp = relations[i].first;
            }

            LizInts.push_back(currentChamp);
        }
    }

    ofstream myfile;
    myfile.open("lol44_resources/menu_input/vector.txt");
    for (int i = 1; i < 8; i++) {
        myfile << LizInts[i] << ", ";
    }

    myfile.close();
}

int whatChamp1(string name) {
    if (name == "Astrox") {
        return 266;
    }
    else if (name == "Ahri") {
        return 103;
    }
    else if (name == "Akali") {
        return 84;
    }
    else if (name == "Akshan") {
        return 166;
    }
    else if (name == "Alistar") {
        return 12;
    }
    else if (name == "Amumu") {
        return 32;
    }
    else if (name == "Anivia") {
        return 34;
    }
    else if (name == "Annie") {
        return 1;
    }
    else if (name == "Aphelios") {
        return 523;
    }
    else if (name == "Ashe") {
        return 22;
    }
    else if (name == "Aurelion Sol") {
        return 136;
    }
    else if (name == "Azir") {
        return 268;
    }
    else if (name == "Bard") {
        return 432;
    }
    else if (name == "Blitzcrank") {
        return 53;
    }
    else if (name == "Brand") {
        return 63;
    }
    else if (name == "Braum") {
        return 201;
    }
    else if (name == "Caitlyn") {
        return 51;
    }
    else if (name == "Camille") {
        return 164;
    }
    else if (name == "Cassiopeia") {
        return 69;
    }
    else if (name == "Cho'Gath") {
        return 31;
    }
    else if (name == "Corki") {
        return 42;
    }
    else if (name == "Darius") {
        return 122;
    }
    else if (name == "Diana") {
        return 131;
    }
    else if (name == "Draven") {
        return 119;
    }
    else if (name == "Dr. Mundo") {
        return 36;
    }
    else if (name == "Ekko") {
        return 245;
    }
    else if (name == "Elise") {
        return 60;
    }
    else if (name == "Evelynn") {
        return 28;
    }
    else if (name == "Ezreal") {
        return 81;
    }
    else if (name == "Fiddlesticks") {
        return 9;
    }
    else if (name == "Fiora") {
        return 114;
    }
    else if (name == "Fizz") {
        return 105;
    }
    else if (name == "Galio") {
        return 3;
    }
    else if (name == "Gangplank") {
        return 41;
    }
    else if (name == "Garen") {
        return 86;
    }
    else if (name == "Gnar") {
        return 150;
    }
    else if (name == "Gragas") {
        return 79;
    }
    else if (name == "Graves") {
        return 104;
    }
    else if (name == "Gwen") {
        return 887;
    }
    else if (name == "Hecarim") {
        return 120;
    }
    else if (name == "Heimerdinger") {
        return 74;
    }
    else if (name == "Illaoi") {
        return 420;
    }
    else if (name == "Irelia") {
        return 39;
    }
    else if (name == "Ivern") {
        return 427;
    }
    else if (name == "Janna") {
        return 40;
    }
    else if (name == "Jarvan IV") {
        return 59;
    }
    else if (name == "Jax") {
        return 24;
    }
    else if (name == "Jayce") {
        return 126;
    }
    else if (name == "Jhin") {
        return 202;
    }
    else if (name == "Jinx") {
        return 222;
    }
    else if (name == "Kai'Sa") {
        return 145;
    }
    else if (name == "Kalista") {
        return 429;
    }
    else if (name == "Karma") {
        return 43;
    }
    else if (name == "Karthus") {
        return 30;
    }
    else if (name == "Kassadin") {
        return 38;
    }
    else if (name == "Katarina") {
        return 55;
    }
    else if (name == "Kayle") {
        return 10;
    }
    else if (name == "Kayn") {
        return 141;
    }
    else if (name == "Kennen") {
        return 85;
    }
    else if (name == "Kha'Zix") {
        return 121;
    }
    else if (name == "Kindered") {
        return 203;
    }
    else if (name == "Kled") {
        return 240;
    }
    else if (name == "Kog'Maw") {
        return 96;
    }
    else if (name == "LeBlanc") {
        return 7;
    }
    else if (name == "Lee Sin") {
        return 64;
    }
    else if (name == "Leona") {
        return 89;
    }
    else if (name == "Lillia") {
        return 876;
    }
    else if (name == "Lissandra") {
        return 127;
    }
    else if (name == "Lucian") {
        return 236;
    }
    else if (name == "Lulu") {
        return 117;
    }
    else if (name == "Lux") {
        return 99;
    }
    else if (name == "Malphite") {
        return 54;
    }
    else if (name == "Malzahar") {
        return 90;
    }
    else if (name == "Maokai") {
        return 57;
    }
    else if (name == "Master Yi") {
        return 11;
    }
    else if (name == "Miss Fortunate") {
        return 21;
    }
    else if (name == "Wukong") {
        return 62;
    }
    else if (name == "Mordekaiser") {
        return 82;
    }
    else if (name == "Morgana") {
        return 25;
    }
    else {
        return 0;
    }

}

int whatChamp2(string name) {
    if (name == "Nami") {
        return 267;
    }
    else if (name == "Nasus") {
        return 75;
    }
    else if (name == "Nautilus") {
        return 111;
    }
    else if (name == "Neeko") {
        return 518;
    }
    else if (name == "Nidalee") {
        return 76;
    }
    else if (name == "Nocturne") {
        return 56;
    }
    else if (name == "Nunu & Willump") {
        return 20;
    }
    else if (name == "Olaf") {
        return 2;
    }
    else if (name == "Orianna") {
        return 61;
    }
    else if (name == "Ornn") {
        return 516;
    }
    else if (name == "Pantheon") {
        return 80;
    }
    else if (name == "Poppy") {
        return 78;
    }
    else if (name == "Pyke") {
        return 555;
    }
    else if (name == "Qiyana") {
        return 246;
    }
    else if (name == "Quinn") {
        return 133;
    }
    else if (name == "Rakan") {
        return 497;
    }
    else if (name == "Rammus") {
        return 33;
    }
    else if (name == "Rek'Sai") {
        return 421;
    }
    else if (name == "Rell") {
        return 526;
    }
    else if (name == "Renekton") {
        return 58;
    }
    else if (name == "Rengar") {
        return 107;
    }
    else if (name == "Riven") {
        return 92;
    }
    else if (name == "Rumble") {
        return 68;
    }
    else if (name == "Ryze") {
        return 13;
    }
    else if (name == "Samira") {
        return 360;
    }
    else if (name == "Sejuani") {
        return 113;
    }
    else if (name == "Senna") {
        return 235;
    }
    else if (name == "Seraphine") {
        return 147;
    }
    else if (name == "Sett") {
        return 875;
    }
    else if (name == "Shaco") {
        return 35;
    }
    else if (name == "Shen") {
        return 98;
    }
    else if (name == "Shyvana") {
        return 102;
    }
    else if (name == "Singed") {
        return 27;
    }
    else if (name == "Sion") {
        return 14;
    }
    else if (name == "Sivir") {
        return 15;
    }
    else if (name == "Skarner") {
        return 72;
    }
    else if (name == "Sona") {
        return 37;
    }
    else if (name == "Soraka") {
        return 16;
    }
    else if (name == "Swain") {
        return 50;
    }
    else if (name == "Sylas") {
        return 517;
    }
    else if (name == "Syndra") {
        return 134;
    }
    else if (name == "Tahm Kench") {
        return 223;
    }
    else if (name == "Taliyah") {
        return 163;
    }
    else if (name == "Talon") {
        return 91;
    }
    else if (name == "Taric") {
        return 44;
    }
    else if (name == "Teemo") {
        return 17;
    }
    else if (name == "Thresh") {
        return 412;
    }
    else if (name == "Tristana") {
        return 18;
    }
    else if (name == "Trundle") {
        return 48;
    }
    else if (name == "Tryndamere") {
        return 23;
    }
    else if (name == "Twisted Fate") {
        return 4;
    }
    else if (name == "Twitch") {
        return 29;
    }
    else if (name == "Udyr") {
        return 77;
    }
    else if (name == "Urgot") {
        return 6;
    }
    else if (name == "Varus") {
        return 110;
    }
    else if (name == "Vayne") {
        return 67;
    }
    else if (name == "Veigar") {
        return 45;
    }
    else if (name == "Vel'Koz") {
        return 161;
    }
    else if (name == "Vex") {
        return 711;
    }
    else if (name == "Vi") {
        return 254;
    }
    else if (name == "Viego") {
        return 234;
    }
    else if (name == "Viktor") {
        return 112;
    }
    else if (name == "Vladimir") {
        return 8;
    }
    else if (name == "Volibear") {
        return 106;
    }
    else if (name == "Warwick") {
        return 19;
    }
    else if (name == "Xayah") {
        return 498;
    }
    else if (name == "Xerath") {
        return 101;
    }
    else if (name == "Xin Zhao") {
        return 5;
    }
    else if (name == "Yasuo") {
        return 157;
    }
    else if (name == "Yone") {
        return 777;
    }
    else if (name == "Yorick") {
        return 83;
    }
    else if (name == "Yuumi") {
        return 350;
    }
    else if (name == "Zac") {
        return 154;
    }
    else if (name == "Zed") {
        return 238;
    }
    else if (name == "Ziggs") {
        return 115;
    }
    else if (name == "Zilean") {
        return 26;
    }
    else if (name == "Zoe") {
        return 142;
    }
    else if (name == "Zyra") {
        return 143;
    }
    else {
        return 0;
    }
}

void FordTextFile(vector<vector<pair<int, int>>> graphInput, string file, int MainChampID, int champ1, int champ2) {

    auto t1 = Clock::now();
    int distance = FordFinal(graphInput, "lol44_resources/data/100SUMMONERS.txt", MainChampID, champ1, champ2);
    auto t2 = Clock::now();
    float distanceUpdate;
    string pushFinal = "";
    string pushFinalTime = "";


    //TIME
    float timeTaken = duration_cast<nanoseconds>(t2 - t1).count();
    timeTaken = timeTaken / 1000000;
    
    timeTaken = (int)(timeTaken * 100 + .5);
    timeTaken = (float)timeTaken / 100;
    string num_str2(std::to_string(timeTaken));
    if (num_str2.find('.') != std::string::npos)
    {
        // Remove trailing zeroes
        num_str2 = num_str2.substr(0, num_str2.find_last_not_of('0') + 1);
        // If the decimal point is now the last character, remove that as well
        if (num_str2.find('.') == num_str2.size() - 1)
        {
            num_str2 = num_str2.substr(0, num_str2.size() - 1);
        }
    }

    pushFinalTime += "Time Taken For Ford: " + num_str2 + " microseconds";


    //DISTANCE
    if (distance > 100000) {
        pushFinal = "N/A";
    }
    distanceUpdate = (float)distance / 1000.0f;
    distanceUpdate = (1 - distanceUpdate) * 100;
    string num_str1(std::to_string(distanceUpdate));
    pushFinal += "Relation Through Shortest Path: " + num_str1 + "%";

    ifstream inFile(file);
    string line;
    vector<int> LizInts;


    while (getline(inFile, line)) {
        istringstream stream(line);
        string fullLine;
        getline(stream, fullLine);
        ofstream myfile;
        myfile.open("lol44_resources/menu_input/BellmanFord.txt");


        myfile << pushFinal << endl;
        myfile << pushFinalTime << endl;
        myfile << "";


        myfile.close();


    }
}

void DTextFile(vector<vector<pair<int, int>>> graphInput, string file, int MainChampID, int champ1, int champ2) {
    
    auto t1 = Clock::now();
    int distance = djikstraMake(graphInput, "lol44_resources/data/100SUMMONERS.txt", MainChampID, champ1, champ2);
    auto t2 = Clock::now();
    float distanceUpdate;
    string pushFinal = "";
    string pushFinalTime = "";


    //TIME
    float timeTaken = duration_cast<nanoseconds>(t2 - t1).count();
    timeTaken = timeTaken / 1000000;

    timeTaken = (int)(timeTaken * 100 + .5);
    timeTaken = (float)timeTaken / 100;
    string num_str2(std::to_string(timeTaken));
    if (num_str2.find('.') != std::string::npos)
    {
        // Remove trailing zeroes
        num_str2 = num_str2.substr(0, num_str2.find_last_not_of('0') + 1);
        // If the decimal point is now the last character, remove that as well
        if (num_str2.find('.') == num_str2.size() - 1)
        {
            num_str2 = num_str2.substr(0, num_str2.size() - 1);
        }
    }

    pushFinalTime += "Time Taken For Dijkstra: " + num_str2 + " microseconds";


    //DISTANCE
    if (distance > 100000) {
        pushFinal = "N/A";
    }
    distanceUpdate = (float)distance / 1000.0f;
    distanceUpdate = (1 - distanceUpdate) * 100;
    string num_str1(std::to_string(distanceUpdate));
    pushFinal += "Relation Through Shortest Path: " + num_str1 + "%";

    ifstream inFile(file);
    string line;
    vector<int> LizInts;


    while (getline(inFile, line)) {
        istringstream stream(line);
        string fullLine;
        getline(stream, fullLine);
        ofstream myfile;
        myfile.open("lol44_resources/menu_input/Dijkstra.txt");


        myfile << pushFinal << endl;
        myfile << pushFinalTime << endl;
        myfile << "";


        myfile.close();


    }

}

int main(int argc, char** argv) {

    while (true) {
        //get champion + build graph based off it 
        std::ifstream input("lol44_resources/menu_output/champion.txt");
        std::stringstream buffer;
        buffer << input.rdbuf();
        std::string srcName = buffer.str();
        int src = whatChamp1(srcName);
        if (src == 0) {
            src = whatChamp2(srcName);
        }

        //build graph
        vector<vector<pair<int, int>>> myGraph = getData("lol44_resources/data/100SUMMONERS.txt", src);
        vector<vector<pair<int, int>>> CuteEdges = CutEdges(myGraph);
        //Define CharacterID to Node Number Conversion
        unordered_map<int, int> nodes;
        for (int i = 0; i < CuteEdges.size(); i++) {
            if (i == 0) {
                nodes[CuteEdges.at(0).at(i).second] = i;
            }
            else {
                nodes[CuteEdges.at(0).at(i).first] = i;
            }
        }

        //build vector based off graph
        TextFileLiz("lol44_resources/data/100SUMMONERS.txt", src);
        cout << "loaded champions" << endl;

        //get input from the ******text files******* from menu
        std::ifstream input2("lol44_resources/menu_output/champion2.txt");
        std::stringstream buffer2;
        buffer2 << input2.rdbuf();
        std::string name2 = buffer2.str();
        int champID2 = whatChamp1(name2);
        if (champID2 == 0) {
            champID2 = whatChamp2(name2);
        }

        std::ifstream input3("lol44_resources/menu_output/champion3.txt");
        std::stringstream buffer3;
        buffer3 << input3.rdbuf();
        std::string name3 = buffer3.str();
        int champID3 = whatChamp1(name3);
        if (champID3 == 0) {
            champID3 = whatChamp2(name3);
        }
        int champ2 = nodes[champID2];
        int champ3 = nodes[champID3];

        //djikstra call 
        // TODO:: output to text file
        DTextFile(CuteEdges, "lol44_resources/data/100SUMMONERS.txt", src, champ2, champ3);
        cout << "loaded dijkstra" << endl;

        //bellman ford call
        //TODO:: output to text file
        FordTextFile(CuteEdges, "lol44_resources/data/100SUMMONERS.txt", src, champ2, champ3);
        cout << "loaded ford" << endl;

        //test api
        vector<string> summonerIDS;
        getSummonerIDS(summonerIDS);
        vector<unordered_map<int, pair<int, int>>> champData = getChampData(summonerIDS);
        makeTextFile(champData);
        cout << "loaded api test pull" << endl;
    }
}
