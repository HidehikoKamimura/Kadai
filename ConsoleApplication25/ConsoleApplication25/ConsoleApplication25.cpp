#include <iostream>
#include <list>

#define _SLEEP  0
#define _AWAKE  1
#define _FIX    2

struct _Node {
    _Node();
    ~_Node();
    unsigned long m_nAdress[3]; // up to 3D.
    unsigned long m_nScore;
    unsigned long m_nWeight;
    _Node* m_ppNext[4];

    int m_Status;
};

_Node::_Node()
{
    m_nScore = 0UL;
    m_Status = _SLEEP;
    memset(m_nAdress, 0x00, sizeof(unsigned long) * 3);
    memset(m_ppNext, 0x00, sizeof(_Node*) * 4);

    m_nWeight = 1;
}

_Node::~_Node()
{
}

struct _Path {
    _Path(_Path* ipParent, _Node* ipNode, unsigned long inThresHold);
    ~_Path();
    bool Acceptable(_Node* ipNextNode, unsigned long inThresHold);
    unsigned long NbLast(_Node* ipLastNode, std::list<_Path*>& orLastPath);
    unsigned long GetLast(_Node* ipLastNode, std::list<_Node*>*& oprNodeList);

    _Path* m_pParent;

    _Node* m_pNode;
    unsigned long m_nScore;
    _Path** m_ppNext;
    _Node** m_ppNodeNext;
    unsigned long m_nNode;
};

_Path::_Path(_Path* ipParent, _Node* ipNode, unsigned long inThresHold)
{
    m_pParent = ipParent;

    if (ipParent == nullptr)
        // m_nScore = -1L + ipNode->m_nWeight;
        m_nScore = 0UL;
    else
        m_nScore = ipParent->m_nScore + ipNode->m_nWeight;

    m_pNode = ipNode;

    m_nNode = 0UL;
    m_ppNext = nullptr;
    m_ppNodeNext = nullptr;
    
    for (unsigned int i = 0U; i < 4; i++)
        if (ipNode->m_ppNext[i] != nullptr)
            if( Acceptable(ipNode->m_ppNext[i], inThresHold) == true ) m_nNode++;
    
    if (m_nNode == 0UL) return;

    m_ppNext = new _Path*[m_nNode];
    m_ppNodeNext = new _Node*[m_nNode];

    memset(m_ppNext, 0x00, sizeof(_Path*) * m_nNode);
    memset(m_ppNodeNext, 0x00, sizeof(_Node*) * m_nNode);

    unsigned long cnt = 0UL;
    for (unsigned int i = 0U; i < 4; i++)
        if (ipNode->m_ppNext[i] != nullptr)
            if (Acceptable(ipNode->m_ppNext[i], inThresHold) == true) m_ppNodeNext[cnt++] = m_pNode->m_ppNext[i];

    // m_pAlive = nullptr;
}

_Path::~_Path()
{
    if( m_ppNext != nullptr ) delete[] m_ppNext;
}

bool _Path::Acceptable(_Node* ipNextNode, unsigned long inThresHold)
{
    unsigned long _score = m_nScore + ipNextNode->m_nWeight;
    if (_score > ipNextNode->m_nScore || _score > inThresHold) return false;

    return true;
}

unsigned long _Path::NbLast(_Node* ipLastNode, std::list<_Path*>& orLastPath)
{
    unsigned long nLast = 0UL;

    if (m_pNode->m_nAdress[0] == ipLastNode->m_nAdress[0] &&
        m_pNode->m_nAdress[1] == ipLastNode->m_nAdress[1] &&
        m_pNode->m_nAdress[2] == ipLastNode->m_nAdress[2] ) {
        if (m_nNode != 0UL) {
            printf("unconsiderable error...\n");
            return 0UL;
        }
        orLastPath.push_back(this);
        return 1UL;
    }
    for (unsigned long i = 0UL; i < m_nNode; i++) {
        nLast += m_ppNext[i]->NbLast(ipLastNode, orLastPath);
    }
    return nLast;
}

unsigned long _Path::GetLast(_Node* ipLastNode, std::list<_Node*>*& oprNodeList)
{
    std::list<_Path*> lstLastPath;
    unsigned long nPath = NbLast(ipLastNode, lstLastPath);
    if (nPath == 0UL) {
        oprNodeList = nullptr;
        return 0UL;
    }

    oprNodeList = new std::list<_Node*>[nPath];

    nPath = 0UL;
    std::list<_Path*>::iterator itr;
    for (itr = lstLastPath.begin(); itr != lstLastPath.end(); itr++) {
        _Path* _p = *itr;
        while (_p != nullptr) {
            oprNodeList[nPath].push_front(_p->m_pNode);
            _p = _p->m_pParent;
        }


        nPath++;

    }

    return nPath;
}

