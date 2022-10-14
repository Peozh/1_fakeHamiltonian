#include <string>
#include <vector>
#include <iostream>
#include <tuple>

class HamiltonianGraph
{
    struct Line
    {
        int targetNodeIdx;
        int expectation_oneway = 0;
        int expectation_round = 0;
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
            nodes[pair[0]].lines.push_back(Line { pair[1], 0, 0 });
            nodes[pair[1]].lines.push_back(Line { pair[0], 0, 0 });
        }
    }

    std::tuple<int, int> init(int nodeIdx)
    {
        std::cout << "init(" << nodeIdx << ")" << std::endl;
        nodes[nodeIdx].searched = true;
        Node& node = nodes[nodeIdx];
        int count = 0;

        Line* oneway_1st = nullptr;
        Line* oneway_2nd = nullptr;
        Line* round_1st = nullptr;
        Line* round_2nd = nullptr;
        int maxExp_oneway_1st = 0;
        int maxExp_oneway_2nd = 0;
        int maxExp_round_1st = 0;
        int maxExp_round_2nd = 0;
        for (auto& line : node.lines)
        {
            if(nodes[line.targetNodeIdx].searched == false)
            {
                std::tie(line.expectation_oneway, line.expectation_round) = init(line.targetNodeIdx);
                std::cout << "\t" << nodeIdx << " to " << line.targetNodeIdx << " 's exp(oneway) = " << line.expectation_oneway << std::endl;
                std::cout << "\t" << nodeIdx << " to " << line.targetNodeIdx << " 's exp(round) = " << line.expectation_round << std::endl;
                
                if (maxExp_oneway_1st <= line.expectation_oneway) // oneway
                {
                    oneway_2nd = oneway_1st;
                    maxExp_oneway_2nd = maxExp_oneway_1st;
                    oneway_1st = &line;
                    maxExp_oneway_1st = line.expectation_oneway;
                } 
                else 
                {
                    if (maxExp_oneway_2nd < line.expectation_oneway)
                    {
                        oneway_2nd = &line;
                        maxExp_oneway_2nd = line.expectation_oneway;
                    }
                }
                
                if (maxExp_round_1st <= line.expectation_round) // round
                {
                    round_2nd = round_1st;
                    maxExp_round_2nd = maxExp_round_1st;
                    round_1st = &line;
                    maxExp_round_1st = line.expectation_round;
                } 
                else 
                {
                    if (maxExp_round_2nd < line.expectation_round)
                    {
                        round_2nd = &line;
                        maxExp_round_2nd = line.expectation_round;
                    }
                }
                
                ++count;
            }
        }

        if (count == 0) // nodeIdx is non-root leaf node
        {
            return { 1, 1 };
        } 
        else // nodeIdx is middle node
        {
            if (oneway_1st->targetNodeIdx != round_1st->targetNodeIdx)
            {
                return { maxExp_oneway_1st + 1 + maxExp_round_1st, maxExp_round_1st + 1 };
            }

            if (maxExp_oneway_1st + maxExp_round_2nd >= maxExp_oneway_2nd + maxExp_round_1st)
            {
                return { maxExp_oneway_1st + 1 + maxExp_round_2nd, maxExp_round_1st + 1 };
            }
            else
            {
                return { maxExp_oneway_2nd + 1 + maxExp_round_1st, maxExp_round_1st + 1 };
            }
        }
    }

    int findStartPoint(int nodeIdx)
    {
        std::cout << "findStartPoint(" << nodeIdx << ")" << std::endl;
        Node* node = &nodes[nodeIdx];
        int nextNodeIdx;
        do 
        {
            const Line* oneway_1st = nullptr;
            const Line* oneway_2nd = nullptr;
            const Line* round_1st = nullptr;
            const Line* round_2nd = nullptr;
            int maxExp_oneway_1st = 0;
            int maxExp_oneway_2nd = 0;
            int maxExp_round_1st = 0;
            int maxExp_round_2nd = 0;
            for (const auto& line : node->lines)
            {
                if (maxExp_oneway_1st <= line.expectation_oneway) // oneway
                {
                    oneway_2nd = oneway_1st;
                    maxExp_oneway_2nd = maxExp_oneway_1st;
                    oneway_1st = &line;
                    maxExp_oneway_1st = line.expectation_oneway;
                } 
                else 
                {
                    if (maxExp_oneway_2nd < line.expectation_oneway)
                    {
                        oneway_2nd = &line;
                        maxExp_oneway_2nd = line.expectation_oneway;
                    }
                }
                
                if (maxExp_round_1st <= line.expectation_round) // round
                {
                    round_2nd = round_1st;
                    maxExp_round_2nd = maxExp_round_1st;
                    round_1st = &line;
                    maxExp_round_1st = line.expectation_round;
                } 
                else 
                {
                    if (maxExp_round_2nd < line.expectation_round)
                    {
                        round_2nd = &line;
                        maxExp_round_2nd = line.expectation_round;
                    }
                }
            }
            // find next node
            if (oneway_1st->targetNodeIdx != round_1st->targetNodeIdx) // two best(in each field) candidates are different
            {
                nextNodeIdx = oneway_1st->targetNodeIdx;
            }

            if (maxExp_oneway_1st + maxExp_round_2nd >= maxExp_oneway_2nd + maxExp_round_1st) 
            {
                nextNodeIdx = oneway_1st->targetNodeIdx;
            }
            else
            {
                nextNodeIdx = oneway_2nd->targetNodeIdx;
            }
            node = &nodes[nextNodeIdx];
        } while(node->lines.size() > 1);

        return nextNodeIdx;
    }

    int findMaxTreeSize(int startIdx)
    {
        std::cout << "findMaxTreeSize(" << startIdx << ")" << std::endl;
        int ans = nodes[startIdx].lines[0].expectation_oneway + 1;
        std::cout << "\tans = " << ans << std::endl;
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
    if (false) {
        std::vector<std::vector<int>> t = { { 5,1 }, { 2, 5 }, { 3, 5 }, { 3, 6 }, { 2, 4 }, { 4, 0 } }; // 7 예제
        std::cout << solution(t) << " (should 7)\n" << std::endl;
    }
    if (false){
        std::vector<std::vector<int>> t = { { 2, 5 }, { 2, 0 }, { 3, 2 }, { 4, 2 }, { 2, 1 } }; // 4 예제
        std::cout << solution(t) << " (should 4)\n" << std::endl;
    }
    if (false){
        std::vector<std::vector<int>> t = { { 0, 1 }, { 1, 2 }, { 2, 3 }, { 1, 4 }, { 4, 5 }, { 4, 6 } }; // 7
        std::cout << solution(t) << " (should 7)\n" << std::endl;
        /*
            0 - 1 - 2 - 3
                  - 4 - 5
                      - 6
        */
    }
    if (false){
        std::vector<std::vector<int>> t = { { 0, 1 }, { 1, 2 }, { 0, 3 }, { 3, 4 }, { 3, 5 } }; // 6 
        std::cout << solution(t) << " (should 6)\n" << std::endl;
        /*
            0 - 1 - 2
              - 3 - 4
                  - 5
        */
    }
    if (true){
        std::vector<std::vector<int>> t = { { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 4 }, { 4, 5 } }; // 6 일직선
        std::cout << solution(t) << " (should 6)\n" << std::endl;
        /*
            0 - 1 - 2 - 3 - 4 - 5
        */
    }

    
    if (true){
        std::vector<std::vector<int>> t = { { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 4 }, { 4, 5 }, { 2, 6 }, { 6, 7 }, { 3, 8 }, { 8, 9 } }; // 10
        std::cout << solution(t) << " (should 10)\n" << std::endl;
        /*
            0 - 1 - 2 - 3 - 4 - 5
                    6   8
                    7   9 
        */
    }

    if (true){
        std::vector<std::vector<int>> t = { { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 4 }, { 1, 5 }, { 2, 6 }, { 6, 7 }, { 3, 8 } }; // 9
        std::cout << solution(t) << " (should 9)\n" << std::endl;
        /*
            0 - 1 - 2 - 3 - 4
                5   6   8
                    7    
        */
    }
    
}