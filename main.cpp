#include <string>
#include <vector>
#include <iostream>

class HamiltonianGraph
{
    struct Line
    {
        int targetNodeIdx;
        int expectation_1 = 0;
    };
    struct Node
    {
        bool searched = false;
        int nodeIdx;
        std::vector<Line> lines;
    };
private:
    std::vector<Node> nodes;
public:
    HamiltonianGraph(const std::vector<std::vector<int>>& t)
    {
        nodes.resize(t.size() + 1);
        for (const auto& pair : t)
        {
            nodes[pair[0]].lines.push_back(Line { pair[1], 0 });
            nodes[pair[1]].lines.push_back(Line { pair[0], 0 });
        }
    }

    int init(int nodeIdx)
    {
        std::cout << "init(" << nodeIdx << ")" << std::endl;
        nodes[nodeIdx].searched = true;
        Node& node = nodes[nodeIdx];
        int count = 0;

        int maxExpectation_1st = 0;
        for (auto& line : node.lines)
        {
            if(nodes[line.targetNodeIdx].searched == false)
            {
                line.expectation_1 = init(line.targetNodeIdx);
                std::cout << "\t" << nodeIdx << " to " << line.targetNodeIdx << " 's exp = " << line.expectation_1 << std::endl;
                maxExpectation_1st = std::max(maxExpectation_1st, line.expectation_1);
                ++count;
            }
        }

        if (count == 0) // nodeIdx is non-root leaf node
        {
            return 1;
        } 
        else // nodeIdx is middle node
        {
            return maxExpectation_1st + 1;
        }
    }

    int findStartPoint(int nodeIdx)
    {
        Node* node = &nodes[nodeIdx];
        int nextNodeIdx;
        do 
        {
            int maxExpectation_1st = 0;
            for (const auto& line : node->lines)
            {
                maxExpectation_1st = std::max(maxExpectation_1st, line.expectation_1);
                if (maxExpectation_1st == line.expectation_1)
                {
                    nextNodeIdx = line.targetNodeIdx;
                }
            }
            node = &nodes[nextNodeIdx];
        } while(node->lines.size() > 1);

        return nextNodeIdx;
    }

    int findMaxTreeSize(int startIdx)
    {
        std::cout << "findMaxTreeSize(" << startIdx << ")" << std::endl;
        int ans = nodes[startIdx].lines[0].expectation_1 + 1;
        std::cout << "\tinitial ans = " << ans << std::endl;

        Node* node = &nodes[startIdx];
        int nextNodeIdx;
        int maxExpectation_2nd_global = 0;
        do 
        {
            int maxExpectation_1st = 0;
            int maxExpectation_2nd = 0;
            for (const auto& line : node->lines)
            {
                if (line.expectation_1 == 0) continue;

                // todo : 처음부터 가장 큰 값이 들어오면 2nd 가 두번째 큰 값을 저장하지 못함
                if (maxExpectation_1st <= line.expectation_1)
                {
                    maxExpectation_2nd = maxExpectation_1st;
                    maxExpectation_1st = line.expectation_1;
                } 
                else 
                {
                    maxExpectation_2nd = std::max(maxExpectation_2nd, line.expectation_1);
                }
                std::cout << "\t\tmaxExpectation_2nd = " << maxExpectation_2nd << std::endl;
                
                if (maxExpectation_1st == line.expectation_1)
                {
                    nextNodeIdx = line.targetNodeIdx;
                }
            }
            node = &nodes[nextNodeIdx];
            maxExpectation_2nd_global = std::max(maxExpectation_2nd_global, maxExpectation_2nd);
            std::cout << "\tmaxExpectation_2nd_global = " << maxExpectation_2nd_global << std::endl;
        } while(node->lines.size() > 1);

        ans += maxExpectation_2nd_global;
        std::cout << "\tfinal ans = " << ans << std::endl;
        return ans;
    }
};

int solution(std::vector<std::vector<int>> t) {
    int answer = 0;
    int startIdx;
    { // find startIdx
        HamiltonianGraph hg(t);
        hg.init(0);
        startIdx = hg.findStartPoint(0);
    }
    { // init with correct startIdx, find answer
        HamiltonianGraph hg2(t);
        hg2.init(startIdx);

        answer = hg2.findMaxTreeSize(startIdx);
    }
    return answer;
}

int main() 
{
    //std::vector<std::vector<int>> t = { { 5,1 }, { 2, 5 }, { 3, 5 }, { 3, 6 }, { 2, 4 }, { 4, 0 } }; // 7
    //std::vector<std::vector<int>> t = { { 2, 5 }, { 2, 0 }, { 3, 2 }, { 4, 2 }, { 2, 1 } }; // 4

    std::vector<std::vector<int>> t = { { 0, 1 }, { 1, 2 }, { 2, 3 }, { 1, 4 }, { 4, 5 }, { 4, 6 } }; // 6
    //std::vector<std::vector<int>> t = { { 0, 1 }, { 1, 2 }, { 0, 3 }, { 3, 4 }, { 3, 5 } }; // 6
    
    std::cout << solution(t) << std::endl;
}