#define _GRID_SZ 10

unsigned long WakeupAround(const _Node* ipNode, unsigned long iCurScore)
{
    unsigned long nCount = 0UL;
    for (unsigned long i = 0UL; i < 4; i++) {
        if (ipNode->m_ppNext[i] != nullptr && ipNode->m_ppNext[i]->m_Status != _FIX) {
            ipNode->m_ppNext[i]->m_Status = _AWAKE;

            unsigned long _score = iCurScore + ipNode->m_ppNext[i]->m_nWeight;
            if (ipNode->m_ppNext[i]->m_nScore == 0 || ipNode->m_ppNext[i]->m_nScore > _score)
                ipNode->m_ppNext[i]->m_nScore = _score;
            ++nCount;
        }
    }
    return nCount;
}

_Node* PickupAwakeNode(_Node* ipNode)
{
    unsigned long idx = 0UL;

    unsigned long score_min = 0UL;
    unsigned long idx_min = 0UL;
    for (unsigned long i = 0UL; i < _GRID_SZ; i++) {
        for (unsigned long j = 0UL; j < _GRID_SZ; j++) {
            if (ipNode[idx].m_Status == _AWAKE) {
                if (score_min == 0UL || ipNode[idx].m_nScore < score_min) {
                    score_min = ipNode[idx].m_nScore;
                    idx_min = idx;
                }
            }
            ++idx;
        }
    }

    if (score_min == 0UL) return nullptr;

    return ipNode + idx_min;
}

bool IsSame(_Node* ipNode1, _Node* ipNode2)
{
    if (ipNode1->m_nAdress[0] == ipNode2->m_nAdress[0] &&
        ipNode1->m_nAdress[1] == ipNode2->m_nAdress[1] &&
        ipNode1->m_nAdress[2] == ipNode2->m_nAdress[2]) return true;
    return false;
}

void SearchPath(_Path* ipPath, unsigned long inThresHold)
{
    for (unsigned long i = 0UL; i < ipPath->m_nNode; i++) {
        ipPath->m_ppNext[i] = new _Path(ipPath, ipPath->m_ppNodeNext[i], inThresHold);
        SearchPath(ipPath->m_ppNext[i], inThresHold);
    }
}

void PrintPath(_Path* ipPath, unsigned long inIndent)
{
    for (unsigned long i = 0UL; i < inIndent; i++) printf("\t");
    printf("[%lu] (%lu, %lu)\n", ipPath->m_nScore, ipPath->m_pNode->m_nAdress[0], ipPath->m_pNode->m_nAdress[1]);

    for (unsigned long i = 0UL; i < ipPath->m_nNode; i++) {
        PrintPath(ipPath->m_ppNext[i], inIndent + 1);
    }
}

