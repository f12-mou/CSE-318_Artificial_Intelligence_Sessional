#include <bits/stdc++.h>
using namespace std;
#define fast ios_base::sync_with_stdio(false),cin.tie(0),cout.tie(0);

#define N 5

int exploredManhattan, exploredHamming, expandedManhattan, expandedHamming;
int standardRow[1000];
int standardCol[1000];

int gridNumber;
int dx[5]={1,-1,0, 1, 0};
int dy[5]={1,0, 1, 0, -1}; // Ignore the 0th option

class node
{
public:
    short a[N][N];
    int noSteps=0;  // Number of steps to reach this state from the initial position
    int hcost1=0;   // Manhattan heuristic cost
    int hcost2=0;   // hamming heuristic cost
    int blankRow=0; // the position of blank
    int blankCol=0;
    int n;          // board size
    int cost;       // number of steps + heuristic cost
    int direction;  // difference vector direction from parent

    vector<int>directions;  // to generate the solution on the fly

    node()
    {
        n=gridNumber;
        direction=0;
    }
    void calcManhattanCost()
    {
        hcost1=0;
        for(int i=1;i<=n;i++)
        {
            for(int j=1;j<=n;j++)
            {
                if(a[i][j]!=0)
                {
                    int now=a[i][j];
                    int rowDistance = abs(standardRow[now]-i);
                    int colDistance = abs(standardCol[now]-j);
                    hcost1+=(rowDistance+colDistance);
                }
            }
        }
        //cout<<"here is the manhattan distance "<<hcost1<<endl;
    }
    void calcHammingCost()
    {
        hcost2=0;
        int standardArr[N][N];
        int start=1;
        for(int i=1;i<=n;i++)
        {
            for(int j=1;j<=n;j++)
            {
                standardArr[i][j]=start;
                start++;
            }
        }
        standardArr[n][n]=0;
        for(int i=1;i<=n;i++)
        {
            for(int j=1;j<=n;j++)
            {
                if(a[i][j]!=0)
                {
                    int now=a[i][j];
                    if(standardArr[i][j]!=now)
                    {
                        hcost2++;
                    }
                }
            }
        }
        //cout<<"here is the hamming distance "<<hcost2<<endl;
    }
    bool isSolvable()
    {
        vector<int>vec;
        for(int i=1;i<=n;i++)
        {
            for(int j=1;j<=n;j++)
            {
                if(a[i][j]!=0)
                    vec.push_back(a[i][j]);
            }
        }
        int noInversion=0;
        for(int i=0;i<vec.size();i++)
        {
            for(int j=i+1;j<vec.size();j++)
            {
                if(vec[i]>vec[j])
                {
                    noInversion++;
                }
            }
        }
        if(n%2==0)
        {
            noInversion+=abs(blankRow-n);
        }
        if(noInversion%2==0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool operator <(const node& sec)
    {
        if(hcost1<sec.hcost1)
        {
            return true;
        }
        return false;
    }
    void print()
    {
        for(int i=1;i<=gridNumber;i++)
        {
            for(int j=1;j<=gridNumber;j++)
            {
                cout<<a[i][j]<<" ";
            }
            cout<<endl;
        }

    }

};

struct CompareCost{
    bool operator()(node const& p1, node const& p2)
    {
        return p1.cost >= p2.cost;
    }
};

class Game
{
public:
    int arr[N][N];
    int n;
    Game(int gridNumber) // setting the standard array
    {
        n=gridNumber;
        int start=1;
        for(int i=1;i<=n;i++)
        {
            for(int j=1;j<=n;j++)
            {
                arr[i][j]=start;
                start++;
            }
        }
        arr[n][n]=0;
    }
    bool isSame(node x, node y)
    {
        bool ok=true;
        node temp;
        if(x.directions.empty()==true)
            return false;
        int nowDirection=x.directions[x.directions.size()-1];
        int parBlankRow=x.blankRow+dx[nowDirection]*(-1); // applying the inversion opposite to the child
        int parBlankCol=x.blankCol+dy[nowDirection]*(-1);
        for(int i=1;i<=gridNumber;i++)
        {
            for(int j=1;j<=gridNumber;j++)
            {
                temp.a[i][j]=x.a[i][j];
            }
        }
        int c=x.a[parBlankRow][parBlankCol];
        temp.a[parBlankRow][parBlankCol]=0;
        temp.a[x.blankRow][x.blankCol]=c; // only one cell differs
        for(int i=1;i<=gridNumber;i++)
        {
            for(int j=1;j<=gridNumber;j++)
            {
                if(temp.a[i][j]!=y.a[i][j])
                {
                    ok=false;
                    break;
                }
            }
            if(ok==false)
                break;
        }
        return ok;
    }
    bool isGoal(node root) // checks if it contains the standard array
    {
        bool ok=true;
        for(int i=1;i<=n;i++)
        {
            for(int j=1;j<=n;j++)
            {
                if(arr[i][j]!=root.a[i][j])
                {
                    ok=false;
                    break;
                }
            }
            if(ok==false)
                break;
        }
        return ok;
    }
    node solveManhattan(node root)
    {
        node ansNode; // final destination node
        priority_queue<node, vector<node>, CompareCost> pq; // node will be pushed
        root.noSteps=0;
        root.calcManhattanCost();
        root.cost=root.hcost1+root.noSteps;

        pq.push(root);
        exploredManhattan++;

        while(!pq.empty())
        {
            node nowNode = pq.top();
            pq.pop();
            expandedManhattan++;
            if(isGoal(nowNode))
            {
                ansNode=nowNode;
                break;
            }
            for(int i=1;i<=4;i++)
            {
                int childBlankRow = nowNode.blankRow+dx[i];
                int childBlankCol = nowNode.blankCol+dy[i];
                //cout<<"row col "<<childBlankRow<<" "<<childBlankCol<<endl;
                if(childBlankRow<1 || childBlankRow>gridNumber) continue;
                if(childBlankCol<1 || childBlankCol>gridNumber) continue;
                node temp; // creating the child node
                for(int j=1;j<=gridNumber;j++)
                {
                    for(int k=1;k<=gridNumber;k++)
                    {
                        temp.a[j][k]=nowNode.a[j][k];
                    }
                }
                int c = temp.a[childBlankRow][childBlankCol];
                temp.a[childBlankRow][childBlankCol]=0;
                temp.a[nowNode.blankRow][nowNode.blankCol]=c;
                temp.noSteps=nowNode.noSteps+1;
                temp.blankRow=childBlankRow;
                temp.blankCol=childBlankCol;
                temp.direction=i;
                temp.calcManhattanCost();
                temp.cost=temp.hcost1+temp.noSteps;
                if(nowNode.directions.empty()==false)
                    temp.directions=nowNode.directions;
                temp.directions.push_back(i);

                if(isSame(nowNode, temp)==false){
                pq.push(temp);
                exploredManhattan++;
                }
            }

        }
        return ansNode;

    }

    node solveHamming(node root)
    {
        node ansNode;
        priority_queue<node, vector<node>, CompareCost> pq;
        root.noSteps=0;
        root.calcHammingCost(); // to avoid several calls to functions
        root.cost=root.hcost2+root.noSteps;

        pq.push(root);
        exploredHamming++;

        while(!pq.empty())
        {
            node nowNode = pq.top();
            pq.pop();
            expandedHamming++;
            if(isGoal(nowNode))
            {
                ansNode=nowNode;
                break;
            }
            for(int i=1;i<=4;i++)
            {
                int childBlankRow = nowNode.blankRow+dx[i];
                int childBlankCol = nowNode.blankCol+dy[i];
                //cout<<"row col "<<childBlankRow<<" "<<childBlankCol<<endl;
                if(childBlankRow<1 || childBlankRow>gridNumber) continue;
                if(childBlankCol<1 || childBlankCol>gridNumber) continue;
                node temp;
                for(int j=1;j<=gridNumber;j++)
                {
                    for(int k=1;k<=gridNumber;k++)
                    {
                        temp.a[j][k]=nowNode.a[j][k];
                    }
                }
                int c = temp.a[childBlankRow][childBlankCol];
                temp.a[childBlankRow][childBlankCol]=0;
                temp.a[nowNode.blankRow][nowNode.blankCol]=c;
                temp.noSteps=nowNode.noSteps+1;
                temp.blankRow=childBlankRow;
                temp.blankCol=childBlankCol;
                temp.direction=i;
                temp.calcHammingCost();
                temp.cost=temp.hcost2+temp.noSteps;
                if(nowNode.directions.empty()==false)
                    temp.directions=nowNode.directions;
                temp.directions.push_back(i);

                if(isSame(nowNode, temp)==false){
                pq.push(temp);
                exploredHamming++;
                }
            }

        }
        return ansNode;

    }


    void printSolveManhattan(node root)
    {
        if(root.noSteps==0)
        {
            root.print();
            cout<<endl;
            return;
        }


        node temp;
        int nowDirection=root.directions[root.directions.size()-1];
        int parBlankRow=root.blankRow+dx[nowDirection]*(-1);
        int parBlankCol=root.blankCol+dy[nowDirection]*(-1);
        for(int i=1;i<=gridNumber;i++)
        {
            for(int j=1;j<=gridNumber;j++)
            {
                temp.a[i][j]=root.a[i][j];
            }
        }
        int c=root.a[parBlankRow][parBlankCol];
        temp.a[parBlankRow][parBlankCol]=0;
        temp.a[root.blankRow][root.blankCol]=c;
        temp.noSteps=root.noSteps-1;
        temp.blankRow=parBlankRow;
        temp.blankCol=parBlankCol;
        temp.directions=root.directions;
        temp.directions.pop_back();
        printSolveManhattan(temp);
        root.print();
        cout<<endl;

    }
};

signed main()
{
    fast;
    cin>>gridNumber;
    cout<<"The size of the grid is "<<gridNumber<<" * "<<gridNumber<<endl;
    node root; // Initial State of the game
    int start=1;
    for(int i=1;i<=gridNumber;i++)
    {
        for(int j=1;j<=gridNumber;j++)
        {
            int d;
            cin>>d;
            root.a[i][j]=d;
            if(d==0)
            {
                root.blankRow=i;
                root.blankCol=j;
                root.a[i][j]=0;
            }
            // setting standard row and col number
            standardRow[start]=i;
            standardCol[start]=j;
            start++;
        }
    }

    standardRow[0]=gridNumber;
    standardCol[0]=gridNumber; // o will be at n*n

    if(root.isSolvable()==true)
    {
        cout<<"Possible to Solve"<<endl;

        // Manhattan
        Game game(gridNumber);
        node ans=game.solveManhattan(root);
        cout<<"Number of Steps : "<<ans.noSteps<<endl;
        cout<<"In Manhattan Distance Heuristic --------------"<<endl;
        cout<<"Total explored nodes : "<<exploredManhattan<<endl;
        cout<<"Total expanded nodes : "<<expandedManhattan<<endl;
        game.printSolveManhattan(ans);

        // now hamming
        ans = game.solveHamming(root);
        cout<<"Number of Steps : "<<ans.noSteps<<endl;
        cout<<"In Hamming Distance Heuristic ----------------"<<endl;
        cout<<"Total explored nodes : "<<exploredHamming<<endl;
        cout<<"Total expanded nodes : "<<expandedHamming<<endl;
        game.printSolveManhattan(ans);


    }
    else
    {
        cout<<"Unsolvable puzzle"<<endl;
    }

    return 0;
}