bool Dijkstra(_Node* ipNode, _Node* ipStart, _Node* ipEnd, _Node** ippObstacle, unsigned long inObstacle)
{
    unsigned long idx = 0UL;

    /*
     * start, end nodes
     */
    const char* pStartEndFilPath = "C:\\Users\\hopst\\GitHub\\Kadai\\2024_08\\12\\start_end.txt";
    FILE* pStartEndFile = nullptr;
    errno_t err = fopen_s(&pStartEndFile, pStartEndFilPath, "w");

    fprintf(pStartEndFile, "%lu,%lu\t", ipStart->m_nAdress[0], ipStart->m_nAdress[1]);
    fprintf(pStartEndFile, "%lu,%lu\t", ipEnd->m_nAdress[0], ipEnd->m_nAdress[1]);
    fprintf(pStartEndFile, "\n");
    fclose(pStartEndFile);

    /*
     * output obstacles.
     */
    const char* pObstacleFilPath = "C:\\Users\\hopst\\GitHub\\Kadai\\2024_08\\12\\obst.txt";
    FILE* pObstacleFile = nullptr;
    err = fopen_s(&pObstacleFile, pObstacleFilPath, "w");

    for (unsigned long i = 0UL; i < inObstacle; i++) {
        fprintf(pObstacleFile, "%lu,%lu\t", ippObstacle[i]->m_nAdress[0], ippObstacle[i]->m_nAdress[1]);
    }
    fprintf(pObstacleFile, "\n");
    fclose(pObstacleFile);

    /*
     * output nodes with weight = 1 ~ 3.
     */
    for (int w = 1; w <= 3; w++) {
        char pWeightFilPath[256];
        sprintf_s(pWeightFilPath, 256, "C:\\Users\\hopst\\GitHub\\Kadai\\2024_08\\12\\w%d.txt", w);

        FILE* pWeightFile = nullptr;
        err = fopen_s(&pWeightFile, pWeightFilPath, "w");
        idx = 0UL;
        for (unsigned long i = 0UL; i < _GRID_SZ; i++) {
            for (unsigned long j = 0UL; j < _GRID_SZ; j++) {
                if (ipNode[idx].m_nWeight == w) {
                    bool bObst = false;
                    for (unsigned long k = 0UL; k < inObstacle; k++) {
                        if (IsSame(ipNode + idx, ippObstacle[k]) == true) {
                            bObst = true;
                            break;
                        }
                    }
                    if (bObst == false) fprintf_s(pWeightFile, "%lu,%lu\t", ipNode[idx].m_nAdress[0], ipNode[idx].m_nAdress[1]);
                }
                ++idx;
            }
        }
        fprintf_s(pWeightFile, "\n");
        fclose(pWeightFile);
    }

    _Node* _pNode = ipStart;
    _pNode->m_Status = _FIX;
    unsigned long _score = 0;

    while (_pNode != nullptr && IsSame(_pNode, ipEnd) == false) {
        WakeupAround(_pNode, _score);
        _pNode = PickupAwakeNode(ipNode);

        _pNode->m_Status = _FIX;
        _score = _pNode->m_nScore;
    }

    idx = 0UL;

    printf("[score]\n");
    for (unsigned long i = 0UL; i < _GRID_SZ; i++) {
        for (unsigned long j = 0UL; j < _GRID_SZ; j++) {
            printf("%lu\t", ipNode[idx].m_nScore);
            ++idx;
        }
        printf("\n");
    }

    if (ipEnd->m_nScore != 0)
        printf("shortest path : %lu\n", ipEnd->m_nScore);
    else
        printf("error...\n");

    _Path p(nullptr, ipStart, ipEnd->m_nScore);
    SearchPath(&p, ipEnd->m_nScore);
    // PrintPath(&p, 0UL);

    /*
     * output routes
     */
    std::list<_Node*>* plstNode = nullptr;
    unsigned long nlstNode = p.GetLast(ipEnd, plstNode);

    const char* pRouteFilPath = "C:\\Users\\hopst\\GitHub\\Kadai\\2024_08\\12\\path.txt";
    FILE* pRouteFile = nullptr;
    err = fopen_s(&pRouteFile, pRouteFilPath, "w");

    for (unsigned long i = 0UL; i < nlstNode; i++) {
        std::list<_Node*>::iterator itr;
        for (itr = plstNode[i].begin(); itr != plstNode[i].end(); itr++) {
            _Node* pNode = *itr;

            fprintf(pRouteFile, "%lu,%lu\t", pNode->m_nAdress[0], pNode->m_nAdress[1]);
        }
        fprintf(pRouteFile, "\n");
    }

    fclose(pRouteFile);
    return true;
}

int main()
{
    unsigned long pLocObstacle[][2] = {
        {0, 4},
        {0, 5},
        {0, 6},
        {0, 7},
        {0, 8},
        {0, 9},
        {1, 5},
        {1, 6},
        {1, 7},
        {1, 8},
        {1, 9},
        {2, 6},
        {2, 7},
        {2, 8},
        {2, 9},
        {3, 6},
        {3, 7},
        {3, 8},
        {3, 9},
        {6, 1},
        {6, 2},
        {7, 1},
        {7, 2},
        {7, 3},
        {8, 1},
        {8, 2},
        {8, 3},
    };

    unsigned long pLocWeight3[][2] = {
        {0, 3},
        {1, 4},
        {2, 5},
        {3, 5},
        {4, 6},
        {4, 7},
        {4, 8},
        {4, 9},
        {5, 1},
        {5, 2},
        {6, 3},
        {7, 3},
        {8, 3},
        {9, 1},
        {9, 2},
        {9, 3},
        {7, 4},
        {8, 4},
        {6, 0},
        {7, 0},
        {8, 0},
    };

    unsigned long pLocWeight2[][2] = {
        {6, 6},
        {6, 7},
        {7, 6},
        {7, 7},
        {2, 1},
        {2, 2},
        {3, 1},
        {3, 2},
        {4, 1},
        {4, 2},
    };

    _Node* pGraph = new _Node[_GRID_SZ * _GRID_SZ];
    
    /*
     * initial layout
     */
    unsigned long idx = 0UL;
    for (unsigned long i = 0UL; i < _GRID_SZ; i++) {
        for (unsigned long j = 0UL; j < _GRID_SZ; j++) {
            pGraph[idx].m_nAdress[0] = i;
            pGraph[idx].m_nAdress[1] = j;
            if( i != 0UL ) {
                pGraph[idx].m_ppNext[3] = pGraph + idx - _GRID_SZ;
            }
            if (i != _GRID_SZ - 1) {
                pGraph[idx].m_ppNext[1] = pGraph + idx + _GRID_SZ;
            }
            if (j != 0UL) {
                pGraph[idx].m_ppNext[2] = pGraph + idx - 1;
            }
            if (j != _GRID_SZ - 1) {
                pGraph[idx].m_ppNext[0] = pGraph + idx + 1;
            }
            ++idx;
        }
    }

    _Node* pStart = nullptr;
    _Node* pEnd = nullptr;

    /*
     * start, end location.
     */
    idx = 0UL;
    for (unsigned long i = 0UL; i < _GRID_SZ; i++) {
        for (unsigned long j = 0UL; j < _GRID_SZ; j++) {
            if (i == 0UL && j == 0UL) pStart = pGraph + idx;
            if (i == 9UL && j == 9UL) pEnd = pGraph + idx;
            idx++;
        }
    }

    /*
     * obstacle location.
     */
    unsigned long nObstacle = sizeof(pLocObstacle) / (sizeof(unsigned long) * 2);
    _Node** ppObstacle = new _Node * [nObstacle];

    idx = 0UL;
    unsigned long obs_idx = 0UL;
    for (unsigned long i = 0UL; i < _GRID_SZ; i++) {
        for (unsigned long j = 0UL; j < _GRID_SZ; j++) {
            for (unsigned long k = 0UL; k < nObstacle; k++) {
                if (pGraph[idx].m_nAdress[0] == pLocObstacle[k][0] && pGraph[idx].m_nAdress[1] == pLocObstacle[k][1]) {
                    ppObstacle[obs_idx++] = pGraph + idx;
                    break;
                }
            }
            idx++;
        }
    }

    idx = 0UL;
    for (unsigned long i = 0UL; i < _GRID_SZ; i++) {
        for (unsigned long j = 0UL; j < _GRID_SZ; j++) {
            for (unsigned long k = 0UL; k < 4; k++) {
                if (pGraph[idx].m_ppNext[k] != nullptr) {
                    for (unsigned long _idx = 0UL; _idx < 6; _idx++) {
                        if (pGraph[idx].m_ppNext[k] == ppObstacle[_idx]) pGraph[idx].m_ppNext[k] = nullptr;
                    }
                }
            }
            idx++;
        }
    }

    /*
     * location of weight = 3.
     */
    unsigned long nLocWeight3 = sizeof(pLocWeight3) / (sizeof(unsigned long) * 2);
    idx = 0UL;
    for (unsigned long i = 0UL; i < _GRID_SZ; i++) {
        for (unsigned long j = 0UL; j < _GRID_SZ; j++) {
            for (unsigned long k = 0UL; k < nLocWeight3; k++) {
                if (pGraph[idx].m_nAdress[0] == pLocWeight3[k][0] && pGraph[idx].m_nAdress[1] == pLocWeight3[k][1] ) {
                    pGraph[idx].m_nWeight = 3;
                    break;
                }
            }
            idx++;
        }
    }

    /*
     * location of weight = 2.
     */
    unsigned long nLocWeight2 = sizeof(pLocWeight2) / (sizeof(unsigned long) * 2);
    idx = 0UL;
    for (unsigned long i = 0UL; i < _GRID_SZ; i++) {
        for (unsigned long j = 0UL; j < _GRID_SZ; j++) {
            for (unsigned long k = 0UL; k < nLocWeight2; k++) {
                if (pGraph[idx].m_nAdress[0] == pLocWeight2[k][0] && pGraph[idx].m_nAdress[1] == pLocWeight2[k][1]) {
                    pGraph[idx].m_nWeight = 2;
                    break;
                }
            }
            idx++;
        }
    }

    Dijkstra(pGraph, pStart, pEnd, ppObstacle, nObstacle);

    return 0;
}